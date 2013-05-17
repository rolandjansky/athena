#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "hdf5"
pkg_ver = "1.8.10"
# defaults from cmmi are fine for those:
#pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.gz"
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"
#pkg_install_dir = "/tmp/binet/foo-hdf5"

def configure(self):
    env = self.env
    msg = self.msg
    pkg_install_dir = self['pkg_install_dir']
    env['CPPFLAGS'] = env["CPPFLAGS"].replace('"','')
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=c99')
    env['CXXFLAGS'] = env['CXXFLAGS'].replace('-ansi', '-std=c++0x')
    
    sh = self.sh
    # clean-up install-dir... (hackish!!)
    from glob import glob
    map(sh.remove, glob("%(pkg_install_dir)s/lib/libhdf5*" % env))

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % self.env)
    self.env['F9X'] = self.env['FC']

    # for k in ('CPPFLAGS','CXXFLAGS', 'CFLAGS'):
    #     print "%s: %s" % (k, env[k])

    if 'icc' in env['CC']:
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd383','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd981','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd1418','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd1419','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-we1011','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-we117','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd82','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd522','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd444','')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace('-wd304','')

        env['CFLAGS'] = env['CFLAGS'].replace('-wd383','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd981','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd1418','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd1419','')
        env['CFLAGS'] = env['CFLAGS'].replace('-we1011','')
        env['CFLAGS'] = env['CFLAGS'].replace('-we117','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd82','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd522','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd444','')
        env['CFLAGS'] = env['CFLAGS'].replace('-wd304','')
        

    cmd = [
        "./configure",
        '--prefix=%s'%pkg_install_dir,
        '--enable-hl',
        '--enable-linux-lfs',
        '--enable-direct-vfd',
        '--enable-production',
        # threadsafe is incompatible with enable-cxx...
        #'--enable-threadsafe',
        '--with-pthread',
        '--with-zlib',
        '--disable-static',
        '--enable-shared',
        '--enable-cxx',
        '--disable-fortran',
        '--with-szlib',
        ]
    self.run(cmd)

# from cmmi
cmmi_install = install
def install(self):
    """wrap up to handle mysterious debug symbols...
    """

    cmmi_install(self)

    sh  = self.sh
    msg = self.msg
    env = self.env

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    if '-dbg' not in env['CMTCONFIG']:
        from glob import glob
        import sys
        libfiles = glob("%(pkg_install_dir)s/lib/*.so*" % env)
        libfiles.extend(glob("%(pkg_install_dir)s/lib/*.a*" % env))
        if sys.platform == "darwin":
            if len(libfiles) > 0:
                cmd = "strip -S -x %s" % ' '.join(libfiles)
                self.run(cmd)
        else:
            if len(libfiles) > 0:
                cmd = "strip --strip-debug %s" % ' '.join(libfiles)
                self.run(cmd)
    

    
