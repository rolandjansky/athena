# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers


SystematicNameDictionary = {
                            'herwigpp'              : 'bJES_Generator',
                            'distorted_geometry'    : 'bJES_Geometry',
                            'FTFP_BERT'             : 'bJES_HadIntModel'
                           }

jetDefDict =    {
                    'AntiKt4TopoEM' : 'AntiKt4Topo_EMJES',
                    'AntiKt6TopoEM' : 'AntiKt6Topo_EMJES',
                    'AntiKt4LCTopo' : 'AntiKt4Topo_LCJES',
                    'AntiKt6LCTopo' : 'AntiKt6Topo_LCJES'
                }

def ReadBJESHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # Run over each file (one per jet definition)
    # Skip files that don't start with b-JES and contain GSC
    histos = {}
    files = sorted(glob.glob(dirName+"b-JES*GSC.root"))
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
        for aName,aSystName in SystematicNameDictionary.iteritems():
            systematicName = aSystName + "_" + jetDef
            histo = inFile.Get(aName)
            if histo is None:
                print "Failed to get histogram:",systematicName
                return None

            histo2D = ProviderHistoHelpers.ConvertTGraphToProviderHisto(histo,systematicName)
            histo2D.SetDirectory(0)
            
            # It's currently response, set to difference from 1
            for ptBin in range(1,histo2D.GetNbinsX()+1):
                for etaBin in range(1,histo2D.GetNbinsY()+1):
                    histo2D.SetBinContent(ptBin,etaBin,1-histo2D.GetBinContent(ptBin,etaBin))
            
            histos[jetDef][aSystName] = histo2D

        # Done reading, close the file
        inFile.Close()

        # Make one quadrature sum histo of the desired components for now
        # The new provider design should make this irrelevant for multijet components too, but for now...
        for aDef,aHistoSet in histos.iteritems():
            # We want one histo of herwigpp and distorted_geometry (leave out FTFP_BERT)
            histos[aDef]['bJES'] = ProviderHistoHelpers.QuadratureSumHisto('bJES',[histos[aDef]['bJES_Generator'],histos[aDef]['bJES_Geometry']])
            histos[aDef]['bJES'].SetDirectory(0)
    
    return histos

