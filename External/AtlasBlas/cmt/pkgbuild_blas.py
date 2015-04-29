from PyCmt.pkgbuild.cmmi import *

pkg_name= "blas"
pkg_ver = "20070405"

# defaults from cmmi are fine for those:
#pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.gz"
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"
#pkg_install_dir = "/tmp/binet/foo-hdf5"

def configure(self):
    env = self.env
    if not 'dbg' in env['CMTCONFIG']:
        env['FCFLAGS'] += ' -funroll-all-loops -O3'
    if '-m32' in self.cmt(macro_value='cc'):
        env['FC'] += ' -m32'
    return

def build(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    msg.debug('build...')

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env)
    if sh.path.exists("make.inc"):
        sh.remove("make.inc")
    sh.symlink("%(pkg_root)s/src/make.inc"%env, "make.inc")
    
    cmd = [
        "make",
        "-j%i"%cpu_count(),
        "OPTS=%(FCFLAGS)s"%env,
        ]
    self.run(cmd)

def install(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    msg.debug('install...')

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env)
    libfile = "libblas.so.3.0.3" # FIXME: hardcoded DSO extension !
    from shutil import copy2
    dest = osp.join("%(pkg_dest_dir)s/lib/"%env,
                    osp.basename(libfile))
    if not osp.exists(osp.dirname(dest)):
        os.makedirs(osp.dirname(dest))
    copy2(libfile, dest)
    sh.chmod(dest, 0755)

    
    sh.safe_symlink(libfile, "%(pkg_dest_dir)s/lib/libblas.so"%env)
    sh.safe_symlink(libfile, "%(pkg_dest_dir)s/lib/libBLAS.so"%env)
    sh.safe_symlink(libfile, "%(pkg_dest_dir)s/lib/libblas.so.3"%env)
