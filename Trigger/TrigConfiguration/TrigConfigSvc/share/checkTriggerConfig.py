#!/usr/bin/env python

# @file:    checkTriggerConfig.py
# @purpose: print all trigger configuration summary info of a pool file
# @author:  Joerg Stelzer 
# @date:    January 2009
#
# @example:
# @code
# checkTriggerConfig.py aod.pool.root
# checkTrigger /castor/cern.ch/user/j/johndoe/aod.pool.root
# checkTrigger somedir/*/*.pool
# @endcode
#

from optparse import OptionParser

if __name__ == '__main__':

    parser = OptionParser(usage="usage: %prog options file [file2 [...]]")

    parser.add_option( "-d",
                       "--chainsdetail",
                       action = "store_true",
                       default = False,
                       dest = "chainsdetail",
                       help = "prints detailed chain definition" )
    
    (options, args) = parser.parse_args()

    fileNames = [arg for arg in args if arg[0] != "-" ]

    if len(fileNames)==0:
        print "No file specified"
        parser.print_help()
        from sys import exit
        exit(1)

    from TrigConfigSvc.TriggerConfigARA import TriggerConfigARA
    trigconf = TriggerConfigARA(fileNames)
    trigconf.printFileSummary(options.chainsdetail)
