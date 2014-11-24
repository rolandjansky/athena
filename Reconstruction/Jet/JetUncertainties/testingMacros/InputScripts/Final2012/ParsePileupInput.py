# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNames = ['Pileup_PtTerm_NPV','Pileup_PtTerm_Mu','Pileup_OffsetNPV','Pileup_OffsetMu','Pileup_RhoTopology']

jetDefs = ['AntiKt4Topo_EMJES','AntiKt6Topo_EMJES','AntiKt4Topo_LCJES','AntiKt6Topo_LCJES']

def ReadPileupHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # We're just taking it from the old ROOT file...
    # Ensure there is just one such file
    rootFileList = sorted(glob.glob(dirName+"*.root"))
    if len(rootFileList) != 1:
        print "Directory contains multiple or no root files - was expecting one:",dirName
        return None
    rootFile = TFile(rootFileList[0],"READ")

    histos = {}
    for aJetDef in jetDefs:
        histos[aJetDef] = {}

        # Loop over the desired systematics
        for aSystName in SystematicNames:
            systematicName = aSystName + "_" + aJetDef
            histo = rootFile.Get(systematicName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None
            histo.SetDirectory(0)

            histos[aJetDef][aSystName] = histo

    # Done reading, close the file
    rootFile.Close()

    return histos



