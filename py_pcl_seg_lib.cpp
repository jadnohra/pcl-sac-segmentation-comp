#include <iostream>
#include <chrono>
#include <string>
#include <pybind11/pybind11.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/sample_consensus/sac_model_perpendicular_plane.h>
#include <pcl/filters/extract_indices.h>



int ground_segment_cloud_from_file(
    const std::string& in_file_path,
    const std::string& out_ground_file_path,
    const std::string& out_outliers_file_path,
    const std::string& sac_model,
    const std::string& sac_method,
    const int iter_count,
    const float dist_thresh,
    const int axis_index,
    const float axis_angle_eps
    )
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
  if (pcl::io::loadPCDFile<pcl::PointXYZ> (in_file_path.c_str(), *cloud) == -1) //* load the file
  {
      PCL_ERROR ("Couldn't read file  \n");
      return (-1);
  }

  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
  pcl::SACSegmentation<pcl::PointXYZ> seg;
  seg.setOptimizeCoefficients (true);

  std::map<std::string, int> model_map = 
  {
    {"SACMODEL_PLANE", pcl::SACMODEL_PLANE},
    {"SACMODEL_PERPENDICULAR_PLANE", pcl::SACMODEL_PERPENDICULAR_PLANE}
  };

  std::map<std::string, int> sac_map = 
  {
    {"SAC_RANSAC", pcl::SAC_RANSAC},
    {"SAC_LMEDS", pcl::SAC_LMEDS},
    {"SAC_MSAC", pcl::SAC_MSAC},
    {"SAC_RRANSAC", pcl::SAC_RRANSAC},
    {"SAC_RMSAC", pcl::SAC_RMSAC},
    {"SAC_MLESAC", pcl::SAC_MLESAC},
    {"SAC_PROSAC", pcl::SAC_PROSAC}
  };

  seg.setModelType(model_map[sac_model]);
  seg.setMaxIterations(iter_count);
  seg.setMethodType (sac_map[sac_method]);
  seg.setDistanceThreshold (dist_thresh);

  Eigen::Vector3f axis = Eigen::Vector3f(axis_index == 0 ? 1.0: 0.0,
                                         axis_index == 1 ? 1.0: 0.0,
                                         axis_index == 2 ? 1.0: 0.0); 
  seg.setAxis(axis);
  seg.setEpsAngle( axis_angle_eps ); 

  auto start = std::chrono::high_resolution_clock::now();

  seg.setInputCloud (cloud);
  seg.segment (*inliers, *coefficients);

  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "Segmentation time (millis): " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;

  if (inliers->indices.size () == 0)
  {
    PCL_ERROR ("Could not estimate a planar model for the given dataset.\n");
    return (-1);
  }

  std::cout << "Model coefficients: " << coefficients->values[0] << " " 
                                      << coefficients->values[1] << " "
                                      << coefficients->values[2] << " " 
                                      << coefficients->values[3] << std::endl;

  std::cout << "Model inliers: " << inliers->indices.size () << std::endl;

  // Write to disk
  {
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud (cloud);
    extract.setIndices (inliers);
    extract.setNegative (false);
    pcl::PointCloud<pcl::PointXYZ>::Ptr seg_cloud(new pcl::PointCloud<pcl::PointXYZ>);
    extract.filter (*seg_cloud);
    
    {
        pcl::io::savePCDFileASCII (out_ground_file_path.c_str(), *seg_cloud);
    }
  }

  {
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud (cloud);
    extract.setIndices (inliers);
    extract.setNegative (true);
    pcl::PointCloud<pcl::PointXYZ>::Ptr seg_cloud(new pcl::PointCloud<pcl::PointXYZ>);
    extract.filter (*seg_cloud);
    
    {
        pcl::io::savePCDFileASCII (out_outliers_file_path.c_str(), *seg_cloud);
    }
  }

  return (0);
}

PYBIND11_MODULE(py_pcl_seg_lib, m) {
    m.def("ground_segment_cloud_from_file", &ground_segment_cloud_from_file, "");
}