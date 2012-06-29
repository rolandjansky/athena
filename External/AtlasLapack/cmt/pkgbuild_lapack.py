from PyCmt.pkgbuild.cmmi import *

pkg_name= "lapack"
# FIXME:
# we are stuck w/ this version until we can ditch gcc34:
#  only gfortran is supported by lapack-version >= 3.2
pkg_ver = "3.1.1" 

# defaults from cmmi are fine for those:
#pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.gz"
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"

def configure(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    msg.debug('patch...')
    if not 'dbg' in env['CMTCONFIG']:
        env['FCFLAGS'] += ' -funroll-all-loops -O3'
    if '-m32' in self.cmt(macro_value='cc'):
        env['FC'] += ' -m32'
        
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env)

    if sh.path.exists("make.inc"):
        sh.remove("make.inc")
    sh.symlink("%(pkg_root)s/src/make.inc"%env, "make.inc")

    cmd = [
        "patch",
        "-Np1",
        "-i", "%(pkg_root)s/src/blas-link.patch"%env
        ]
    self.run(cmd)
    
    return

def build(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    msg.debug('build...')

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env)
    
    self.run("make clean")

    cmd = [
        "make",
        "lib",
        "-j%i" % cpu_count(),
        "OPTS=%(FCFLAGS)s"%env,
        ]
    self.run(cmd)

def install(self):
    sh = self.sh
    env = self.env
    msg = self.msg
    msg.debug('install...')

    from glob import glob
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s"%env)

    from shutil import copy2
    for libfile in glob("*.so*"): # FIXME: hardcoded DSO extension !
        dest = osp.join("%(pkg_dest_dir)s/lib/"%env,
                        osp.basename(libfile))
        if not osp.exists(osp.dirname(dest)):
            os.makedirs(osp.dirname(dest))
        copy2(libfile, dest)
        sh.chmod(dest, 0755)

    sh.safe_symlink("liblapack.so.3",
                    "%(pkg_dest_dir)s/lib/liblapack.so"%env)
    sh.safe_symlink("liblapack.so",
                    "%(pkg_dest_dir)s/lib/libLAPACK.so"%env)
    sh.safe_symlink("libtmglib.so.3",
                    "%(pkg_dest_dir)s/lib/libtmglib.so"%env)

