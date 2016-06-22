# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

SystematicNames = {'EtaIntercalibration_TotalStat' : 'EtaIntercalibration_TotalStat',\
                   'EtaIntercalibration_TotalSyst' : 'EtaIntercalibration_Modelling',\
                   'EtaIntercalibration_NonClosure' : 'EtaIntercalibration_NonClosure'
                 }

jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
                    'AntiKt6EMTopo' : 'AntiKt6Topo_EMJES',
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
        if jetDef not in histos.keys() :
          histos[jetDef] = {}

        # Read in the histograms of interest from the file
        inFile = TFile(aFileName,"READ")
        for aName in SystematicNames.keys():
            getsystematicName = aName + "_" + jetDef.replace("Topo","")
            systematicName = SystematicNames[aName] + "_" + jetDef

            histo = inFile.Get(getsystematicName)
            if not histo :
                print "Failed to get histogram:",getsystematicName
                continue
            histo.SetName(systematicName+"_old")
 
            # Histo has different range, extend it
            histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,systematicName)
            histoNew.SetDirectory(0)
            histos[jetDef][SystematicNames[aName]] = histoNew

        # Done reading, close the file
        inFile.Close()
    
    return histos
