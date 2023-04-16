load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

rules_python_version = "0.13.0"

http_archive(
    name = "rules_python",
    sha256 = "090bfe913d05878db759cdab77061042ff826c3a96b8853aa695405f8c992af5",
    strip_prefix = "rules_python-{}".format(rules_python_version),
    url = "https://github.com/bazelbuild/rules_python/archive/{}.zip".format(rules_python_version),
)

load("@rules_python//python:repositories.bzl", "python_register_toolchains")

python_register_toolchains(
    name = "python3_8",
    python_version = "3.8",
)

load("@python3_8//:defs.bzl", "interpreter")
load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "py_deps",
    python_interpreter_target = interpreter,
    requirements_lock = "//python:requirements_lock.txt",
)

load("@py_deps//:requirements.bzl", "install_deps")

install_deps()


http_archive(
    name = "rules_pcl",
    url = "https://github.com/jadnohra/rules_pcl/archive/v1.1.0c.tar.gz",
    strip_prefix = "rules_pcl-1.1.0c",
)

load("@rules_pcl//bzl:repositories.bzl", "pcl_repositories")
pcl_repositories()

# NOTE: This must be loaded after the call to pcl_repositories().
load("@rules_pcl//bzl:init_deps.bzl", "pcl_init_deps")
pcl_init_deps()


http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-fc56ce8a8b51e3dd941139d329b63ccfea1d304b",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/fc56ce8a8b51e3dd941139d329b63ccfea1d304b.zip"],
)
# We still require the pybind library.
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.10.4",
  urls = ["https://github.com/pybind/pybind11/archive/v2.10.4.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(
    name = "local_config_python",
    python_interpreter_target = interpreter,
)
