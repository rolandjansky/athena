#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file:    checkTrigger.py
# @purpose: read a POOL file and dump a trigger summary.
#           Copied (& modified) from CheckFile.py from Sebastien Binet
# @author:  Till Eifert <Till.Eifert@cern.ch>
# @date:    October 2008
#
# @example:
# @code
# checkTrigger aod.pool.root
# checkTrigger /castor/cern.ch/user/j/johndoe/aod.pool.root
# checkTrigger somedir/*/*.pool
# @endcode
#

__author__  = "Till Eifert <Till.Eifert@cern.ch>"


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
##    p( "-d",
##       "--detailed-dump",
##       action  = "store_true",
##       dest    = "doDetailedDump",
##       default = False,
##       help = "Switch to activate or not a detailed dump of each ... " )
    p( "--sort-fct",
       dest    = "sortFctName",
       default = "name",
       help = "Sorting function used to list triggers (allowed are: name, counter, evtsBeforePS, evtsAfterPS, evtsAfterPT)" )
    p( "-o",
       "--output",
       dest = "outFileName",
       default = None,
       help = "Name of the output file which will contain the informations gathered during checkTrigger processing. These informations will be stored into a python-shelve file. If the given file name extension is '.csv' a comma separated file is written instead. Note that the python-shelve files can be read-in again by this script." )
    
    (options, args) = parser.parse_args()

    fileNames = []
   
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName is None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fileName is not None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    fileNames = set( fileNames )
    sc = 0

    for fileName in fileNames:
        try:
            import TrigConfigSvc.TriggerSummary as TS
            #TS.PoolOpts.FAST_MODE = options.fastMode
            poolFile = TS.PoolFile( fileName )
            poolFile.checkFile( sorting = options.sortFctName )
##            if options.doDetailedDump:
##                dumpFile = os.path.basename(fileName)+ ".txt"
##                print "## dumping details into [%s]" % dumpFile
##                poolFile.detailedDump( dumpFile )
            if options.outFileName:
                outFileName = options.outFileName
                print("## saving checkFile report into [%s]..." % outFileName)
                poolFile.saveReport( outFileName )
        except Exception as e:
            print("## Caught exception [%s] !!" % str(e.__class__))
            print("## What: %s" % e)
            print(sys.exc_info()[0])
            print(sys.exc_info()[1])
            sc = 1
            pass

        except Exception:
            print("## Caught something !! (don't know what)")
            print(sys.exc_info()[0])
            print(sys.exc_info()[1])
            sc = 10
            pass
        if len(fileNames) > 1:
            print("")
        pass # loop over fileNames
    
    print("## Bye.")
    sys.exit(sc)
