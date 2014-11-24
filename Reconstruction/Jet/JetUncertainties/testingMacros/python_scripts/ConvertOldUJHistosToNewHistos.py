# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys
import re
import array
from ROOT import *
def GetKeyNames(self,dir=""):
    self.cd(dir)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


def ScaleBins(self,factor,axis="x"):
    xBins = []
    for aBin in range(1,self.GetNbinsX()+2):
        xBins.append(self.GetXaxis().GetBinLowEdge(aBin))

    yBins = []
    if self.GetDimension() > 1:
        for aBin in range(1,self.GetNbinsY()+2):
            yBins.append(self.GetYaxis().GetBinLowEdge(aBin))

    zBins = []
    if self.GetDimension() > 2:
        for aBin in range(1,self.GetNbinsZ()+2):
            zBins.append(self.GetZaxis().GetBinLowEdge(aBin))

    if "x" in axis:
        xBins = [aVal*factor for aVal in xBins]
    if "y" in axis:
        yBins = [aVal*factor for aVal in yBins]
    if "z" in axis:
        zBins = [aVal*factor for aVal in zBins]

    histo = None
    if self.GetDimension() == 1:
        if isinstance(self,TH1C):
            histo = TH1C(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins))
        elif isinstance(self,TH1S):
            histo = TH1S(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins))
        elif isinstance(self,TH1I):
            histo = TH1I(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins))
        elif isinstance(self,TH1F):
            histo = TH1F(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins))
        elif isinstance(self,TH1D):
            histo = TH1D(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins))
        for xBin in range(1,self.GetNbinsX()+1):
            histo.SetBinContent(xBin,self.GetBinContent(xBin))
    elif self.GetDimension() == 2:
        if isinstance(self,TH2C):
            histo = TH2C(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins))
        elif isinstance(self,TH2S):
            histo = TH2S(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins))
        elif isinstance(self,TH2I):
            histo = TH2I(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins))
        elif isinstance(self,TH2F):
            histo = TH2F(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins))
        elif isinstance(self,TH2D):
            histo = TH2D(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins))
        for xBin in range(1,self.GetNbinsX()+1):
            for yBin in range(1,self.GetNbinsY()+1):
                histo.SetBinContent(xBin,yBin,self.GetBinContent(xBin,yBin))
    elif self.GetDimension() == 3:
        if isinstance(self,TH3C):
            histo = TH3C(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
        elif isinstance(self,TH3S):
            histo = TH3S(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
        elif isinstance(self,TH3I):
            histo = TH3I(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
        elif isinstance(self,TH3F):
            histo = TH3F(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
        elif isinstance(self,TH3D):
            histo = TH3D(self.GetName()+"_scaled",self.GetTitle(),len(xBins)-1,array.array('d',xBins),len(yBins)-1,array.array('d',yBins),len(zBins)-1,array.array('d',zBins))
        for xBin in range(1,self.GetNbinsX()+1):
            for yBin in range(1,self.GetNbinsY()+1):
                for zBin in range(1,self.GetNbinsZ()+1):
                    histo.SetBinContent(xBin,yBin,zBin,self.GetBinContent(xBin,yBin,zBin))
    return histo
        
TH1.ScaleBins = ScaleBins



if len(sys.argv) != 3:
    print "Incorrect number of arguments.  Expected the following:"
    print "     1. Output root file"
    print "     2. Input root file"
    exit(1)

outFileName = sys.argv[1]
inFileName  = sys.argv[2]


if not outFileName.endswith(".root"):
    print "Output file doesn't appear to be a root file:",outFile
    print "Blocking for safety"
    exit(2)
if not inFileName.endswith(".root"):
    print "Input file doesn't appear to be a root file:",inFileName
    print "Blocking for safety"
    exit(3)


outFile = TFile.Open(outFileName,"RECREATE")
inFile  = TFile.Open(inFileName,"READ")

# Get each hist and change names
# Also duplicate validity a bunch
validHist = inFile.Get("Valid_area")
#if validHist.GetXaxis().GetBinLowEdge(validHist.GetNbinsX()+1) > 5.0e3:
#    validHist = validHist.ScaleBins(1.e-3,"x")
#if validHist.GetYaxis().GetBinLowEdge(validHist.GetNbinsY()+1) > 5.0e2:
#    validHist = validHist.ScaleBins(1.e-3,"y")
outFile.cd()
validHist.Write("Valid_area")
for histName in inFile.GetKeyNames():
    hist = inFile.Get(histName)

    histNameConverted = None
    jetType="AntiKt10LCTopoTrimmedPtFrac5SmallR30"
    if histName.startswith("valid") or histName.startswith("Valid"):
        continue # Special case, we are duplicating this histogram
    elif histName.endswith(jetType+"_pT"):
        histNameConverted = re.sub(jetType+"_pT","pT_"+jetType,histName)
    elif histName.endswith(jetType+"_Pileup_NPV"):
        histNameConverted = re.sub(jetType+"_Pileup_NPV","Pileup_NPV_"+jetType,histName)
    elif histName.endswith(jetType+"_Pileup_Mu"):
        histNameConverted = re.sub(jetType+"_Pileup_Mu","Pileup_Mu_"+jetType,histName)
    elif histName.endswith(jetType):
        histNameConverted = histName
    else:
        print "Unexpected hist name: ",histName
        exit(5)
    
    # Stretch pileup histos from TH1 to TH2, where 2nd dim is eta [-4.5,4.5] as a single bin (for equivalent with small-R)
    converted = hist
    if "Pileup" in histName:
        # 1D (pT) --> 2D(pT,eta)
        bins = []
        for aBin in range(1,hist.GetNbinsX()+2):
            bins.append(hist.GetXaxis().GetBinLowEdge(aBin))
        binArray = array.array('d',bins)

        etaBins=[-4.5,4.5]
        etaBinArray = array.array('d',etaBins)
        
        converted = TH2D(histName+"_2D",hist.GetTitle(),len(bins)-1,binArray,len(etaBins)-1,etaBinArray)

        for aBin in range(1,hist.GetNbinsX()+1):
            if fabs(hist.GetXaxis().GetBinLowEdge(aBin) - converted.GetXaxis().GetBinLowEdge(aBin)) > 1.e-4:
                print "ERROR: Bin edges differ, %f vs %f"%(hist.GetXaxis().GetBinLowEdge(aBin),converted.GetXaxis().GetBinLowEdge(aBin))
                exit(4)
            converted.SetBinContent(aBin,1,hist.GetBinContent(aBin))

    # Scale if necessary from MeV to GeV
    #if converted.GetXaxis().GetBinLowEdge(converted.GetNbinsX()+1) > 5.0e3:
    #    converted = converted.ScaleBins(1.e-3,"x")
    #if converted.GetYaxis().GetBinLowEdge(converted.GetNbinsY()+1) > 5.0e2:
    #    converted = converted.ScaleBins(1.e-3,"y")


    outFile.cd()
    converted.Write(histNameConverted)
    if not "Pileup" in histNameConverted:
        validHist.Write("valid_"+histNameConverted)

inFile.Close()
outFile.Close()

