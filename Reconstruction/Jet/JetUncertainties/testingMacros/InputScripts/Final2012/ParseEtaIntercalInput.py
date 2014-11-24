# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

SystematicNames = ['EtaIntercalibration_TotalStat','EtaIntercalibration_Modelling']

jetDefDict =    {
                    'AntiKt4TopoEM' : 'AntiKt4Topo_EMJES',
                    'AntiKt6TopoEM' : 'AntiKt6Topo_EMJES',
                    'AntiKt4LCTopo' : 'AntiKt4Topo_LCJES',
                    'AntiKt6LCTopo' : 'AntiKt6Topo_LCJES'
                }

def ReadEtaIntercalibrationHistograms(dirName):
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

        # Read in the histograms of interest from the file
        inFile = TFile(aFileName,"READ")
        for aName in SystematicNames:
            systematicName = aName + "_" + jetDef
            histo = inFile.Get(systematicName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None
            histo.SetName(systematicName+"_old")
            
            # Histo has different range, extend it
            histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,systematicName)
            histoNew.SetDirectory(0)
            histos[jetDef][aName] = histoNew

        # Done reading, close the file
        inFile.Close()
    
    return histos
