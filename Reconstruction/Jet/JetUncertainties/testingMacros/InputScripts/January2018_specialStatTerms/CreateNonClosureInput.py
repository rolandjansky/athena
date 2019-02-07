from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

## Right now we don't support AFII but we still need non-closure histograms for Pythia8.
# Create blank ones and add them.

SystematicNameDictionary = {
                            'RelativeNonClosure_AFII_AntiKt4EMTopo' : 'RelativeNonClosure_AFII',
                            'RelativeNonClosure_AFII_AntiKt4EMPFlow' : 'RelativeNonClosure_AFII'
                           }

jetDefList =    [ 'AntiKt4Topo_EMJES', 'AntiKt4PFlow_EMJES']

def CreateNonClosureHistograms():

    histos = {}
    for jetDef in jetDefList :
      histos[jetDef] = {}

      # Add a blank nonclosure histo for Pythia8
      # Also add a blank Closeby histo for Pythia8
      etaBins = [min(ProviderHistoHelpers.GetDefaultEtaBins()),0,max(ProviderHistoHelpers.GetDefaultEtaBins())]
      histos[jetDef]["RelativeNonClosure_MC16"] = ProviderHistoHelpers.MakeProviderHisto("RelativeNonClosure_MC16",ptBins=[15,1000,2500],etaBins=etaBins)
      histos[jetDef]["RelativeNonClosure_MC16"].SetDirectory(0)
      histos[jetDef]["CloseBy_Pythia8"] = ProviderHistoHelpers.MakeProviderHisto("CloseBy_MC16",ptBins=[15,1000,2500],etaBins=etaBins)
      histos[jetDef]["CloseBy_Pythia8"].SetDirectory(0)

    return histos
