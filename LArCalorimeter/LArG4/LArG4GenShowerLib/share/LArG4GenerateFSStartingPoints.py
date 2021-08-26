#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
# print command line for logging
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4GenerateFSStartingPoints.py 574715 2013-12-09 23:43:19Z gsedov $")

parser.add_option("--inputFile",  dest="input",       default=None, action="append",      help="select the input genevent file name (preferred over the particle gun)")
parser.add_option("--outputFile", dest="output",      default='StartGenEvents',           help="select the output file name (default: %default)")
parser.add_option("--geometry",   dest="geometry",    default='',                         help="select the geometry tag (default can be used)")
parser.add_option("--condition",  dest="condition",   default='',                         help="select the geometry tag (REQUIRED)")
parser.add_option("--physlist",   dest="physlist",                                        help="select the physics list (default can be used)")
parser.add_option("--etaMin",     dest="etaMin",      default='-3.3',                     help="particle gun: Minimum Eta (default: %default)")
parser.add_option("--etaMax",     dest="etaMax",      default='3.3',                      help="particle gun: Maximum Eta (default: %default)")
parser.add_option("--energyMin",  dest="energyMin",   default='20000',                    help="particle gun: particle Minimum energy (default: %default MeV)")
parser.add_option("--energyMax",  dest="energyMax",   default='50000',                    help="particle gun: Maximum energy (default: %default MeV)")
parser.add_option("--pid",        dest="pid",         default='11',                       help="particle gun: particle type (11=electron, 13=muon, 111=pi0 211=pi+, .., default: %default)")
parser.add_option("--nevents",    dest="nevents",     default=5000,           type=int,   help="number of events to process (default: %default)")
parser.add_option("--spratio",    dest="spratio",     default=0.02,           type=float, help="the ratio of starting points to be saved (default: %default)")
(options, args) = parser.parse_args()

if len(options.condition) == 0 :
    print ("No condition tag set, aborting")

exec = __file__.replace("LArG4GenerateFSStartingPoints.py","LArG4GenerateFSStartingPointsBody.py")
os.system('athena -c "options={:s}" {:s}'.format(str(options),exec))
