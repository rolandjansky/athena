#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/bin/gen-typereg-dso.py
# @purpose a python script to workaround various limitations of rootmap files
#          and reflex/cint typename impedance mismatches
# @author Sebastien Binet <binet@cern.ch>
# @date   February 2009

__doc__ = '''a python script to workaround various limitations of rootmap
files and reflex/cint typename impedance mismatches.
'''
__version__ = '$Revision: 1.1 $'
__author__ = 'Sebastien Binet <binet@cern.ch>'


if __name__ == "__main__":
    import sys
    import os
    import PyUtils.Dso as Dso
    oname = 'typereg_dso_db.csv'
    if len(sys.argv) > 1:
        oname = sys.argv[1]
    else:
        from PyCmt.Cmt import CmtWrapper
        project_root = CmtWrapper().projects()[0]
        from PyUtils.path import path
        oname = path(project_root) / "InstallArea" / "share" / oname
        if not os.path.exists(oname.dirname()):
            os.makedirs(oname.dirname())
            pass
    rflx_names = Dso.gen_typeregistry_dso(oname)
    sys.exit(0)
