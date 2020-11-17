# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import sys
import re
from math import fabs
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


if len(sys.argv) < 4:
    print "Too few arguments.  Expected the following:"
    print "  1. Input root file 1"
    print "  2. Input root file 2"
    print "  3. Output plot file (.root)"
    exit(1)

inFile1 = TFile.Open(sys.argv[1],"READ")
inFile2 = TFile.Open(sys.argv[2],"READ")
outFile = TFile.Open(sys.argv[3],"RECREATE")

# First check for different histograms
for histName in inFile1.GetKeyNames():
    if histName not in inFile2.GetKeyNames():
        print "Histogram in file1 but not file2:",histName
for histName in inFile2.GetKeyNames():
    if histName not in inFile1.GetKeyNames():
        print "Histogram in file2 but not file1:",histName

# Now compare the same histograms
for histName in inFile1.GetKeyNames():
    if histName in inFile2.GetKeyNames():
        hist1 = inFile1.Get(histName)
        hist2 = inFile2.Get(histName)
        hist1.Add(hist2,-1)
        if fabs(hist1.GetMaximum()) > 1.e-4 or fabs(hist1.GetMinimum()) > 1.e-4:
            print "Histogram values differ:",histName
            outFile.cd()
            hist1.Write(histName)

outFile.Close()
inFile1.Close()
inFile2.Close()

