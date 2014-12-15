#
# 

from PyCmt.pkgbuild.distutils_support import *
import os

pkg_name= "rootplot"
pkg_ver = "2.2.1"

ds_configure = configure
def configure(self):
    env = self.env
    sh = self.sh
    msg = self.msg
    
    ## msg.debug('patching...')
    ## sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    ## cmd = [
    ##     "patch",
    ##     "-Np1",
    ##     "-i", "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.patch"%env
    ##     ]
    ## self.run(cmd)
    return ds_configure(self)

