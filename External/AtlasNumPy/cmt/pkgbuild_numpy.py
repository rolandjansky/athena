#
# 

from PyCmt.pkgbuild.distutils_support import *
import os

pkg_name= "numpy"
pkg_ver = "1.6.1"

def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    env['FC'] = env['FC'].replace('-m32','').strip()
    env['F77']= env['FC']
    env['LDFLAGS'] += " ".join([self.cmt(macro_value="Python_linkopts"),
                               "-shared"])

    # FIXME
    # setup.py does not honour the cross-compilation
    # correctly, at least at link time...
    if '-m32' in self.cmt(macro_value='cppflags'):
        env['CC'] += ' -m32'
        env['CXX'] += ' -m32'

    env['CFLAGS'] += ' -std=c99'
    
    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    includes = self.cmt(macro_value="includes")
    ppcmd = self.cmt(macro_value="ppcmd")
    includes = [inc.strip() for inc in includes.split(ppcmd) if inc.strip()]
    includes = ":".join(includes).replace('"','')
    env['includes'] = includes
    
    from textwrap import dedent
    site_cfg = open("site.cfg", "a")
    site_cfg.write(dedent("""\
    [DEFAULT]
    library_dirs = %(LD_LIBRARY_PATH)s:/usr/lib:/lib
    include_dirs = %(includes)s
    fcompiler    = %(FC)s
    
    [blas_opt]
    libraries = blas, f77blas, cblas, atlas

    [lapack_opt]
    libraries = lapack, f77blas, cblas, atlas
    """ % env))
    site_cfg.flush()
    site_cfg.close()
    del site_cfg

    msg.debug('patching...')
    cmd = [
        "patch",
        "-Np1",
        "-i", "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.patch"%env
        ]
    self.run(cmd)
    
    return

def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('build...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "python",
        "setup.py",
        "build",
        "--fcompiler=%(FC)s" % env
        ]
    self.run(cmd)


## hack for darwin: use the numpy version from lcg (which is lagging)
import sys
if sys.platform == 'darwin':
    def configure(self):
        return
    def build(self):
        return
    def install(self):
        return

