#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    pool_extractFileIdentifier.py
# @purpose: extract the GUID of a POOL file.
#           Also speeds up greatly processing time by shrinking LD_LIBRARY_PATH
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    March 2009
#
# @example:
#
# python pool_extractFileIdentifier.py aod.pool.root
#
# if pool_extractFileIdentifier.py has been made 'chmod +x' one can just do:
# ./pool_extractFileIdentifier.py aod.pool.root

from __future__ import with_statement, print_function

from future import standard_library
standard_library.install_aliases()

def pool_extract(files):
    print (":: extracting GUID for [%i] files... "% len(files))
    import os
    import subprocess
    sc,exe = subprocess.getstatusoutput('which pool_extractFileIdentifier')
    if sc != 0:
        print (":: could not find 'pool_extractFileIdentifier' !")
        print (exe)
        return 1

    cmd = "%s %s" % (exe, " ".join(files))
    sc, out = subprocess.getstatusoutput(cmd)

    out = os.linesep.join(
        [o for o in out.splitlines()
         if not (o.startswith("Warning in <TClass::TClass>: no dictionary for class ") or
                 o.startswith('Warning in <TEnvRec::ChangeValue>: duplicate entry'))]
        )

    if sc != 0:
        print (":: problem running pool_extractFileIdentifier:")
        print (out)
        return sc

    print (out)
    print (":: extracting GUID for [%i] files... [done]" % len(files))
    return sc
    
if __name__ == "__main__":
    import sys
    from optparse import OptionParser
    parser = OptionParser(usage="%prog file1.pool [file2.pool [...]]")
    parser.add_option("-f", "--files",
                      dest = "files",
                      help = "(list of) files to extract the GUID(s) from")
    options, args = parser.parse_args()

    files = list()
    if len(args) > 0:
        files = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.files is None and len(files) == 0:
        str(parser.print_help() or "")
        print (":: You have to provide at least one POOL file to extract a GUID from:")
        print (" shell> pool_extractFileIdentifier.py aod.pool")
        sys.exit(1)

    if not (options.files is None):
        import os
        for f in options.files.split():
            f = os.path.expandvars(os.path.expanduser(f))
            files.append(f)

    sys.exit(pool_extract(files=files))
