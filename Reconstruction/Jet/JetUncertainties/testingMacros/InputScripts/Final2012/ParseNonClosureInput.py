# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNameDictionary = {
                            'AFIIunc' : 'RelativeNonClosure_AFII'
                           }

jetDefDict =    {
                    '4EM' : 'AntiKt4Topo_EMJES',
                    '6EM' : 'AntiKt6Topo_EMJES',
                    '4LC' : 'AntiKt4Topo_LCJES',
                    '6LC' : 'AntiKt6Topo_LCJES'
                }

def ReadNonClosureHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over each file (one per jet definition)
    histos = {}
    files = sorted(glob.glob(dirName+"*.root"))
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
            systematicName = aSystName + "_" + jetDef
            histo = inFile.Get(aName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None
            
            histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histo,systematicName)
            histoSym.SetDirectory(0)
            histos[jetDef][aSystName] = histoSym

            # Also add a blank nonclosure histo for Pythia8
            # Also add a blank Closeby histo for both AFII and Pythia8
            etaBins = [min(ProviderHistoHelpers.GetDefaultEtaBins()),0,max(ProviderHistoHelpers.GetDefaultEtaBins())]
            histos[jetDef]["RelativeNonClosure_Pythia8"] = ProviderHistoHelpers.MakeProviderHisto("RelativeNonClosure_Pythia8",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["RelativeNonClosure_Pythia8"].SetDirectory(0)
            histos[jetDef]["CloseBy_Pythia8"] = ProviderHistoHelpers.MakeProviderHisto("CloseBy_Pythia8",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["CloseBy_Pythia8"].SetDirectory(0)
            histos[jetDef]["CloseBy_AFII"] = ProviderHistoHelpers.MakeProviderHisto("CloseBy_AFII",ptBins=[15,1000,2500],etaBins=etaBins)
            histos[jetDef]["CloseBy_AFII"].SetDirectory(0)
        
        # Done reading, close the file
        inFile.Close()

    return histos
