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

jetDefList = ['AntiKt4Topo_EMJES']#,"AntiKt4Topo_LCJES"]#,'AntiKt6Topo_EMJES','AntiKt4Topo_LCJES','AntiKt6Topo_LCJES']

def ReadHighPtHistogramsFromOldFile(fileName) :

    histos = {}
    for jetDef in jetDefList :
      histos[jetDef] = {}
    inFile = TFile(fileName,"READ")
    uncNames = ["SingleParticle_HighPt"]
    jetCollections = {"AntiKt4EMTopo" : "AntiKt4Topo_EMJES"}#, "AntiKt4LCTopo" : "AntiKt4Topo_LCJES"}
    for name in uncNames :
      for jetCollection in jetCollections.keys() :
        getJetType = jetCollections[jetCollection]
        fetchName = name+"_"+getJetType
        hist = inFile.Get(fetchName)
        hist.SetDirectory(0)
        histos[jetCollections[jetCollection]][name] = hist

    return histos

def ReadHighPtHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # Run over the two files
    emFileList = sorted(glob.glob(dirName+"EM*/*.root"))
    lcFileList = sorted(glob.glob(dirName+"LC*/*.root"))
    if len(emFileList) != 1:
        print "Found a number of EM root files not equal to 1 in dir:",dirName
        return None
    if len(lcFileList) != 1:
        print "Found a number of LC root files not equal to 1 in dir:",dirName
        return None
    emFile = TFile(emFileList[0],"READ")
    lcFile = TFile(lcFileList[0],"READ")

    histos = {}
    for aJetDef in jetDefList:
        histos[aJetDef] = {}

        rootFile = None
        if "EMJES" in aJetDef:
            rootFile = emFile
        elif "LCJES" in aJetDef:
            rootFile = lcFile
        else:
            print "Unexpected jet def:",aJetDef
            return None
        
        for histName in rootFile.GetKeyNames():
            if aJetDef not in histName: continue
            histo = rootFile.Get(histName)
            if histo is None:
                print "Failed to get histogram:",histName
                return None
            histo.SetName(histName+"_1D")

            histos[aJetDef][re.sub("_%s"%(aJetDef),"",histName)] = ProviderHistoHelpers.ConvertPtHistoToProviderHisto(histo,histName)
            histos[aJetDef][re.sub("_%s"%(aJetDef),"",histName)].SetDirectory(0)

    lcFile.Close()
    emFile.Close()

    return histos

