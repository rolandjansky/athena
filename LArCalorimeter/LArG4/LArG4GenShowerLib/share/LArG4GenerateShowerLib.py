#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
# print command line for logging
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4GenerateShowerLib.py 767177 2016-08-10 08:49:45Z disimone $")

parser.add_option("--inputEvtFileList",    dest="inputevt",    action="append",                       help="select the input file name")
parser.add_option("--inputStructFileList", dest="inputstruct", action="append",                       help="select the output file name")
parser.add_option("--nevents",    dest="nevents",                  type=int,   help="select the number of events to generate (default: all)")
parser.add_option("--skipevents", dest="skipevents",               type=int,   help="select the number of events to skip (default: 0)")
parser.add_option("--geometry",   dest="geometry",                             help="select the geometry tag (default can be used)")
parser.add_option("--condition",  dest="condition",                            help="select the condition tag (REQUIRED)")
parser.add_option("--physlist",   dest="physlist",                             help="select the physics list (default can be used)")
parser.set_defaults(inputevt=[],inputstruct=[],nevents=-1,skipevents=0,geometry="",condition="")
(options, args) = parser.parse_args()

if len(options.inputevt) == 0 :
    print ("No input, aborting")
    import sys
    sys.exit(1)

if len(options.condition) == 0 :
    print ("No condition tag set, aborting")

exec = __file__.replace("LArG4GenerateShowerLib.py","LArG4GenerateShowerLibBody.py")
os.system('athena -c "options={:s}" {:s}'.format(str(options),exec))
