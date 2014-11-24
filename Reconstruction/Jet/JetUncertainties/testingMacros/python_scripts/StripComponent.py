# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
    print "  3. Name of component to strip"
    exit(1)

inputFile  = TFile.Open(sys.argv[1],"READ")
outputFile = TFile.Open(sys.argv[2],"RECREATE")
compName   = sys.argv[3]

foundComponent = False
for histName in inputFile.GetKeyNames():
    hist = inputFile.Get(histName)
    if TString(histName).Contains(compName):
        print "Removing component:",histName
        continue
    else:
        outputFile.cd()
        hist.Write(histName)

outputFile.Close()
inputFile.Close()

