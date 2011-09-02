#
# 

from PyCmt.pkgbuild.distutils_support import *

import os, shutil

pkg_name= "h5py"
pkg_ver = "2.0.0"

old_build = build
def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('patching...')
    sh.chdir("%(pkg_build_dir)s" % env)
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "patch",
        "-Np1",
        "-i",
        "%(pkg_root)s/src/h5py.nounicode.support.patch"%env,
        ]
    #self.run(cmd)
    
    cmd = [
        "patch",
        "-Np1",
        "-i",
        "%(pkg_root)s/src/h5py.highlevel.file.exclusive.open.hack.patch"%env,
        ]
    #self.run(cmd)
    
    cmd = [
        "patch",
        "-Np1",
        "-i",
        "%(pkg_root)s/src/h5py.detect_hdf5.hack.patch"%env,
        ]
    #self.run(cmd)
    
    # FIXME: this is b/c of a bug in cython which
    #        generates C++ style comments...
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=c99')
    
    msg.debug('build...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "python",
        "setup.py",
        "build",
        ## FIXME: this assumes hdf5 is always built in the same project
        ## than h5py...
        "--hdf5=%(CMTINSTALLAREA)s/%(CMTCONFIG)s" % env,
        ]
    self.run(cmd)
