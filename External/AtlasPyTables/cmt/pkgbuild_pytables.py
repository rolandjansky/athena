#
# 

from PyCmt.pkgbuild.distutils_support import *
import os

pkg_name= "tables"
pkg_ver = "2.3"

def configure(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    includes = self.cmt(macro_value="includes")
    ppcmd = self.cmt(macro_value="ppcmd")
    includes = [inc.strip() for inc in includes.split(ppcmd) if inc.strip()]
    includes = ":".join(includes).replace('"','')
    env['includes'] = includes

    msg.verbose("includes: %s", includes)

    # ordered set...
    _includes = includes.split(":")
    includes = []
    for inc in _includes:
        if inc not in includes:
            includes.append(inc)
            
    hdf5_dir= None
    bz2_dir = None
    lzo_dir = None
    for d in includes:
        msg.verbose("--> [%s]", d)
        if osp.exists(d):
            if osp.exists(osp.join(d, "hdf5.h")):
                hdf5_dir = osp.dirname(d)
            if osp.exists(osp.join(d, "bzlib.h")):
                bz2_dir  = osp.dirname(d)
            if osp.exists(osp.join(d, "lzo", "lzoconf.h")):
                lzo_dir  = osp.dirname(d)
    msg.debug("HDF5_DIR:  [%s]", hdf5_dir)
    msg.debug("LZO_DIR:   [%s]", lzo_dir)
    msg.debug("BZIP2_DIR: [%s]", bz2_dir)
    assert hdf5_dir, "could not find hdf5_dir"
    assert lzo_dir, "could not find lzo_dir"
    assert bz2_dir, "could not find bz2_dir"

    env["HDF5_DIR"] = hdf5_dir
    env["LZO_DIR"]  = lzo_dir
    env["BZIP2_DIR"]= bz2_dir
    
    from textwrap import dedent
    site_cfg = open("site.cfg", "a")
    site_cfg.write(dedent("""\
    [DEFAULT]
    library_dirs = %(LD_LIBRARY_PATH)s:/usr/lib:/lib
    include_dirs = %(includes)s
    fcompiler    = %(FC)s
    """ % env))
    site_cfg.flush()
    site_cfg.close()
    del site_cfg

    # FIXME: hack needed for pytables. it prepends a spurious '-l' in front
    #        of all -lsomelib !
    env['LIBS'] = ""

    # tables/numexpr/interpreter.c is not C89 compliant...
    env['CFLAGS'] = env['CFLAGS'].replace(' -ansi ', ' -std=c99 ')
    
    msg.debug('patching...')
    cmd = [
        "patch",
        "-Np1",
        "-i", "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.patch"%env
        ]
    #self.run(cmd)
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
        "--hdf5=%(HDF5_DIR)s" % env,
        "--lzo=%(LZO_DIR)s" % env,
        "--bzip2=%(BZIP2_DIR)s" % env,
        ]
    self.run(cmd)

