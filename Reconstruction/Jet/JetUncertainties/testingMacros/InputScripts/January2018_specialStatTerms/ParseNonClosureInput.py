from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNameDictionary = {
                            'RelativeNonClosure_AFII_AntiKt4EMTopo' : 'RelativeNonClosure_AFII',
                            'RelativeNonClosure_AFII_AntiKt4EMPFlow' : 'RelativeNonClosure_AFII'
                           }

jetDefDict =    {
                    'EMTopo' : 'AntiKt4Topo_EMJES',
                    'EMPFlow' : 'AntiKt4PFlow_EMJES',
                }

def ReadNonClosureHistograms(dirName,freezepT=False):
    if dirName.endswith(".root") :
      files = [dirName]

    elif not dirName.endswith("/") :
      dirName = dirName + "/"
    print "Looking in dirName",dirName
    import os
    os.listdir(dirName)
    files = sorted(glob.glob(dirName+"*.root"))
   
    # Run over each file (one per jet definition)
    histos = {}
    for aFileName in files:
        # Determine the jet definition
        jetDef = ""
        for aFileDef,aJetDef in jetDefDict.iteritems():
            if aFileDef in aFileName:
                jetDef = aJetDef
                break
        if jetDef == "":
            print "Failed to determine jet definition for file:",aFileName
            return None
        histos[jetDef] = {}

        # Read in the histogram from the file
        inFile = TFile(aFileName,"READ")
        for aName,aSystName in SystematicNameDictionary.iteritems():
            print aName,aFileDef,aSystName
            if not aFileDef in aName : continue
            systematicName = aSystName + "_" + jetDef
            print systematicName
            histo = inFile.Get(aName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None
 
            if freezepT :
              # Loop over eta bins.
              for ybin in range(histo.GetNbinsY()+2) :
                # Loop out in pT.
                freezeval = 0
                for xbin in range(histo.GetNbinsX()+2) :
                  inbin = histo.GetBinContent(xbin,ybin)
                  #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
                  # Store bin contents as we go out: last one is one we want as frozen value.
                  if inbin > -0.5 :
                    freezeval = inbin
                  else :
                    histo.SetBinContent(xbin,ybin,freezeval)

           
            histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histo,systematicName)
            histoSym.SetDirectory(0)
            histos[jetDef][aSystName] = histoSym
            print "Added hist with name",aSystName,"for jetDef",jetDef

            # Also add a blank nonclosure histo for Pythia8
            # Also add a blank Closeby histo for both AFII and Pythia8
            etaBins = [min(ProviderHistoHelpers.GetDefaultEtaBins()),0,max(ProviderHistoHelpers.GetDefaultEtaBins())]
            histos[jetDef]["RelativeNonClosure_MC16"] = ProviderHistoHelpers.MakeProviderHisto("RelativeNonClosure_MC16",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["RelativeNonClosure_MC16"].SetDirectory(0)
            histos[jetDef]["CloseBy_Pythia8"] = ProviderHistoHelpers.MakeProviderHisto("CloseBy_MC16",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["CloseBy_Pythia8"].SetDirectory(0)
            histos[jetDef]["CloseBy_AFII"] = ProviderHistoHelpers.MakeProviderHisto("CloseBy_AFII",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["CloseBy_AFII"].SetDirectory(0)
        
        # Done reading, close the file
        inFile.Close()

    return histos
