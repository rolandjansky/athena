# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNames = ['Pileup_PtTerm_NPV','Pileup_PtTerm_Mu','Pileup_OffsetNPV','Pileup_OffsetMu']#,'Pileup_RhoTopology']

jetDefs = {'AntiKt4EMTopo':'AntiKt4Topo_EMJES'}#,'AntiKt6Topo_EMJES','AntiKt4Topo_LCJES','AntiKt6Topo_LCJES']

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
                if systematicName+"NEW" not in rootFile.GetKeyNames() :
                    continue
                histo = rootFile.Get(systematicName+"NEW")
                if histo is None:
                    print "Failed to get histogram:",systematicName
                    return None
            
                histo.SetDirectory(0)

                histos[jetDefs[aJetDef]][aSystName] = histo

            # Done reading, close the file
            rootFile.Close()

        # one extra file: rho topology term comes from current prerecs
        # THIS IS A GIANT FIXME
        rootFile = TFile("/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/share/JES_2015/Prerec/JESUncertainty_2015.root","READ")
        histo = rootFile.Get("Pileup_RhoTopology_AntiKt4EMTopo")
        histo.SetDirectory(0)
        histos[jetDefs[aJetDef]]['Pileup_RhoTopology'] = histo

    return histos



