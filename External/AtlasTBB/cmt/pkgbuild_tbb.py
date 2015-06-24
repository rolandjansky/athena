#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "tbb"
pkg_ver = "42_20131003oss"

pkg_src = "%(pkg_root)s/src/%(pkg_name)s%(pkg_ver)s_src.tar.gz"
# defaults from cmmi are fine for those:
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"
#pkg_install_dir = "/tmp/binet/foo-hdf5"

def configure(self):

    env = self.env
    msg = self.msg
    sh  = self.sh
    env['compiler'] = env['compiler'].replace('lcg-', '')
    tmp=env['gcc-version']
    env['compiler'] = env['compiler'].replace('-'+tmp, '')
    pkg_install_dir = self['pkg_install_dir']
    env['CPPFLAGS'] = env["CPPFLAGS"].replace('"','')
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=c99')
    #env['CXXFLAGS'] = env['CXXFLAGS'].replace('-ansi', '-std=c++0x')

    # tbb can't compile otherwise...
    env['CFLAGS'] = env['CFLAGS'].replace('-pedantic', '')
    env['CXXFLAGS'] = env['CXXFLAGS'].replace('-pedantic', '')

    env['cfg'] = 'release'
    env['arch'] = 'x86_64' if 'x86_64' in env['CMTCONFIG'] else 'i686'
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s%(pkg_ver)s" % self.env)
    # fix missing config file for clang
    os.system("sed -e 's#CPLUS = g++#CPLUS = clang++#g;s#CONLY = gcc#CONLY = clang#g' build/linux.gcc.inc > build/linux.clang.inc")

    os.system('patch -p0 < %s/cmt/clang.patch' % self.pkg_root)
    
    cmd = [
        "make",
        "info",
        ]
    self.run(cmd)
    return

def build(self):
    env = self.env
    msg = self.msg
    sh  = self.sh
    env['cfg'] = 'release'
    env['arch'] = 'x86_64' if 'x86_64' in env['CMTCONFIG'] else 'i686'
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s%(pkg_ver)s" % self.env)
    cmd = [
        "make",
        "tbb", "tbbmalloc", "tbbproxy",
        ]
    self.run(cmd)
    return

def install(self):
    env = self.env
    msg = self.msg
    sh  = self.sh
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s%(pkg_ver)s" % self.env)

    import glob
    import os
    import shutil
    import sys

    if 'linux' in sys.platform.lower():
        libs = glob.glob("%(pkg_build_dir)s/%(pkg_name)s%(pkg_ver)s/build/linux_*_release/*.so*"%env)
    elif 'darwin' in sys.platform.lower():
        libs = glob.glob("%(pkg_build_dir)s/%(pkg_name)s%(pkg_ver)s/build/macosx_*_release/*.so*"%env)
    else:
        raise RuntimeError("unknown platform [%s]" % sys.platform)

    if len(libs) <= 0:
        raise RuntimeError("no libraries found to be installed !")
    
    libdir = "%(pkg_install_dir)s/lib" % env
    try:
        if not os.path.exists(libdir):
            os.makedirs(libdir)
    except OSError:
        # typical race condition on os.path...
        pass

    for lib in libs:
        shutil.copy2(lib, libdir)

    hdrdir = "%(pkg_install_dir)s/include" % env
    try:
        if not os.path.exists(hdrdir):
            os.makedirs(hdrdir)
    except OSError:
        # typical race condition on os.path...
        pass

    hdrdir = "%(pkg_install_dir)s/include/tbb" % env
    try:
        if os.path.exists(hdrdir):
            shutil.rmtree(hdrdir, ignore_errors=True)
    except OSError:
        # typical race condition on os.path...
        pass

    shutil.copytree("include/tbb", hdrdir)
