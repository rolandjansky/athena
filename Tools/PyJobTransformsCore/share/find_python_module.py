#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import sys,os

def usage():
    use = "usage: %s <module> [module]" % os.path.basename(sys.argv[0])
    print(use + """
    
Print the full path of the requested python module(s), as found in sys.path (PYTHONPATH + system paths)
Wildcards are accepted, in which case all (first) matches are printed, one per line.
The '.py' suffix can be omitted, in which case both .py and .pyc extensions are tried.
If no match if found, nothing is printed.""")


if len(sys.argv) <= 1:
    usage()
    sys.exit(1)

try:
    from PyJobTransformsCore.envutil import find_python_modules
except ImportError:
    raise EnvironmentError("ATLAS Runtime environment not setup.")
    
for mod in sys.argv[1:]:
    full = find_python_modules(mod)
    for f in full: print(f)
