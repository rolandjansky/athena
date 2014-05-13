#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "gperftools"
pkg_ver = "2.2"
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
    #env['CXXFLAGS'] = env['CXXFLAGS'].replace('-ansi', '-std=c++0x')

    # gperftools-2.0 can't compile otherwise...
    env['CFLAGS'] = env['CFLAGS'].replace('-pedantic', '')
    env['CXXFLAGS'] = env['CXXFLAGS'].replace('-pedantic', '')
    
    sh = self.sh
    # clean-up install-dir... (hackish!!)
    #from glob import glob
    #map(sh.remove, glob("%(pkg_install_dir)s/lib/libhdf5*" % env))

    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % self.env)

    self.env['F9X'] = self.env['FC']

    # for k in ('CPPFLAGS','CXXFLAGS', 'CFLAGS'):
    #     print "%s: %s" % (k, env[k])

    cmd = [
        "./configure",
        '--prefix=%s'%pkg_install_dir,
        '--enable-shared',
        '--enable-static',
        ]
    self.run(cmd)

