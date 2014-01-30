#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "libunwind"
pkg_ver = "1.1"
# defaults from cmmi are fine for those:
#pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.gz"
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"
#pkg_install_dir = "/tmp/binet/foo-hdf5"

def configure(self):
    env = self.env
    msg = self.msg
    pkg_install_dir = self['pkg_install_dir']
    env['CPPFLAGS'] = env["CPPFLAGS"].replace('"','')
    # gnu89 is the only switch which works on slc5 and slc6...
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=gnu89') \
                    + ' -U_FORTIFY_SOURCE'
    
    sh = self.sh
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % self.env)
    cmd = [
        "./configure",
        '--prefix=%s'%pkg_install_dir,
        '--host=%s' % env['pkg_host_triple'],
        '--enable-shared',
        '--enable-static',
        ]
    self.run(cmd)

def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('make...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    cmd = "make"
    self.run(cmd)
