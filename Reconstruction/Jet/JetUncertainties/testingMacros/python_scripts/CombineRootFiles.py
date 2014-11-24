# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys
import re
from ROOT import *
def GetKeyNames(self,dir=""):
    self.cd(dir)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


if len(sys.argv) < 3:
    print "Too few arguments.  Expected the following:"
    print "     1. Output root file"
    print "     2+ Input root files"
    exit(1)

outFileName = sys.argv[1]
inFileNames = sys.argv[2:]

if not outFileName.endswith(".root"):
    print "Output file doesn't appear to be a root file:",outFile
    print "Blocking for safety"
    exit(2)
for aFileName in inFileNames:
    if not aFileName.endswith(".root"):
        print "Input file doesn't appear to be a root file:",aFileName
        print "Blocking for safety"
        exit(3)

outFile = TFile(outFileName,"RECREATE")

for aFileName in inFileNames:
    print "Reading file:",aFileName
    aFile = TFile(aFileName,"READ")
    
    for histName in aFile.GetKeyNames():
        hist = aFile.Get(histName)
        outFile.cd()
        hist.Write(histName)

    aFile.Close()

outFile.Close()

