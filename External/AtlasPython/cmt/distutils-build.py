#!/usr/bin/env python
# @file distutils-build.py
# @purpose: a little python script to integrate distutils-based projects into
#           CMT-based ones
# @author: Sebastien Binet <binet@cern.ch>
# @date:   April 2009
from __future__ import with_statement

__doc__ = """\
a python script to integrate distutils-based projects into CMT-based ones.
"""

__author__ = 'Sebastien Binet <binet@cern.ch>'
__version__ = '$Revision: 1.2 $'


### stdlib imports ------------------------------------------------------------
import sys
import os
import os.path as osp
import subprocess
import logging
###

### globals -------------------------------------------------------------------
msg = None

### utils ---------------------------------------------------------------------
def _rmtree(path, *args):
    """disgusting rmtree-like function to workaround NFS and AFS metadata files
    """
    import commands as com
    sc,out = com.getstatusoutput('/bin/rm -rf %s' % path)
    if sc:
        print >> sys.stderr, out
        
def prepare_build(options):

    def monkey_patch(path):
        if not osp.exists(path): os.makedirs(path)
        # fix freaking CMT 'feature': make sure the path is in PYTHONPATH
        # *especially* if it didn't exist when cmt/setup.sh was run
        pypath = os.environ['PYTHONPATH'].split(os.pathsep)
        pypath.append(path) # put it at the end, to not confuse anybody
        os.environ['PYTHONPATH']=os.pathsep.join(pypath)
        
    for p in (options.platlib_dir, #options.purelib_dir,
              ):
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
##     try:
##         if osp.exists(options.build_dir):
##             shutil.rmtree(options.build_dir)
##     except Exception,err:
##         msg.warning(err)
    
##     if not osp.exists(options.build_dir):
##         os.makedirs(options.build_dir)

    dst = osp.join(options.build_dir,options.name)
    if osp.exists(dst):
        try:
            _rmtree(dst)
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
    atexit.register(_rmtree, dir_name, ignore_errors)
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
    stdout = _mkstemp(prefix='distutils-logfile-%s-'%options.name,
                      suffix='.txt')
    msg.info('building [%s] (logfile %s)', options.name, stdout.name)
    import functools, os
    run = functools.partial(run,
                            stdout=stdout,
                            stderr=stdout,
                            )
    prepare_build(options)

    setup = [
        'python',
        os.path.join(os.getcwd(), 'setup.py'),
        #'--quiet',
        '--verbose',
        ]


    msg.debug('cleaning up build-dir...')
    try:
        run(setup + ['clean'],   env=os.environ)
    except subprocess.CalledProcessError, err:
        msg.debug(err)
        pass

    run_cmd = run
    def _run(*args, **kwargs):
        try:
            run_cmd(*args, **kwargs)
        except subprocess.CalledProcessError, err:
            msg.error('problem executing %s', err.cmd)
            msg.error('dumping build log:')
            import sys
            print >> sys.stdout , "="*80
            stdout.flush()
            for l in open(stdout.name):
                print >> sys.stdout, l,
            print >> sys.stdout, "="*80
            raise
    run = _run
    
    if options.cfg_file and os.path.exists(options.cfg_file):
        msg.debug('copying .cfg file...')
        import shutil
        if os.path.exists('site.cfg'):
            os.remove('site.cfg')
        shutil.copyfile(options.cfg_file, 'site.cfg')
        
    
    msg.debug('running \'config\'...')
    run(setup + ['config'],  env=os.environ)

    msg.debug('building...')
    run(setup + [
        'build',
        '--build-base=%s' % osp.join(options.build_dir, 'base'),
        '--build-temp=%s' % osp.join(options.build_dir, 'tmp'),
        '--build-lib=%s'  % osp.join(options.build_dir, 'base'),
        ],
        env=os.environ)

    import sys
    python_config_version = "%i.%i" % sys.version_info[:2]

    msg.debug('installing...')
    run(setup + [
        'install',
        '--force',
        '--root=%s' % os.path.sep.join([options.installarea, options.cmtconfig]),
        #'--prefix=%s' % options.install_dir,
        '--install-platlib=lib/python%s' % python_config_version,
        '--install-purelib=lib/python%s' % python_config_version,
        '--install-scripts=bin',
        '--install-data=lib/python%s' % python_config_version,
        ],
        env=os.environ)
    stdout.close()
    return 0

def main():
    from optparse import OptionParser
    parser = OptionParser(usage='usage: %prog --name=<name> --src-dir=<dir-name> --install-dir=<install-dir>')
    p = parser.add_option
    p('--name',
      dest='name')
    p('--src-dir',
      dest='top_dir',
      help='the top level directory containing the \'setup.py\' file')
    p('--install-dir',
      dest='install_dir',
      help='the install directory name')
    p('-l','--lvl',
      dest='lvl',
      default='INFO',
      help='logging level (%default)')
    p('--cfg-file',
      dest='cfg_file',
      default=None,
      help = "path to an INI-like file to configure distutils")
    
    (options, args) = parser.parse_args()
    if (options.top_dir is None or
        options.install_dir is None):
        parser.print_help()
        return 1

    os.environ['PYTHON_EGG_CACHE'] = '/dev/null/.py-eggs'
    
    # retrieve build-environment with CMT configuration
    from PyCmt.Cmt import CmtWrapper
    cmt = CmtWrapper()
    fetch = cmt.show
    cmt_env = {
        'CMTINSTALLAREA' : fetch(macro_value='CMTINSTALLAREA'),
        'LDFLAGS' : fetch(macro_value='py_distutils_ldflags'),
        'CC'      : fetch(macro_value='py_distutils_cc'),
        'CFLAGS'  : fetch(macro_value='py_distutils_cflags'),
        'CPPFLAGS': fetch(macro_value='py_distutils_cppflags'),
        'CXX'     : fetch(macro_value='py_distutils_cxx'),
        'CXXFLAGS': fetch(macro_value='py_distutils_cxxflags'),
        'F77'     : fetch(macro_value='py_distutils_fcompiler'),
        'FFLAGS'  : fetch(macro_value='py_distutils_fflags'),
        }
    # FIXME
    # another hack: distcc may disturb some autoconf pkgs
    # => disable it by default
    cmt_env['CC'] = cmt_env['CC'].replace('distcc', '').strip()
    cmt_env['CXX']= cmt_env['CXX'].replace('distcc','').strip()
    cmt_env['F77']= cmt_env['F77'].replace('distcc', '').strip()

    cmt_env['FC'] = cmt_env['F77']
    cmt_env['FCFLAGS'] = cmt_env['FFLAGS']
    
    os.environ.update(cmt_env)

    osp = os.path

    # for proper clean-up
    pkg_name = fetch(macro_value='package')
    TMPDIR = osp.join(fetch(macro_value='%s_root'%pkg_name),
                      fetch(macro_value='tag'),
                      options.name+'-build')
    if osp.exists(TMPDIR):
        #import shutil
        _rmtree(TMPDIR)
    os.makedirs(TMPDIR)
    os.environ['TMPDIR'] = TMPDIR
    
    global msg
    msg = logging.getLogger('DistUtils')
    msg.setLevel(getattr(logging, options.lvl))
    
    top_dir = osp.expandvars(osp.expanduser(options.top_dir))
    top_dir = osp.abspath(top_dir)

    if not osp.isdir(top_dir):
        # is it a tar.gz-like file ?
        if osp.isfile(top_dir):
            # then uncompress somewhere...
            src_file = top_dir
            top_dir = _mkdtemp(prefix='distutils-top_dir-%s-'%options.name)
            _uncompress(src_file, top_dir, msg)
            if not ('setup.py' in os.listdir(top_dir)):
                top_dir = osp.join(top_dir, os.listdir(top_dir)[0])
                assert 'setup.py' in os.listdir(top_dir)
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
    
    options.bin_dir = osp.join(
        options.installarea,
        'share',
        'bin'
        )
    options.platlib_dir = osp.join(
        options.installarea,
        options.cmtconfig,
        'lib',
        'python%s.%s'%sys.version_info[:2]
        )
    options.purelib_dir = osp.join(
        options.installarea,
        'python')

    if options.name is None:
        options.name = osp.basename(options.top_dir)
    options.build_dir = osp.join(fetch(macro_value='%s_root'%pkg_name),
                                 fetch(macro_value='tag'),
                                 options.name+'-build')

    # for python-eggs
    os.environ['PYTHON_EGG_CACHE'] = osp.join(options.build_dir,
                                              '.python-eggs')

    try:
        sc = 3
        sc = build(options)
    except subprocess.CalledProcessError, err:
        msg.error(err)
        sc = 4
    except Exception, err:
        raise

    if sc == 0:
        # if we are here, then everything went fine: clean-up
        #import shutil
        _rmtree(options.build_dir)
        pass
    
    return sc

if __name__ == '__main__':
    sc = main()
    import sys
    sys.exit(sc)
    
    
