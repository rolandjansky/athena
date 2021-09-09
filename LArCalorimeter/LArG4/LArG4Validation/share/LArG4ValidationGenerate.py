#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# print command line for logging
import os
from optparse import OptionParser
parser = OptionParser(usage = "usage: %prog", version="%prog $Id: LArG4ValidationGenerate.py 583537 2014-02-15 21:17:29Z zmarshal $")

#parser.add_option("--cpuNumber", dest="cpu",         default='00',                       help="select the desired CPU")
parser.add_option("--inputFile",  dest="input",       default=None,                       help="select the input benevent file name (preferred over the particle gun)")
parser.add_option("--outputFile", dest="output",      default='sp',                       help="select the output file name prefix (default: %default)")
parser.add_option("--geometry",   dest="geometry",    default='',                         help="select the geometry tag (leave empty for auto)")
parser.add_option("--condition",  dest="condition",   default='',                         help="select the condition tag (REQUIRED)")
parser.add_option("--physlist",   dest="physlist",    default='',                         help="select the physics list (leave empty for auto)")
parser.add_option("--etaMin",     dest="etaMin",      default='-5.5',                     help="particle gun: Minimum Eta (default: %default)")
parser.add_option("--etaMax",     dest="etaMax",      default='5.5',                      help="particle gun: Maximum Eta (default: %default)")
parser.add_option("--energyMin",  dest="energyMin",   default='20000',                    help="particle gun: particle Minimum energy (default: %default MeV)")
parser.add_option("--energyMax",  dest="energyMax",   default='500000',                   help="particle gun: particle Maximum energy (default: %default MeV)")
parser.add_option("--pid",        dest="pid",         default='11',                       help="particle gun: particle type (11=electron, 13=muon, 111=pi0 211=pi+, .., default: %default)")
parser.add_option("--parameterize",dest="parameterize", default=0,            type=int,   help="parameteriziation level (0=off, default: %default)")
parser.add_option("--nevents",    dest="nevents",     default=-1,             type=int,   help="number of events to generate (default: all)")
parser.add_option("--runNumber",  dest="runNumber",   default=0,              type=int,   help="run number (default: %default)")
parser.add_option("--FSLib",      dest="fsLibs",      default=[],      action="append",   help="path to the frozen shower libraries (separate flag for each library must be used)")
(options, args) = parser.parse_args()

exec = __file__.replace("LArG4ValidationGenerate.py","LArG4ValidationGenerateBody.py")
os.system('athena -c "options={:s}" {:s}'.format(str(options),exec))
