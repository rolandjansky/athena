#!/usr/bin/env python
# @file autoconf-build.py
# @purpose: a little python script to integrate autotools-based projects into
#           CMT-based ones
# @author: Sebastien Binet <binet@cern.ch>
# @date:   June 2009
from __future__ import with_statement

__doc__ = """\
a little python script to integrate autotools-based projects into CMT-based ones.
"""

__author__ = 'Sebastien Binet <binet@cern.ch>'
__version__ = '$Revision: 1.2 $'


### stdlib imports ------------------------------------------------------------
import sys
import os
import os.path as osp
import subprocess
###

try:
    from PyUtils.Logging import logging
except ImportError:
    import logging

### globals -------------------------------------------------------------------
msg = None

def prepare_build(options):

    def monkey_patch(path):
        if not osp.exists(path): os.makedirs(path)
        # fix freaking CMT 'feature': make sure the path is in PYTHONPATH
        # *especially* if it didn't exist when cmt/setup.sh was run
        pypath = os.environ['PYTHONPATH'].split(os.pathsep)
        pypath.append(path) # put it at the end, to not confuse anybody
        os.environ['PYTHONPATH']=os.pathsep.join(pypath)
        
    for p in (options.platlib_dir, options.purelib_dir):
        msg.debug('monkey patching [%s]', p)
        monkey_patch(p)
    del monkey_patch

    
    import atexit
    def orig_cwd(cwd):
        os.chdir(cwd)
    atexit.register(orig_cwd, os.getcwd())
    del orig_cwd

    # start afresh...
    import shutil
    try:
        if osp.exists(options.build_dir):
            shutil.rmtree(options.build_dir)
    except Exception,err:
        msg.warning(err)
    
    if not osp.exists(options.build_dir):
        os.makedirs(options.build_dir)

    dst = osp.join(options.build_dir,options.name)
    if osp.exists(dst):
        try:
            shutil.rmtree(dst)
        except Exception,err:
            msg.warning(err)
            pass
        
    shutil.copytree(src=options.top_dir,
                    dst=dst,
                    symlinks=True)
    os.chdir(dst)

def _mkstemp(*args, **kwargs):
    import tempfile
    fd, fname = tempfile.mkstemp(*args, **kwargs)
    import os
    os.close(fd)
    if os.path.exists(fname):
        os.remove(fname)
    return open(fname, 'w')

def _mkdtemp(*args, **kwargs):
    """wrapper for tempfile.mkdtemp to register the removal of the directory
    """
    import tempfile
    dir_name = tempfile.mkdtemp(*args, **kwargs)
    import atexit, shutil
    ignore_errors = True
    atexit.register(shutil.rmtree, dir_name, ignore_errors)
    return dir_name

def _uncompress(fname, outdir, msg):
    """uncompress the file `fname` under the directory `outdir`
    """
    import os
    assert os.access(fname, os.R_OK)
    fname = os.path.abspath(os.path.realpath(fname))
    if not os.path.exists(outdir):
        os.makedirs(outdir)
    orig_dir = os.getcwd()
    try:
        os.chdir(outdir)
        ext = os.path.splitext(fname)[1][1:] # drop the dot
        if ext in ('gz', 'bz2'):
            import tarfile
            f = tarfile.open(fname, 'r:%s'%ext)
            f.extractall()
        else:
            err = 'extension [%s] not handled (yet?)' % ext
            msg.error(err)
            raise ValueError(err)
    finally:
        os.chdir(orig_dir)
        
def build(options):
    msg = options.msg
    run = subprocess.check_call
    #run = subprocess.call
    stdout = _mkstemp(prefix='autoconf-logfile-%s-'%options.name,
                      suffix='.txt')
    msg.info('building [%s] (logfile %s)', options.name, stdout.name)
    import functools, os
    run = functools.partial(run,
                            stdout=stdout,
                            stderr=stdout,
                            )
    prepare_build(options)

    configure = [ './configure' ]
    
    msg.debug('running \'configure\'...')
    cfg_options = ['--prefix=%s'%options.install_dir]
    if options.configure_options:
        cfg_options.extend(options.configure_options)
        msg.debug('using these additional configure options: %r',
                  cfg_options)
        pass
    
    run(configure + cfg_options,  env=os.environ)

    msg.debug('building...')
    run('make -j2'.split(), env=os.environ)

    msg.debug('installing...')
    run('make install'.split(), env=os.environ)

    # if we are here then everything went fine
    import os
    stdout.close()
    if not options.keep_logfiles:
        import atexit
        atexit.register(os.remove, stdout.name)
    return 0

def main():
    from optparse import OptionParser
    parser = OptionParser(usage='usage: %prog --name=<name> --src-dir=<dir-name> --install-dir=<install-dir>')
    p = parser.add_option
    p('--name',
      dest='name')
    p('--src-dir',
      dest='top_dir',
      help='the top level directory containing the \'configure\' file')
    p('--install-dir',
      dest='install_dir',
      help='the install directory name')
    p('--configure-options',
      default=None,
      dest='configure_options',
      help='additional configure options')
    p('-l','--lvl',
      dest='lvl',
      default='INFO',
      help='logging level (%default)')
    p('--keep-logfiles',
      dest='keep_logfiles',
      default=True,
      action ="store_true",
      help = "switch to keep or not the logfiles of the builds")
    
    (options, args) = parser.parse_args()
    if (options.top_dir is None or
        options.install_dir is None):
        parser.print_help()
        return 1

    # retrieve build-environment with CMT configuration
    from PyUtils.Cmt import CmtWrapper
    cmt = CmtWrapper()
    fetch = cmt.show
    cmt_env = {
        'CMTINSTALLAREA' : fetch(macro_value='CMTINSTALLAREA'),
        'LDFLAGS' : fetch(macro_value='pkg_autoconf_ldflags'),
        'LIBS'    : fetch(macro_value='pkg_autoconf_libs'),
        'CC'      : fetch(macro_value='pkg_autoconf_cc'),
        'CFLAGS'  : fetch(macro_value='pkg_autoconf_cflags'),
        'CXX'     : fetch(macro_value='pkg_autoconf_cxx'),
        'CXXFLAGS': fetch(macro_value='pkg_autoconf_cxxflags'),
        'FC'      : fetch(macro_value='pkg_autoconf_fc'),
        'FCFLAGS' : fetch(macro_value='pkg_autoconf_fcflags'),
        'CPPFLAGS': fetch(macro_value='pkg_autoconf_cppflags'),
        }
    cmt_env['CFLAGS'] = cmt_env['CFLAGS'].replace('-Wno-deprecated', '')
    cmt_env['CFLAGS'] = cmt_env['CFLAGS'].replace('-Woverloaded-virtual', '')
    cmt_env['LDFLAGS'] = cmt_env['LDFLAGS'].replace('-Wl,--as-needed -Wl,--no-undefined', '')
    cmt_env['F77'] = cmt_env['FC']
    os.environ.update(cmt_env)

    global msg
    msg = logging.getLogger('AutoConf')
    msg.setLevel(getattr(logging, options.lvl))
    
    osp = os.path
    top_dir = osp.expandvars(osp.expanduser(options.top_dir))
    top_dir = osp.abspath(top_dir)

    if not osp.isdir(top_dir):
        # is it a tar.gz-like file ?
        if osp.isfile(top_dir):
            # then uncompress somewhere...
            src_file = top_dir
            top_dir = _mkdtemp(prefix='autoconf-top_dir-%s-'%options.name)
            _uncompress(src_file, top_dir, msg)
            if not ('configure' in os.listdir(top_dir)):
                top_dir = osp.join(top_dir, os.listdir(top_dir)[0])
                assert 'configure' in os.listdir(top_dir)
        else:
            msg.error('invalid --src-dir directory [%s] !', top_dir)
            return 2
    else:
        if not osp.exists(top_dir):
            msg.error('no such directory [%s]', top_dir)
            return 2

    install_dir = osp.expandvars(osp.expanduser(options.install_dir))
    install_dir = osp.abspath(install_dir)
    
    if not osp.exists(install_dir):
        msg.debug('creating directory [%s]', install_dir)
        os.makedirs(install_dir)

    options.install_dir = install_dir
    options.top_dir = top_dir
    options.msg = msg
    options.cmt = cmt

    options.installarea = fetch(macro_value='CMTINSTALLAREA')
    options.cmtconfig   = fetch(macro_value='tag')
    
    options.bin_dir = osp.join(options.installarea, 'share', 'bin')
    options.platlib_dir = osp.join(options.installarea,
                                   options.cmtconfig,
                                   'lib',
                                   'python%s.%s'%sys.version_info[:2])
    options.purelib_dir = osp.join(options.installarea, 'python')

    pkg_name = fetch(macro_value='package')
    if options.name is None:
        options.name = osp.basename(options.top_dir)
    options.build_dir = osp.join(fetch(macro_value='%s_root'%pkg_name),
                                 fetch(macro_value='tag'),
                                 options.name+'-build')

    if options.configure_options:
        options.configure_options = options.configure_options.split()
        msg.debug('using additional configure options: %r',
                  options.configure_options)
        pass
    
    try:
        sc = 3
        sc = build(options)
    except subprocess.CalledProcessError, err:
        msg.error(err)
        sc = 4
    except Exception, err:
        raise
    return sc

if __name__ == '__main__':
    sc = main()
    import sys
    sys.exit(sc)
    
    
