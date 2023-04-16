# -*- coding: utf-8 -*-
import os
import re
import sys

from notebook.notebookapp import main

if __name__ == "__main__":
    os.chdir(
        os.path.dirname(os.path.realpath(__file__))
    )  # Set the notebooks root to our directory
    sys.argv[0] = re.sub(r"(-script\.pyw?|\.exe)?$", "", sys.argv[0])
    sys.exit(main())
