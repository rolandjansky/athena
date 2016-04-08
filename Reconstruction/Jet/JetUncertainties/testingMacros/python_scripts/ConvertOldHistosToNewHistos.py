# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys
import re
import array
import math
from ROOT import *
def GetKeyNames(self,dir=""):
    self.cd(dir)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

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

for histName in inFile.GetKeyNames():
    hist = inFile.Get(histName)

    converted = hist
    if histName.startswith("Zjet") or histName.startswith("MPF") or histName.startswith("Gjet") or histName.startswith("MJB") or histName.startswith("Effective") or histName.startswith("SingleParticle") or histName.startswith("Insitu") or histName.startswith("InSitu"):
        # 2D(pT,eta) --> 1D(pT)
        bins = []
        for aBin in range(1,hist.GetNbinsX()+2):
            bins.append(hist.GetXaxis().GetBinLowEdge(aBin))

        binArray = array.array('d',bins)
        converted = TH1D(histName+"_1D",hist.GetTitle(),len(bins)-1,binArray)
        
        for aBin in range(1,hist.GetNbinsX()+1):
            if math.fabs(hist.GetXaxis().GetBinLowEdge(aBin) - converted.GetXaxis().GetBinLowEdge(aBin)) > 1.e-4:
                print "ERROR: Bin edges differ, %f vs %f"%(hist.GetXaxis().GetBinLowEdge(aBin),converted.GetXaxis().GetBinLowEdge(aBin))
                exit(4)
            converted.SetBinContent(aBin,hist.GetBinContent(aBin,1))

    histNameConverted = None
    if histName.endswith("AntiKt4Topo_LCJES"):
        histNameConverted = re.sub("AntiKt4Topo_LCJES","AntiKt4LCTopo",histName)
    elif histName.endswith("AntiKt4Topo_EMJES"):
        histNameConverted = re.sub("AntiKt4Topo_EMJES","AntiKt4EMTopo",histName)
    elif histName.endswith("AntiKt6Topo_LCJES"):
        histNameConverted = re.sub("AntiKt6Topo_LCJES","AntiKt6LCTopo",histName)
    elif histName.endswith("AntiKt6Topo_EMJES"):
        histNameConverted = re.sub("AntiKt6Topo_EMJES","AntiKt6EMTopo",histName)
    elif histName.endswith("AntiKt4EMTopo") or histName.endswith("AntiKt4LCTopo") or histName.endswith("AntiKt6EMTopo") or histName.endswith("AntiKt6LCTopo"):
        histNameConverted = histName
    else:
        print "Unexpected hist name: ",histName
        exit(5)

    # Watch for Pythia8==MC12
    if "Pythia8" in histNameConverted or "MC12a" in histNameConverted:
        histNameConverted = re.sub("Pythia8","MC12",re.sub("MC12a","MC12",histNameConverted))

    outFile.cd()
    converted.Write(histNameConverted)

inFile.Close()
outFile.Close()

