#
# 

from PyCmt.pkgbuild.distutils_support import *
import os
import sys

pkg_name= "pyyaml"
pkg_ver = "3.10"


def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    # for python-eggs
    import os.path as osp
    os.environ['PYTHON_EGG_CACHE'] = osp.join(
        "%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env,
        '.python-eggs'
        )
    
    msg.debug('build...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "python",
        "setup.py",
        "--without-libyaml",
        "build"
        ]
    self.run(cmd)

def install(self):
    sh  = self.sh
    msg = self.msg
    env = self.env
    
    msg.debug('install...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    platlib = "%(pkg_install_dir)s/lib/python" % env + \
              python_config_version
    purelib = "%(pkg_install_dir)s/lib/python" % env + \
              python_config_version
    bindir  = "%(pkg_install_dir)s/bin" % env
    
    import os
    for d in (platlib, purelib, bindir):
        if not os.path.exists(d):
            os.makedirs(d)
    cmd = [
        "python",
        "setup.py",
        "--without-libyaml",
        "install",
        "--force",
        "--root=%(pkg_install_dir)s" % env,
        '--install-platlib=lib/python%s' % python_config_version,
        '--install-purelib=lib/python%s' % python_config_version,
        '--install-scripts=bin',
        '--install-data=lib/python%s' % python_config_version,
        ]
    self.run(cmd)
    
