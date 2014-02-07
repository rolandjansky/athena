# Munge sys.path to something usable.
# When we're started from the Atlas environment, we'll have everything
# that Atlas sets up in our path.  This stuff will in general not work
# with the python version built into gdb.  So clean everything off
# of the path except for system directories.  Then add to the path
# our current directory.
# Preserve the first path in the last --- that's the gdb data directory.

import sys
import os
import gdb
def _fdummy(): return
thisdir = os.path.dirname (_fdummy.func_code.co_filename)
sys.path = ([sys.path[0], thisdir] +
            [p for p in sys.path[1:] if 1 or p.startswith('/usr/lib')])

# Put this directory the source path, for `source'.
gdb.execute ("dir %s" % thisdir)

import pyinteract
import findlib
import sources
gdb.execute ("source importcmd")

