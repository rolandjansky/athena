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
    print "  3. Output name of histogram"
    print "  4+ Input histogram name(s) to be combined in a special format"
    print "     Format: histName,minVal,maxVal"
    print "     Example: PunchThroughCentral,0,1.3 where values are for |eta|"
    exit(1)

inputFile  = TFile.Open(sys.argv[1],"READ")
outputFile = TFile.Open(sys.argv[2],"RECREATE")
outName    = sys.argv[3]
inTuples   = sys.argv[4:]

inNames   = []
inMinVals = []
inMaxVals = []
for inTuple in inTuples:
    tokens = inTuple.split(",")
    if len(tokens) != 3:
        print "Unexpected input format: ",inTuple
        exit(2)
    inNames.append(tokens[0])
    inMinVals.append(float(tokens[1]))
    inMaxVals.append(float(tokens[2]))

inHists  = {}
outHists = {}

# Get the input histos
for histName in inputFile.GetKeyNames():
    hist = inputFile.Get(histName)
    
    foundHist = False
    for inName,inMin,inMax in zip(inNames,inMinVals,inMaxVals):
        if TString(histName).Contains(inName):
            foundHist = True
            jetDef = getJetDef(histName)
            if not (jetDef in inHists):
                inHists[jetDef] = {}
                inHists[jetDef]["hist"] = []
                inHists[jetDef]["min"]  = []
                inHists[jetDef]["max"]  = []
            inHists[jetDef]["hist"].append(hist)
            inHists[jetDef]["min"].append(inMin)
            inHists[jetDef]["max"].append(inMax)

    if not foundHist:
        outputFile.cd()
        hist.Write(histName)

# Build the output histos
for aJetDef,aHist in inHists.iteritems():
    hists   = aHist["hist"]
    minVals = aHist["min"]
    maxVals = aHist["max"]

    xBins = []
    for iX in range(1,hists[0].GetXaxis().GetNbins()+2):
        xBins.append(hists[0].GetXaxis().GetBinLowEdge(iX))

    yBins = []
    for iY in range(1,hists[0].GetYaxis().GetNbins()+2):
        yBins.append(hists[0].GetYaxis().GetBinLowEdge(iY))

    # Set in dicts by minVal to sort, then ask if maxVal is next minVal
    zDict = {}
    zBins = []
    for minVal,maxVal,hist in zip(minVals,maxVals,hists):
        zDict[minVal] = maxVal
    for minVal,maxVal in sorted(zDict.iteritems()):
        #print "Min val: ",minVal," max val:",maxVal
        if not zBins:
            zBins.append(minVal)
            zBins.append(maxVal)
        elif fabs(zBins[-1]-minVal)<1.e-3:
            zBins.append(maxVal)
        else:
            print "Min value does not correspond to previous max"
            print "Max value: ",zBins[-1]
            print "Min value: ",minVal
            exit(3)

    # Make the actual histos
    outHists[aJetDef] = TH3D(outName+"_"+aJetDef,outName+"_"+aJetDef,len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
    outHist = outHists[aJetDef]
    
    # Fill the histos
    for minVal,maxVal in sorted(zDict.iteritems()):
        index = 0
        for minValGlobal,iVal in zip(minVals,range(0,len(minVals))):
            if fabs(minValGlobal-minVal)<1.e-3:
                index = iVal

        hist = hists[index]
        for xBin in range(1,hist.GetXaxis().GetNbins()+1):
            for yBin in range(1,hist.GetYaxis().GetNbins()+1):
                outHist.SetBinContent(xBin,yBin,index+1,hist.GetBinContent(xBin,yBin))
    
    # Done filling this histo
    # Now write
    outputFile.cd()
    outHist.Write(outName+"_"+aJetDef)

outputFile.Close()
inputFile.Close()

