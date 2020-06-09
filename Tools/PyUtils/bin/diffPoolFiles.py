#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    diffPoolFiles.py
# @purpose: check that 2 POOL files have same content (containers and sizes)
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    March 2007
#
# @example:
#
# diffPoolFiles aod.pool ref.aod.pool
#

__author__  = "Sebastien Binet"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-r] file1.pool [-f] file2.pool")
    parser.add_option( "-r",
                       "--ref",
                       dest = "refFileName",
                       help = "The path to the first POOL file to analyze" )
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the second POOL file to analyze" )
    parser.add_option( "-v",
                       "--verbose",
                       action  = "store_true",
                       dest = "verbose",
                       default = False,
                       help = "Switch to activate verbose printout" )
    parser.add_option( "-s",
                       "--strict",
                       action  = "store_true",
                       dest = "strict",
                       default = False,
                       help = "Compare both memSize and diskSize" )


    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.refFileName = args[0]
        pass
    if len(args) > 1 and args[1][0] != "-":
        options.fileName = args[1]
        pass

    if options.fileName    is None or options.refFileName is None :
        str(parser.print_help() or "")
        sys.exit(1)
        pass

    chkFileName = os.path.expandvars(os.path.expanduser(options.fileName))
    refFileName = os.path.expandvars(os.path.expanduser(options.refFileName))
    
    import PyUtils.PoolFile as PF
    diff = PF.DiffFiles( refFileName = refFileName,
                         chkFileName = chkFileName,
                         verbose = options.verbose,
                         strict = options.strict )
    
    diff.printSummary()
    sys.exit(diff.status())
