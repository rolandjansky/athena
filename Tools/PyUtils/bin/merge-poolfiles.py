#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
## @file PyUtils/bin/merge-poolfiles.py
## @brief take a bunch of input pool files and produce a single one
##          autoconfiguration is (attempted to be) performed
## @author Sebastien Binet <binet@cern.ch>

__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """take a bunch of input pool files and produce a single one.
autoconfiguration is (attempted to be) performed
"""

import sys

if __name__ == "__main__":
   
    from PyUtils.Logging import logging
    msg = logging.getLogger('pool-merge')
    msg.setLevel(logging.INFO)
    
    from optparse import OptionParser
    parser = OptionParser(
        usage="usage: %prog [-o] out.merged.pool f1.pool f2.pool [...]"
        )
    p = parser.add_option
    p( "-o",
       "--output",
       dest = "outfname",
       default = None,
       help = "Name of the merged output POOL file" )
    p( "--evts",
       dest = "evts",
       default = -1,
       help = "Number of events to process (default: %default)" )
    p( "--logfile",
       dest = "logfile",
       default = None,
       help = "Path to a file where to put athena job's logfile (default: stdout)" )
    
    
    msg.info(':'*40)
    msg.info('welcome to poolfiles merger')
    
    (options, args) = parser.parse_args()

    fnames = []
    
    if len(args) > 0:
        from os.path import expanduser, expandvars
        fnames = [ expandvars(expanduser(arg))
                   for arg in args if arg[0] != "-" ]
        pass

    if options.outfname is None:
        # take the first out of fnames
        options.outfname = fnames[0]
        fnames = fnames[1:]
        
    if len(fnames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)


    # running merger...
    from PyUtils.PoolFile import merge_pool_files
    sc = merge_pool_files(input_files=fnames, output_file=options.outfname,
                          nevts=options.evts,
                          msg=msg,
                          logfile=options.logfile)
    msg.info('bye')
    msg.info(':'*40)
    sys.exit(sc)
    
