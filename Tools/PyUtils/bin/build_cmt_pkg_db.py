#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    PyUtils/bin/build_cmt_pkg_db.py
# @purpose: build the static list of packages (and their dependencies) of a
#           given project
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    June 2008
#
# @example:
#  build_cmt_pkg_db AtlasCore
#  build_cmt_pkg_db -p AtlasCore -o atlas_core_cmt_db.pkl
#  build_cmt_pkg_db --project AtlasCore -o atlas_core_cmt.db

__version__ = "$Revision: 1.2 $"

from optparse import OptionParser

import os, sys, commands

import PyCmt.Logging as L
from PyCmt import Cmt

if __name__ == "__main__":

    parser = OptionParser(usage='usage: %prog [options] [-p] ProjectName [[-o] OutputDb]')
    p = parser.add_option
    p('-p', '--project',
      dest='project',
      help='name of the project to inspect and build the package-db')

    p('-o', '--output',
      dest='output',
      help='name of the output file where to store the package-db (shelve)')

    p('-l', '--level',
      dest='log_lvl',
      default='INFO',
      help='logging level (aka verbosity)')

    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.project = args[0]
        pass

    if len(args) > 1 and args[1][0] != "-":
        options.output = args[1]
        pass

    if options.project is None:
        str(parser.print_help() or '')
        raise SystemExit(1)

    if options.output is None:
        options.output = 'proj_%s_cmt_pkg_shelve.db' % options.project.lower()
    options.output = os.path.expanduser (options.output)
    options.output = os.path.expandvars (options.output)
    options.output = os.path.realpath   (options.output)
    

    options.log_lvl = options.log_lvl.upper()
    if hasattr(L.logging, options.log_lvl):
        lvl = getattr(L.logging, options.log_lvl)
    else:
        print "*"*80
        print "BootStrap: Unknown logging level [%s] !!" % options.log_lvl
        print "BootStrap: Defaulting to [INFO]..."
        print "*"*80
        lvl = L.logging.INFO

    cmt = Cmt.CmtWrapper(lvl)
    
    msg = cmt.msg
    msg.info ('building dependencies...')

##     _allowed_output_db_formats = ('.pkl',)
##     _ext = os.path.splitext (options.output)[1]
##     if not _ext in _allowed_output_db_formats:
##         msg.error ('unknown output-db format: [%s]', _ext)
##         msg.error ('allowed formats are %r', _allowed_output_db_formats)
##         raise SystemExit(1)

##     if _ext == '.pkl':
##         try:
##             import cPickle as pickle
##         except ImportError:
##             import pickle
##         ofile = open (options.output, 'w')
##     else:
##         pass
    if os.path.exists (options.output):
        os.unlink (options.output)
    import shelve
    out_db = shelve.open (options.output)
    msg.info ('project used: %sRelease', options.project)
    msg.info ('output-db:    %s',        options.output)

    import tempfile,atexit,shutil
    tmp_root = tempfile.mkdtemp()
    atexit.register (shutil.rmtree, tmp_root)
    cwd = os.getcwd()
    os.chdir (tmp_root)

    cmt_tmp_dir = os.path.join (os.path.realpath (tmp_root),
                                'Dep%s'%options.project,
                                'cmt')
    if not os.path.exists (cmt_tmp_dir):
        os.makedirs (cmt_tmp_dir)

    os.chdir (cmt_tmp_dir)

    proj_releases  = [cmt.project_release(options.project)]
    proj_releases += [cmt.project_release(d)
                      for d in cmt.project_deps(options.project)]
    msg.debug('==> proj_releases: %s', proj_releases)
    
    req = open ('requirements', 'w')
    req.writelines (os.linesep.join ([
        "package Dep%s" % options.project,
        "",
        "author AtlasCollaboration",
        ""
        "".join( [ "use %s *\n" % p for p in proj_releases ] ),
        ""
        ]))
    req.close()
    sc, out = commands.getstatusoutput ('%s config'%cmt.bin)
    if sc != 0:
        err = "*** problem running [cmt config] ! ***\n%s" % out
        raise RuntimeError(err)
    else:
        msg.debug ('[cmt config] cmd was OK')
        
    sc, out = commands.getstatusoutput ('%s show uses >| uses.%s.cmt'%\
                                        (cmt.bin, options.project))
    if sc != 0:
        err = "*** problem running [cmt show uses] ! ***\n%s"%out
        raise RuntimeError(err)
    else:
        msg.debug ('[cmt show uses >| uses.%s.cmt] cmd was OK',
                   options.project)

    msg.info ('building package db...')
    pkg_db = Cmt.buildPkgDb ('uses.%s.cmt'%options.project, msg)

    msg.info ('building dependency graph...')
    db = Cmt.buildDepGraph ('uses.%s.cmt'%options.project, pkg_db, msg)
    
    os.chdir(cwd)
    out_db[options.project] = db
    out_db.close()

    msg.info ('nbr of packages: %i', len(db.pkgs()))
##     pkgs = db.pkgs().keys()[:10]
##     for i in pkgs:
##         msg.info('\t%s',i)
    raise SystemExit(0)

