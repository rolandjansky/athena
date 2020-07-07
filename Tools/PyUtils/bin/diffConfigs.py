#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    diffConfigs.py
# @purpose: check that 2 ConfigurationShelves have same content (both in
#           configurables and properties)
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    September 2008
#
# @example:
#
# diffConfigs ref.pkl chk.pkl
#

from __future__ import print_function

__author__  = "Sebastien Binet"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-r] ref.pkl [-f] chk.pkl")
    parser.add_option( "-r",
                       "--ref",
                       dest = "refFileName",
                       help = "The path to the first ConfigurationShelve file to analyze" )
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the second ConfigurationShelve file to analyze" )
    parser.add_option( "-v",
                       "--verbose",
                       action  = "store_true",
                       dest = "verbose",
                       default = False,
                       help = "Switch to activate verbose printout" )


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

    print ("::: comparing configurations:")
    print (":::  ref: %s" % refFileName)
    print (":::  chk: %s" % chkFileName)
    from AthenaCommon.ConfigurationShelve import cmpConfigs
    ref, chk, report = cmpConfigs (ref=refFileName,
                                   chk=chkFileName)
    if len(report)==0:
        print ("::: all good")
        sys.exit(0)

    for l in report: print (l)
    print ("::: configurations differ !")
    sys.exit(1)
