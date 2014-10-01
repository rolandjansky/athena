import os
import os.path as osp

pkg_name= "ShowerDeconstruction"
pkg_ver = "0.6"
pkg_src = "%(pkg_root)s/src"
pkg_installarea_dir = "%(CMTINSTALLAREA)s/%(CMTCONFIG)s"
pkg_install_dir = "%(pkg_root)s/%(CMTCONFIG)s/pkg-build-install-%(pkg_name)s"

def configure(self):
    return
   
def build(self):
    return

def install(self):
    cmd = "/bin/cp -pRf %(pkg_src)s/lib %(pkg_install_dir)s/" % self.env
    print cmd
    self.run(cmd)

    cmd = "/bin/cp -pRf %(pkg_src)s/include %(pkg_install_dir)s/" % self.env
    print cmd
    self.run(cmd)
