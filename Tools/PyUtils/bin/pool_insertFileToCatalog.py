#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    pool_insertFileToCatalog.py
# @purpose: insert a POOL file into a POOL file catalog without displaying the
#           annoying ROOT warnings for missing dictionaries
#           Also speeds up greatly processing time by shrinking LD_LIBRARY_PATH
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    May 2006
#
# @example:
#
# python pool_insertFileToCatalog.py aod.pool.root
#
# if pool_insertFileToCatalog.py has been made 'chmod +x' one can just do:
# ./pool_insertFileToCatalog.py aod.pool.root
from __future__ import with_statement, print_function

from future import standard_library
standard_library.install_aliases()

def pool_insert(files, catalog_name="xmlcatalog_file:PoolFileCatalog.xml"):
    print (":: inserting [%i] files into pool catalog... (%s)"%(
        len (files),
        catalog_name
        ))
    import os
    import subprocess
    sc,exe = subprocess.getstatusoutput ('which pool_insertFileToCatalog')
    if sc != 0:
        print (":: could not find 'pool_insertFileToCatalog' !")
        print (exe)
        return 1

    os.environ['POOL_CATALOG'] = catalog_name
    cmd = "%s %s" % (exe, " ".join(files))
    sc, out = subprocess.getstatusoutput (cmd)
        
    out = os.linesep.join(
        [o for o in out.splitlines()
         if not (o.startswith("Warning in <TClass::TClass>: no dictionary for class ") or
                 o.startswith('Warning in <TEnvRec::ChangeValue>: duplicate entry'))]
        )

    if sc != 0:
        print (":: problem running pool_insertFileToCatalog:")
        print (out)
        return 2

    print (out)
    print (":: inserting [%i] files into pool catalog... [done]"%len(files))
    return sc
    
if __name__ == "__main__":
    import sys
    from optparse import OptionParser
    parser = OptionParser(usage="%prog file1.pool [file2.pool [...]] [--catalog='xmlcatalog_file:PoolFileCatalog.xml']")
    parser.add_option("-f", "--files",
                      dest = "files",
                      help = "(list of) files to be inserted in the catalog")
    parser.add_option("--catalog",
                      dest = "catalog",
                      default = "xmlcatalog_file:PoolFileCatalog.xml",
                      help = "catalog connection string")
    options, args = parser.parse_args()

    files = list()
    if len(args) > 0:
        files = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.files is None and len(files) == 0:
        str(parser.print_help() or "")
        print (":: You have to provide at least one POOL file to insert:")
        print (" shell> pool_insertFileToCatalog.py aod.pool")
        sys.exit(1)

    if not (options.files is None):
        import os
        for f in options.files.split():
            f = os.path.expandvars(os.path.expanduser(f))
            files.append(f)

    sc = pool_insert(files=files, catalog_name=options.catalog)
    sys.exit(sc)
