import os
from  distutils import dir_util

pkg_name= "eigen"
pkg_ver = "3.1.3"
pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.bz2"
pkg_installarea_dir = "%(CMTINSTALLAREA)s/%(CMTCONFIG)s/include"
pkg_install_dir = "%(pkg_root)s/%(CMTCONFIG)s/pkg-build-install-%(pkg_name)s"

def configure(self):
    return
    
def build(self):
    sh = self.sh
    msg = self.msg
    env = self.env
    os.remove('%(pkg_name)s-%(pkg_ver)s.tar.bz2' % self.env)
    dir_util.copy_tree(self.sh.getcwd(), self.env['pkg_install_dir'])
    os.system('cd %s; patch -p0 < %s/cmt/placement-new.patch' % (self.env['pkg_install_dir'], self.pkg_root))
    os.system('cd %s; patch -p0 < %s/cmt/typedef-warnings.patch' % (self.env['pkg_install_dir'], self.pkg_root))
    os.system('cd %s; patch -p0 < %s/cmt/register-warnings.patch' % (self.env['pkg_install_dir'], self.pkg_root))
    return

def install(self):
    return

