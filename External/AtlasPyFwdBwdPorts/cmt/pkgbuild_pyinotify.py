#
# 

from PyCmt.pkgbuild.distutils_support import *
import os
import sys

pkg_name= "pyinotify"
pkg_ver = "0.9.1"

if sys.platform == 'darwin':
    # inotify is not available on darwin
    def configure(self):
        return
    def build(self):
        return
    def install(self):
        return

