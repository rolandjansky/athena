#
# 

import os
import os.path as osp

pkg_name= "bzip2"
pkg_ver = "1.0.5"
pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.gz"
pkg_installarea_dir = "%(CMTINSTALLAREA)s/%(CMTCONFIG)s"
pkg_install_dir = "%(pkg_root)s/%(CMTCONFIG)s/pkg-build-install-%(pkg_name)s"
#pkg_install_dir = "/tmp/binet/foo-bzip2"

pkg_patch = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.patch"

def configure(self):
    sh = self.sh
    msg = self.msg
    env = self.env
    pkg_patch = open(self.env['pkg_patch'])
    msg.debug("applying patch... [%s]", pkg_patch.name)
    self.run('patch -p0', stdin=pkg_patch)
    
def build(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    sh.chdir("%(pkg_name)s-%(pkg_ver)s"%env)
    
    env['MY_LDFLAGS'] = env['LDFLAGS']
    env['MY_CFLAGS']  = env['CFLAGS']
    env['MY_CC'] = env['CC']

    # macosx patch
    import sys
    if sys.platform == "darwin":
        env['MY_LDFLAGS'] += ' '+self.cmt(macro_value='shlibflags')
        
    cmd = "make -f Makefile-libbz2_so"
    self.run(cmd)
    sh.symlink('libbz2.so.1.0', 'libbz2.so.1')
    sh.symlink('libbz2.so.1',   'libbz2.so')
    
    cmd = "make bzip2recover libbz2.a"
    self.run(cmd)

def install(self):
    cmd = "make install PREFIX=%(pkg_install_dir)s"%self.env
    self.run(cmd)

    ## also install the shared objects
    from glob import glob
    self.env['_libbz2_files'] = ' '.join(glob("libbz2.so*"))
    cmd = "/bin/cp -pRf %(_libbz2_files)s %(pkg_install_dir)s/lib" % self.env
    self.run(cmd)
