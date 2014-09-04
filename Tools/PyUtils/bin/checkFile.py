#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    checkFile.py
# @purpose: read a POOL file and dump its content.
#           Inspired from CheckESD.C ROOT macro from Fredrik Akesson.
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    August 2005
#
# @example:
# @code
# checkFile aod.pool.root
# checkFile /castor/cern.ch/user/j/johndoe/aod.pool.root
# checkFile somedir/*/*.pool
# @endcode
#

__version__ = "$Revision: 1.5 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")
    p = parser.add_option
    p( "-f",
       "--file",
       dest = "fileName",
       help = "The path to the POOL file to analyze" )
    p( "-d",
       "--detailed-dump",
       action  = "store_true",
       dest    = "doDetailedDump",
       default = False,
       help = "Switch to activate or not a detailed dump of each TTree in the POOL file" )
    p( "--sort-fct",
       dest    = "sortFctName",
       default = "diskSize",
       help = "Sorting function used to list containers (allowed are: diskSize, memSize, name)" )
    p( "-o",
       "--output",
       dest = "outFileName",
       default = None,
       help = "Name of the output file which will contain the informations gathered during checkFile processing. These informations will be stored into a python-shelve file." )
    p( "--fast",
       dest = "fastMode",
       default = False,
       action  = "store_true",
       help = "Switch to enable the fast mode of checkFile.py (memory size will not be accurate -AT ALL-)"
       )
    p( "--detailed-branch-size",
       dest = "super_detailed_branch_sz",
       default = False,
       action  = "store_true",
       help = "Switch to enable a very detailed computation of the branch sizes (computed from the basket length) [SLOW]"
       )
    (options, args) = parser.parse_args()

    fileNames = []
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    fileNames = set( fileNames )
    sc = 0
    for fileName in fileNames:
        try:
            import PyUtils.PoolFile as PF
            PF.PoolOpts.FAST_MODE = options.fastMode
            PF.PoolOpts.SUPER_DETAILED_BRANCH_SZ = options.super_detailed_branch_sz
            poolFile = PF.PoolFile( fileName )
            poolFile.checkFile( sorting = options.sortFctName )
            if options.doDetailedDump:
                dumpFile = os.path.basename(fileName)+ ".txt"
                print "## dumping details into [%s]" % dumpFile
                poolFile.detailedDump( dumpFile )
            if options.outFileName:
                outFileName = options.outFileName
                print "## saving checkFile report into [%s]..." % outFileName
                poolFile.saveReport( outFileName )
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass

        except :
            print "## Caught something !! (don't know what)"
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 10
            pass
        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames
    
    print "## Bye."
    sys.exit(sc)
