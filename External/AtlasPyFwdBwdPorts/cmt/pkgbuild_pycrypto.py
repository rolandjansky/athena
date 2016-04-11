#
# 

from PyCmt.pkgbuild.distutils_support import *
import os
import sys

pkg_name= "pycrypto"
pkg_ver = "2.1.0"

dist_configure = configure
def configure(self):
    env = self.env
    # there are C++-style comments in various C files...
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=c99')
    if env['CC'].find ('clang')>=0 or env['CC'].find ('icc')>=0:
        env['CFLAGS'] = env['CFLAGS'] + ' -DLTC_NO_ASM '
    return dist_configure(self)
