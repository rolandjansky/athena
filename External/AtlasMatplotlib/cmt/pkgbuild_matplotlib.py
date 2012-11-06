#
# 

from PyCmt.pkgbuild.distutils_support import *
import os

pkg_name= "matplotlib"
pkg_ver = "1.0.1"

dist_configure = configure
def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    # hack for PyCXX which mixes C++-style comments in C files
    env['CFLAGS'] = env['CFLAGS'].replace('-ansi', '-std=c99')
    env['CXXFLAGS'] = env['CXXFLAGS'].replace(
        '-Wstrict-prototypes', '').replace(
        '-std=c99', '-ansi')

    includes = self.cmt(macro_value="includes")
    ppcmd = self.cmt(macro_value="ppcmd")
    includes = [inc.strip() for inc in includes.split(ppcmd) if inc.strip()]
    includes = ":".join(includes).replace('"','')
    env['includes'] = includes

    # fix for macosx/freetype2...
    import sys
    if sys.platform == 'darwin':
        env['CPPFLAGS'] += " -I/usr/X11/include -I/usr/X11/include/freetype2"
        env['LDFLAGS']  += " -L/usr/X11/lib"
        
    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    msg.debug('patching...')
    cmd = [
        "patch",
        "-Np1",
        "-i", "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.patch"%env
        ]
    #self.run(cmd)
    
    site_cfg = open(osp.join(sh.getcwd(),'setup.cfg'), 'a')
    env['macosx'] = (sys.platform=='darwin')
    site_cfg.write ("""\
[DEFAULT]
library_dirs = %(LD_LIBRARY_PATH)s:/usr/lib:/lib
include_dirs = %(includes)s
fcompiler    = %(FC)s
    
[provide_packages]
## Date/timezone support:
pytz = True
dateutil = True

[gui_support]
qt4agg = True
tkagg = False
wxagg = False
macosx = %(macosx)s

[rc_options]
backend = Qt4Agg
# the following is not needed anymore
#numerix = numpy
"""%env)
    del env['macosx']
    site_cfg.flush()
    site_cfg.close()
    del site_cfg

    return

import sys
if sys.platform == 'darwin':
    # use mpl from lcg
    def configure(self):
        return
    def build(self):
        return
    def install(self):
        return

