#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "szip"
pkg_ver = "2.1"

def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    orig_cflags = env['CFLAGS']
    orig_cxxflags = env['CXXFLAGS']
    
    # patch for macosx...
    import sys
    if sys.platform == 'darwin':
        env['CFLAGS'] = env['CFLAGS'].replace(' -g', ' ')
        env['CXXFLAGS'] = env['CXXFLAGS'].replace(' -g', ' ')

    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        './configure',
        '--prefix=%(pkg_install_dir)s'%env,
        '--disable-static',
        '--enable-shared',
        '--enable-encoding',
        '--with-pic',
        ]
    self.run(cmd)

    if sys.platform == 'darwin':
        env['CFLAGS'] = orig_cflags
        env['CXXFLAGS'] = orig_cxxflags
