# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import re
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


if len(sys.argv) < 5:
    print "Too few arguments.  Expected the following:"
    print "  1. Input file"
    print "  2. Output file"
    print "  3. Original name of component"
    print "  4. New name of component"
    exit(1)

inputFile  = TFile.Open(sys.argv[1],"READ")
outputFile = TFile.Open(sys.argv[2],"RECREATE")
compName   = sys.argv[3]
newName    = sys.argv[4]

for histName in inputFile.GetKeyNames():
    hist = inputFile.Get(histName)
    if TString(histName).Contains(compName):
        newHistName = re.sub(compName,newName,histName)
        print "Renaming component: %s --> %s"%(histName,newHistName)
        hist.Write(newHistName)
        continue
    else:
        outputFile.cd()
        hist.Write(histName)

outputFile.Close()
inputFile.Close()

