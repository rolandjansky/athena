#!/usr/bin/env python

# @file:    mpMon.py
# @purpose: This script extract the WORKER information from the parent process's log and then WORKER's logs in AthenaMP
# @author:  Mous Tatarkhanov <tmmous@cern.ch>
# @date:    April 15, 2009
# @example:
# @code
# extract_mp_stat.py    log.parent_process
# @endcode
#


__version__ = "$Revision: 285809 $"
__author__  = "Mous Tatarkhanov <tmmous@cern.ch>"

from optparse import OptionParser
#import AthenaMP.mpMonTools as mpt

import sys

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] -f jobo")
    p = parser.add_option
    p( "-f",
        "--file",
        dest = "log_file",
        default = None,
        help ="athenaMP parent process stdout log" 
    )
    
    (options, args) = parser.parse_args()
    
    import sys, os
    if  options.log_file == None:
        str(parser.print_help() or "")
        sys.exit(1)
        
    log_file = options.log_file

    print "log_file = %s" % log_file
    if not os.path.exists(log_file):
        print "log_file doesn't exist. Please give valid parent process log file"
        str(parser.print_help() or "")
        sys.exit(1)
    
    
    from AthenaMP import mpMonTools as mpt
    mpt.writeOutWorkersStat(log_file)
    print ("DONE...")
