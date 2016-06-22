# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
#                    'AntiKt6EMTopo' : 'AntiKt6Topo_EMJES',
                    'AntiKt4LCTopo' : 'AntiKt4Topo_LCJES',
#                    'AntiKt6LCTopo' : 'AntiKt6Topo_LCJES'
                }

def ReadBJESHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # Run over each file (one per jet definition)
    # Skip files that don't start with bJES and contain GSC
    histos = {}
    files = sorted(glob.glob(dirName+"bJES*GSC.root"))

    if len(files) > 1 :
      print "Not what I expected!"
      return None
    aFileName = files[0]

    # Read in the histograms of interest from the file
    inFile = TFile(aFileName,"READ")
    for inFileDef in jetDefDict.keys() :
        jetDef = jetDefDict[inFileDef]
        if jetDef not in histos.keys() :
          histos[jetDef] = {}
        fetchName = "bJES_" + inFileDef
        systematicName = "bJES_" + jetDef
        histo = inFile.Get(fetchName)
        if histo is None:
          print "Failed to get histogram:",systematicName
          return None
        histo.SetDirectory(0)
        nbinsx = histo.GetNbinsX()
        binsx = histo.GetXaxis().GetXbins()
        newhisto = TH2D("bJES_extendedEta_{0}".format(jetDef),"bJES_extendedEta_{0}".format(jetDef),nbinsx,binsx.GetArray(),2,-4.4,4.4)
        newhisto.SetDirectory(0)

        # This input is symmetric in eta but doesn't extend far enough. 
        # Should be symmetric in eta: find max value in each row and set everywhere.
        # eta bins are y bins:
        for ptbin in range(1,histo.GetNbinsX()+1) :

          # Get max val
          maxVal = 0.0
          for etabin in range(1,histo.GetNbinsY()) :
            thisVal = histo.GetBinContent(ptbin,etabin)
            if thisVal > maxVal : maxVal = thisVal
          for etabin in range(1,newhisto.GetNbinsY()+1) :
            newhisto.SetBinContent(ptbin,etabin,maxVal)
         
        histos[jetDef]['bJES'] = newhisto

    # Done reading, close the file
    inFile.Close()

    return histos

