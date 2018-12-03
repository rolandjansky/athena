# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import sys
import re
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


if len(sys.argv) < 4:
    print "Too few arguments.  Expected the following:"
    print "  1. Input file"
    print "  2. Output file"
    print "  3+ Name(s) of components to extract"
    exit(1)

inputFile  = TFile.Open(sys.argv[1],"READ")
outputFile = TFile.Open(sys.argv[2],"RECREATE")
compNames  = sys.argv[3:]

# Check that the request is possible
for compName in compNames:
    if compName not in inputFile.GetKeyNames():
        print "Unable to continue - input file does not contain the requested component:",compName
        exit(2)

# Now do what the user asked
for compName in compNames:
    hist = inputFile.Get(compName)
    outputFile.cd()
    hist.Write(compName)

outputFile.Close()
inputFile.Close()

