#
# 

from PyCmt.pkgbuild.distutils_support import *
import os
import sys

pkg_name= "abc"
pkg_ver = "2.6.1.0"

if sys.version_info[:2] >= (2,6):
    def configure(self):
        return
    def build(self):
        return
    def install(self):
        return
    
