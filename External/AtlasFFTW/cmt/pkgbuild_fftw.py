from PyCmt.pkgbuild.cmmi import *
from os import system

pkg_name= "fftw"
pkg_ver = "3.3.3"

def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env
    pkg_install_dir = self['pkg_install_dir']
    
    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    os.system('pwd')
    cmd = [
        './configure',
        '--prefix=%s'%pkg_install_dir,
        '--enable-shared',
        '--enable-float',
        ]
    
    self.run(cmd)
    
