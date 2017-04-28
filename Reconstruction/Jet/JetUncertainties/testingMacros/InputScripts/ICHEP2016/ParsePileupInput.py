# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNames = ['Pileup_OffsetNPV','Pileup_OffsetMu']#,'Pileup_PtTerm_NPV','Pileup_PtTerm_Mu','Pileup_RhoTopology']

jetDefs = {'AntiKt4EMTopo':'AntiKt4Topo_EMJES','AntiKt4LCTopo':'AntiKt4Topo_LCJES'}#,'AntiKt6Topo_EMJES','AntiKt4Topo_LCJES','AntiKt6Topo_LCJES']

def ReadPileupHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # We're just taking it from the old ROOT file...
    # Ensure there is just one such file
    rootFileList = sorted(glob.glob(dirName+"*.root"))

    histos = {}

    for aJetDef in jetDefs.keys():
        histos[jetDefs[aJetDef]] = {}

        for file in rootFileList :
            rootFile = TFile(file,"READ")

            # Loop over the desired systematics
            for aSystName in SystematicNames:
                systematicName = aSystName + "_" + aJetDef
                if systematicName not in rootFile.GetKeyNames() :
                    continue
                histo = rootFile.Get(systematicName)
                if histo is None:
                    print "Failed to get histogram:",systematicName
                    return None
            
                histo.SetDirectory(0)

                histos[jetDefs[aJetDef]][aSystName] = histo

            # Done reading, close the file
            rootFile.Close()

        # one extra file: rho topology terms are from 2012
        # and pt terms have been calculated once for 2015 by Steve A.
        # Inputs for both collections were collated in a single file
        rootFile = TFile("/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/inputs/Moriond2016/Pileup/compileVariousPileupTerms_KateJuly31.root","READ")
        for term in ['Pileup_PtTerm_NPV','Pileup_PtTerm_Mu','Pileup_RhoTopology'] :
          histo = rootFile.Get(term+"_"+aJetDef)
          histo.SetDirectory(0)
          histos[jetDefs[aJetDef]][term] = histo

    return histos



