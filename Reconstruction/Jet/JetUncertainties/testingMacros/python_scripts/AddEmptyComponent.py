# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import re
import array
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


def getJetDef(histName):
    tokens = histName.split("_")
    return tokens[-1]


if len(sys.argv) < 6:
    print "Too few arguments.  Expected the following:"
    print "  1. Input file"
    print "  2. Output file"
    print "  3. Name of component to add"
    print "  4. Number of dimensions"
    print "  5. Ranges in a special format"
    print "     Format: xmin,xmax;ymin,max;zmin,zmax"
    print "     Example: 15,2500;-4.5,4.5"
    exit(1)

inputFile  = TFile.Open(sys.argv[1],"READ")
outputFile = TFile.Open(sys.argv[2],"RECREATE")
compName   = sys.argv[3]
numDim     = int(sys.argv[4])
binning    = sys.argv[5]

# Parse binning
if numDim != 1 and numDim != 2 and numDim != 3:
    print "Unexpected number of dimensions:",numDim
    exit(2)
binTokens = binning.split(";")
if len(binTokens) != numDim:
    print "Binning specifications and nDim don't match, %d vs %d"%(len(binTokens),numDim)
    exit(3)
bins = []
for aToken in binTokens:
    rangeTokens = aToken.split(",")
    if len(rangeTokens) != 2:
        print "Unexpected number of range tokens:",len(rangeTokens)
    bins.append([float(rangeTokens[0]),float(rangeTokens[1])])


# Get the list of jet definitions and write original histos
jetDefs = []
for histName in inputFile.GetKeyNames():
    jetDef = getJetDef(histName)
    if not (jetDef in jetDefs):
        jetDefs.append(jetDef)
    hist = inputFile.Get(histName)
    outputFile.cd()
    hist.Write(histName)

# Now write the new empty histos
for aDef in jetDefs:
    histName = compName+"_"+aDef
    
    hist = None
    if numDim == 1:
        hist = TH1D(histName,histName,1,bins[0][0],bins[0][1])
        hist.SetBinContent(1,0)
    elif numDim == 2:
        hist = TH2D(histName,histName,1,bins[0][0],bins[0][1],1,bins[1][0],bins[1][1])
        hist.SetBinContent(1,1,0)
    elif numDim == 3:
        hist = TH3D(histName,histName,1,bins[0][0],bins[0][1],1,bins[1][0],bins[1][1],1,bins[2][0],bins[2][1])
        hist.SetBinContent(1,1,1,0)

    print "Adding empty %dD histogram of name %s"%(numDim,histName)
    outputFile.cd()
    hist.Write(histName)



outputFile.Close()
inputFile.Close()


