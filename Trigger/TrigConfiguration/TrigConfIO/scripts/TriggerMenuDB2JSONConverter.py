#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys, os
from optparse import OptionParser, OptionGroup

class Silence():
    def __init__(self, verbose):
        self.verbose = verbose
    def __enter__(self):
        if not self.verbose:
            self.so = sys.stdout
            sys.stdout = open(os.devnull, 'w')
    def __exit__(self, type, value, traceback):
        if not self.verbose:
            sys.stdout = self.so
            
def main():


    parser = OptionParser(usage="usage: %prog [options] smk")

    parser.add_option("-v", "--verbose",
                      action = "store_true",
                      dest = "verbose",
                      default = False,
                      help="verbose output [default: %default]")
    
    actions = OptionGroup(parser, "actions")

    actions.add_option( "",
                        "--l1",
                        dest = "l1psk",
                        metavar = "PSK",
                        default = 0,
                        help = "Level 1 prescale key PSK for L1 prescale set to convert" )

    actions.add_option( "",
                        "--hlt",
                        type="int",
                        dest = "hltpsk",
                        metavar="PSK",
                        default = 0,
                        help = "HLT prescale key PSK for HLT prescale set to convert" )

    actions.add_option( "",
                        "--showpsk",
                        dest = "showpsk",
                        action = "store_true",
                        default = False,
                        help = "only prints prescale keys attached to SMK [default: %default]" )

    parser.add_option_group(actions)

    parser.add_option( "",
                       "--db",
                       dest = "db",
                       default = "TRIGGERDB",
                       help = "database alias TRIGGERDB|TRIGGERDBREPR|TRIGGERDBMC  [default: %default]" )


    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("Please specify the SMK")

    try:
        smk = int(args[0])
    except ValueError:
        parser.error("SMK must be an integer")


    if not (options.l1psk or options.hltpsk or options.showpsk):
        parser.print_help()
        parser.error("Please specify one of the actions")

    if options.showpsk:
        with Silence(options.verbose):
            from TrigConfigSvc.TrigConfigSvcUtils import getHLTPrescalesFromSMK
            hltpsk = getHLTPrescalesFromSMK(options.db,smk)
        print "Trigger keys connected to SM %i are %r" % (smk, [int(k[0]) for k in hltpsk])


    if options.hltpsk:
        with Silence(options.verbose): # suppress all the output from loading the athena modules
            from TrigConfigSvc.TrigConfigSvcUtils import queryHLTPrescaleTableRun2
            res = queryHLTPrescaleTableRun2(options.db,options.hltpsk,smk)        

        from collections import defaultdict, OrderedDict
        d = defaultdict(lambda : list())

        for e in res:
            d[(int(e[1]),e[0])] += [ (e[2],e[4]) ]

        # building json structure
        prescaleSet = []
        for (chainID, chainName),prescales in sorted(d.items()):
            entry = OrderedDict()
            entry['chainId'] = chainID
            entry['chainName'] =chainName
            entry.update(prescales)
            prescaleSet.append( entry )

        # saving to file
        import json
        filename = "HLTPrescales_smk%i_psk%i.json" % (smk, options.hltpsk)
        FH = open(filename, "w")
        json.dump(prescaleSet, FH, indent=4, separators=(',', ': '))
        FH.close()
        print "Wrote file %s" % filename

    if options.l1psk:
        with Silence(options.verbose): # suppress all the output from loading the athena modules
            from TrigConfigSvc.TrigConfigSvcUtils import queryHLTPrescaleTableRun2
            res = queryHLTPrescaleTableRun2(options.db,options.hltpsk,smk)        

        from collections import defaultdict, OrderedDict
        d = defaultdict(lambda : list())

        for e in res:
            d[(int(e[1]),e[0])] += [ (e[2],e[4]) ]

        # building json structure
        prescaleSet = []
        for (chainID, chainName),prescales in sorted(d.items()):
            entry = OrderedDict()
            entry['chainId'] = chainID
            entry['chainName'] =chainName
            entry.update(prescales)
            prescaleSet.append( entry )

        # saving to file
        import json
        filename = "HLTPrescales_smk%i_psk%i.json" % (smk, options.hltpsk)
        FH = open(filename, "w")
        json.dump(prescaleSet, FH, indent=4, separators=(',', ': '))
        FH.close()
        print "Wrote file %s" % filename




if __name__=="__main__":
    sys.exit(main())
        
