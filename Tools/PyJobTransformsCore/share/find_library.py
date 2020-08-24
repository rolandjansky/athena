#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import sys,os

__doc__ = """Print the full path of the requested shared library, as found in LD_LIBRARY_PATH.
Wildcards are accepted, in which case all (first) matches are printed.
The 'lib' prefix and 'so' suffix can be omitted. If no match if found, nothing is printed."""

def usage():
    use = "usage: %s <library> [library]" % os.path.basename(sys.argv[0])
    print(use + os.linesep*2 + __doc__)

if len(sys.argv) <= 1:
    usage()
    sys.exit(1)

try:
    from PyJobTransformsCore.envutil import find_libraries
except ImportError:
    raise EnvironmentError("ATLAS Runtime environment not setup.")

for lib in sys.argv[1:]:
    full = find_libraries(lib)
    for f in full: print(f)
