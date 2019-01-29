from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# One file from Eric with the offset terms.
# A few extra files: rho topology terms are newly computed from values from
# Jamie and Jacob (by me, see script in input directory)
# and pt terms have been calculated once for 2015 by Steve Alkire.
# pT term inputs for both collections were collated in a single file.

SystematicNames = ['Pileup_OffsetNPV','Pileup_OffsetMu','Pileup_PtTerm_npv','Pileup_PtTerm_mu','Pileup_RhoTopology']

jetDefs = {'AntiKt4EMTopo':'AntiKt4Topo_EMJES','AntiKt4EMPFlow':'AntiKt4PFlow_EMJES'}

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
                histo.SetDirectory(0)

                histos[jetDefs[aJetDef]][aSystName] = histo

            # Done reading, close the file
            rootFile.Close()


    # Did we get everything?
    for aJetDef in jetDefs.keys():
      print "Beginning jetDef",aJetDef
      for aSystName in SystematicNames:
        print "Beginning systName",aSystName
        if not aSystName in histos[jetDefs[aJetDef]].keys() :
          print "Failed to find histogram:",aSystName,"for jet definition",aJetDef

    return histos



