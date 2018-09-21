# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys
import re
import math
import os
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
    if not os.path.isfile(aFileName):
        print "Input file doesn't exist:",aFileName
        print "Blocking for safety"
        exit(3)

outFile = TFile(outFileName,"RECREATE")

# Store the names of the histograms we are adding to enforce uniqueness
uniqueHistos = {}

for aFileName in inFileNames:
    print "Reading file:",aFileName
    aFile = TFile(aFileName,"READ")
    
    for histName in aFile.GetKeyNames():
        if histName not in uniqueHistos.keys():
            hist = aFile.Get(histName)
            outFile.cd()
            hist.Write(histName)
            
            # Store the histogram and name
            hist.SetDirectory(0)
            uniqueHistos[histName] = hist
        else:
            # Check if they are identical histograms
            hist = aFile.Get(histName)
            hist.Add(uniqueHistos[histName],-1)
            areDifferent = ( math.fabs(hist.GetMinimum()) > 1.e-8 or math.fabs(hist.GetMaximum()) > 1.e-8 )

            if areDifferent:
                print "[ERROR] Two non-identical histograms with the same name were provided: %s"%(histName)
                print "Aborting..."
                exit(1)
            else:
                print "[SAFE] Skipping non-unique histogram name which is identical to the old one: %s"%(histName)


    aFile.Close()

outFile.Close()

