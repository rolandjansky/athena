# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import sys
import re
import array
from math import fabs
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

if len(sys.argv) < 7:
    print "Too few arguments.  Expected the following:"
    print "  1. Input file"
    print "  2. Output file"
    print "  3. Name of histogram to modify"
    print "  4. Axis (x, y, or z)"
    print "  5. New lower value, -999 to leave unchanged"
    print "  6. New upper value, -999 to leave unchanged"
    sys.exit(1)

# Arguments
inFile     = TFile.Open(sys.argv[1],"READ")
outFile    = TFile.Open(sys.argv[2],"RECREATE")
histName   = sys.argv[3]
axis       = sys.argv[4]
lowBound   = float(sys.argv[5])
highBound  = float(sys.argv[6])
changeLow  = not (fabs(lowBound+999) < 1.e-4)
changeHigh = not (fabs(highBound+999) < 1.e-4)

# Sanity checks
if histName not in inFile.GetKeyNames():
    print "The requested histogram is not in the input file:",histName
    sys.exit(2)
if axis not in ["x","y","z","X","Y","Z"]:
    print "Axis is not x, y, or z:",axis
    sys.exit(2)
if (not changeLow) and (not changeHigh):
    print "Requesting to change neither the low or high boundary, nothing to do"
    sys.exit(2)
if changeLow and changeHigh and lowBound > highBound:
    print "Requesting a new lower boundary that is below the upper boundary"
    sys.exit(2)

# Write histograms, changing the one requested
for aHist in inFile.GetKeyNames():
    hist = inFile.Get(aHist)
    if aHist != histName:
        outFile.cd()
        hist.Write(aHist)
    else:
        # Build the histogram bins
        binsX = []
        for xBin in range(1,hist.GetNbinsX()+2):
            binsX.append(hist.GetXaxis().GetBinLowEdge(xBin))
        binsY = []
        if hist.GetDimension() > 1:
            for yBin in range(1,hist.GetNbinsY()+2):
                binsY.append(hist.GetYaxis().GetBinLowEdge(yBin))
        binsZ = []
        if hist.GetDimension() > 2:
            for zBin in range(1,hist.GetNbinsZ()+2):
                binsZ.append(hist.GetZaxis().GetBinLowEdge(zBin))

        # Check which one we are changing
        refBins = None
        if axis == "x" or axis == "X":
            refBins = binsX
        elif axis == "y" or axis == "Y":
            if hist.GetDimension() > 1:
                refBins = binsY
            else:
                print "Requested y axis for a 1D histogram"
                sys.exit(3)
        elif axis == "z" or axis == "Z":
            if hist.GetDimension() > 2:
                refBins = binsZ
            else:
                print "Requested z axis for a 1D or 2D histogram"
                sys.exit(3)
        else:
            print "Couldn't interpret axis request:",axis
            sys.exit(3)

        # Modify bins as needed
        newBins = []
        if changeLow and lowBound > refBins[-1]:
            print "New low bin is above the upper bound of the histogram"
            sys.exit(4)
        if changeHigh and highBound < refBins[0]:
            print "New high bin is below the lower bound of the histogram"
            sys.exit(4)
        if changeLow:
            newBins.append(lowBound)
        for aBin in refBins:
            if changeLow and fabs(aBin-lowBound) < 1.e-4:
                continue # Added before
            elif changeLow and aBin < lowBound:
                continue
            elif changeHigh and fabs(aBin-highBound) < 1.e-4:
                continue # Adding after
            elif changeHigh and aBin > highBound:
                continue
            newBins.append(aBin)
        if changeHigh:
            newBins.append(highBound)
        
        # Map old bins to new bins
        # Very stupid brute force due to time constraints
        binPairs = []
        for aBin,aBinI in zip(refBins,range(1,len(refBins)+1)):
            for newBin,newBinI in zip(newBins,range(1,len(newBins)+1)):
                if fabs(aBin-newBin)<1.e-4:
                    binPairs.append([aBinI,newBinI])
        if changeLow and binPairs[0][1] != 1:
            if newBins[0] < refBins[0]:
                binPairs.insert(0,[1,1]) # hold fixed to new bins
            else:
                binPairs.insert(0,[binPairs[0][0]-1,binPairs[0][1]-1])
        if changeHigh and binPairs[0][-1] != len(binPairs):
            if newBins[-1] > refBins[-1]:
                binPairs.append([len(refBins),len(newBins)]) # hold fixed to new bins
            else:
                binPairs.append([binPairs[-1][0]+1,binPairs[-1][1]+1])
        if len(binPairs) != len(newBins):
            print "Error: mismatched numbers of bins and bin pairs"
            sys.exit(5)

                
        # Make the histogram
        # Doing this brute-force for now...
        newHist = None
        if hist.GetDimension() == 1:
            if isinstance(hist,TH1F):
                newHist = TH1F(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('f',newBins))
            elif isinstance(hist,TH1D):
                newHist = TH1D(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('d',newBins))
            else:
                print "Unsupported type of 1D histogram"
                sys.exit(5)
        elif hist.GetDimension() == 2:
            if isinstance(hist,TH2F):
                if axis == "x" or axis == "X":
                    newHist = TH2F(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('f',newBins),len(binsY)-1,array.array('f',binsY))
                elif axis == "y" or axis == "Y":
                    newHist = TH2F(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('f',binsX),len(newBins)-1,array.array('f',newBins))
            elif isinstance(hist,TH2D):
                if axis == "x" or axis == "X":
                    newHist = TH2D(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('d',newBins),len(binsY)-1,array.array('d',binsY))
                elif axis == "y" or axis == "Y":
                    newHist = TH2D(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('d',binsX),len(newBins)-1,array.array('d',newBins))
            else:
                print "Unsupported type of 2D histogram"
                sys.exit(5)
        elif hist.GetDimension() == 3:
            if isinstance(hist,TH3F):
                if axis == "x" or axis == "X":
                    newHist = TH3F(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('f',newBins),len(binsY)-1,array.array('f',binsY),len(binsZ)-1,array.array('f',binsZ))
                elif axis == "y" or axis == "Y":
                    newHist = TH3F(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('f',binsX),len(newBins)-1,array.array('f',newBins),len(binsZ)-1,array.array('f',binsZ))
                elif axis == "z" or axis == "Z":
                    newHist = TH3F(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('f',binsX),len(binsY)-1,array.array('f',binsY),len(newBins)-1,array.array('f',newBins))
            elif isinstance(hist,TH3D):
                if axis == "x" or axis == "X":
                    newHist = TH3D(histName+"_mod",hist.GetTitle(),len(newBins)-1,array.array('d',newBins),len(binsY)-1,array.array('d',binsY),len(binsZ)-1,array.array('d',binsZ))
                elif axis == "y" or axis == "Y":
                    newHist = TH3D(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('d',binsX),len(newBins)-1,array.array('d',newBins),len(binsZ)-1,array.array('d',binsZ))
                elif axis == "z" or axis == "Z":
                    newHist = TH3D(histName+"_mod",hist.GetTitle(),len(binsX)-1,array.array('d',binsX),len(binsY)-1,array.array('d',binsY),len(newBins)-1,array.array('d',newBins))
            else:
                print "Unsupported type of 3D histogram"
                sys.exit(5)
        else:
            print "Unsupported histogram dimensionality"
            sys.exit(5)

        # Now we have the histogram, let's fill it
        # Done this way now as it runs faster (although less compact for humans...)
        if hist.GetDimension() == 1:
            for oldBin,newBin in binPairs:
                newHist.SetBinContent(newBin,hist.GetBinContent(oldBin))
                newHist.SetBinError(newBin,hist.GetBinError(oldBin))
        if hist.GetDimension() == 2:
            if axis == "x" or axis == "X":
                for oldBin,newBin in binPairs:
                    for yBin in range(1,hist.GetNbinsY()+1):
                        newHist.SetBinContent(newBin,yBin,hist.GetBinContent(oldBin,yBin))
                        newHist.SetBinError(newBin,yBin,hist.GetBinError(oldBin,yBin))
            else:
                for xBin in range(1,hist.GetNbinsX()+1):
                    for oldBin,newBin in binPairs:
                        newHist.SetBinContent(xBin,newBin,hist.GetBinContent(xBin,oldBin))
                        newHist.SetBinError(xBin,newBin,hist.GetBinError(xBin,oldBin))
        if  hist.GetDimension() == 3:
            if axis == "x" or axis == "X":
                for oldBin,newBin in binPairs:
                    for yBin in range(1,hist.GetNbinsY()+1):
                        for zBin in range(1,hist.GetNbinsZ()+1):
                            newHist.SetBinContent(newBin,yBin,zBin,hist.GetBinContent(oldBin,yBin,zBin))
                            newHist.SetBinError(newBin,yBin,zBin,hist.GetBinError(oldBin,yBin,zBin))
            elif axis == "y" or axis == "Y":
                for xBin in range(1,hist.GetNbinsX()+1):
                    for oldBin,newBin in binPairs:
                        for zBin in range(1,hist.GetNbinsZ()+1):
                            newHist.SetBinContent(xBin,newBin,zBin,hist.GetBinContent(xBin,oldBin,zBin))
                            newHist.SetBinError(xBin,newBin,zBin,hist.GetBinError(xBin,oldBin,zBin))
            else:
                for xBin in range(1,hist.GetNbinsX()+1):
                    for yBin in range(1,hist.GetNbinsY()+1):
                        for oldBin,newBin in binPairs:
                            newHist.SetBinContent(xBin,yBin,newBin,hist.GetBinContent(xBin,yBin,oldBin))
                            newHist.SetBinError(xBin,yBin,newBin,hist.GetBinError(xBin,yBin,oldBin))

        # Filled histogram, finally
        # Write it out with the old name
        outFile.cd()
        newHist.Write(histName)



outFile.Close()
inFile.Close()


