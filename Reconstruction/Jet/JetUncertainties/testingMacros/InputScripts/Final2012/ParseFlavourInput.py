# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


SystematicNameDictionary = {
                            'LQ'                    : 'flavorCompLight',
                            'Gluon'                 : 'flavorCompGlu',
                            'HerwigVsPythia_Gluon'  : 'FlavorResponse'
                           }

jetDefDict =    {
                    '4EM' : 'AntiKt4Topo_EMJES',
                    '6EM' : 'AntiKt6Topo_EMJES',
                    '4LC' : 'AntiKt4Topo_LCJES',
                    '6LC' : 'AntiKt6Topo_LCJES'
                }

def ReadFlavourHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over each file (one per jet definition for LQ/Gluon and one per jet definition for HvP)
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
        if jetDef not in histos:
            histos[jetDef] = {}

        # Read in the histograms from the file
        # Note that we want all the histograms, but with this scheme not everything in the dict is in each file
        # So instead loop over all histos in the file
        inFile = TFile(aFileName,"READ")
        for histName in inFile.GetKeyNames():
            systematicName = SystematicNameDictionary[histName]+"_"+jetDef
            histo = inFile.Get(histName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None

            histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histo,systematicName)
            histoSym.SetDirectory(0)
            histos[jetDef][SystematicNameDictionary[histName]] = histoSym
        # Done reading, close the file
        inFile.Close()

    return histos



