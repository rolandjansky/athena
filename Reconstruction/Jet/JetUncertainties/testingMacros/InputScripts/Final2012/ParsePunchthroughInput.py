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
                            'PunchThroughUncertainties_interpolation_resp_eta_0' : 'PunchThroughCentral',
                            'PunchThroughUncertainties_interpolation_resp_eta_1' : 'PunchThroughCrack',
                            'PunchThroughUncertainties_interpolation_resp_eta_2' : 'PunchThroughForward'
                           }

jetDefDict =    {
                    'AntiKt4TopoEM' : 'AntiKt4Topo_EMJES',
                    'AntiKt6TopoEM' : 'AntiKt6Topo_EMJES',
                    'AntiKt4LCTopo' : 'AntiKt4Topo_LCJES',
                    'AntiKt6LCTopo' : 'AntiKt6Topo_LCJES'
                }

#SystematicNameList = ['PunchThroughCentral','PunchThroughCrack','PunchThroughForward']
#jetDefList = ['AntiKt4Topo_EMJES','AntiKt6Topo_EMJES','AntiKt4Topo_LCJES','AntiKt6Topo_LCJES']

def ReadPunchthroughHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over the one file
    rootFileList = sorted(glob.glob(dirName+"*.root"))
    if len(rootFileList) != 1:
        print "Found a number of root files not equal to 1 in dir:",dirName
        return None
    rootFile = TFile(rootFileList[0],"READ")

    histos = {}
    for aJetDef,aJetDefProv in jetDefDict.iteritems():
        histos[aJetDefProv] = {}

        for histName in rootFile.GetKeyNames():
            if aJetDef not in histName: continue
            for aKey,aVal in SystematicNameDictionary.iteritems():
                histName = aJetDef+"_"+aKey
                histo = rootFile.Get(histName)
                if histo is None:
                    print "Failed to get histogram:",histName
                    return None
                histo.SetName(aVal+"_"+aJetDefProv)
                histo.SetDirectory(0)

                histos[aJetDefProv][aVal] = histo

    # Done reading, close the file
    rootFile.Close()

    ## Curently it's just a flat histogram of 0
    #histos = {}
    #for aJetDef in jetDefList:
    #    histos[aJetDef] = {}
    #    
    #    for aSystName in SystematicNameList:
    #        histo = ProviderHistoHelpers.MakeProviderHisto(aSystName+"_"+aJetDef,etaBins=[0,19.25,19.75,20,1000])
    #        for xBin in range(1,histo.GetNbinsX()+1):
    #            histo.SetBinContent(xBin,1,0)
    #            histo.SetBinContent(xBin,2,0)
    #            histo.SetBinContent(xBin,3,0.1)
    #            histo.SetBinContent(xBin,4,0.1)
    #        histo.SetDirectory(0)
    #        histos[aJetDef][aSystName] = histo

    return histos

