#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "lzo"
pkg_ver = "2.0.3"

def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "./configure",
        "--prefix=%(pkg_install_dir)s" % env,
        '--disable-static',
        '--enable-shared',
        '--enable-encoding',
        '--with-pic',
        ]
    import sys
    if sys.platform == "darwin":
        cmd.append('--disable-asm')
    self.run(cmd)
    
cmmi_orig_build = build
def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    import sys
    if sys.platform == "darwin":
        env['LDFLAGS'] += ' '+self.cmt(macro_value='shlibflags')

    return cmmi_orig_build(self)
