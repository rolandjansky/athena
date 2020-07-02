#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import sys,os

__doc__ = """Print the full path of files as found in DATAPATH.
Wildcards are accepted, in which case all (first) matches are printed.
If no match if found, nothing is printed."""

def usage():
    use = "usage: %s <file> [file]" % os.path.basename(sys.argv[0])
    print (use + os.linesep*2 + __doc__)

if len(sys.argv) <= 1:
    usage()
    sys.exit(1)

try:
    from PyJobTransformsCore.envutil import find_files_env
except ImportError:
    import traceback
    traceback.print_exc()
    raise EnvironmentError("ATLAS Runtime environment not setup.")

for data in sys.argv[1:]:
    full = find_files_env(data,'DATAPATH')
    for f in full: print (f)
