#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @purpose: driver for the pkgbuild-based builders

from __future__ import with_statement

### imports -------------------------------------------------------------------
import sys
import os
import optparse

import PyCmt.pkgbuild as pkgbuild

msg = pkgbuild.msg

### functions -----------------------------------------------------------------
def main():
    parser = optparse.OptionParser(usage="usage: %prog pkgbuild.py")
    p = parser.add_option
    p("-f", "--file",
      default = None,
      dest = "fname",
      help = "path to the pkgbuild.py file containing build instructions")
    p("-p", "--pkg-root",
      default = None,
      dest = "pkg_root",
      help = "directory containing the cmt/requirements file (so we can "+\
             "query CMT for build environment and options)")

    (opts, args) = parser.parse_args()

    if opts.fname is None or opts.pkg_root is None:
        parser.print_help()
        return 1

    import os.path as osp
    def _norm_path(p):
        p = osp.expanduser(osp.expandvars(p))
        p = osp.realpath(osp.normpath(p))
        return p
    opts.fname = _norm_path(opts.fname)
    opts.pkg_root = _norm_path(opts.pkg_root)

    # may raise
    os.access(opts.fname, os.R_OK)
    os.access(opts.pkg_root, os.R_OK)
    assert osp.isdir(opts.pkg_root)
    assert osp.isfile(opts.fname)

    env = os.environ.copy()
    env['pkg_root'] = opts.pkg_root
    return pkgbuild.build_package(opts.fname, env=env, msg=msg)

if __name__ == "__main__":
    import sys
    sys.exit(main())
    
        
    
    
