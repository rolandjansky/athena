#
# 

from PyCmt.pkgbuild.distutils_support import *
import os

pkg_name= "numexpr"
pkg_ver = "1.4.2"

dist_configure = configure
def configure(self):
    sh = self.sh
    msg = self.msg
    env = self.env
    # numexpr/interpreter.c is not C89 compliant...
    env['CFLAGS'] = env['CFLAGS'].replace(' -ansi ', ' -std=c99 ')

    return dist_configure(self)
