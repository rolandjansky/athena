#
# 

from PyCmt.pkgbuild.distutils_support import *
import os
import sys

pkg_name= "collections"
pkg_ver = "1.1"

if sys.version_info[:2] >= (2,6):
    def configure(self): return
    def build(self): return
    def install(self): return
    
