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

jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
#                    'AntiKt6TopoEM' : 'AntiKt6Topo_EMJES',
                    'AntiKt4LCTopo' : 'AntiKt4Topo_LCJES',
#                    'AntiKt6LCTopo' : 'AntiKt6Topo_LCJES'
                }

SystematicNameList = ['PunchThrough_MC15','PunchThrough_AFII']

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
            for aKey in SystematicNameList:
              if "_MC" in aKey :
                histName = aKey+"_"+aJetDef
                histo = rootFile.Get(histName)
                if histo is None:
                    print "Failed to get histogram:",histName
                    return None
                histo.SetName(aKey+"_"+aJetDefProv+"badaxes")
                histo.SetDirectory(0)
                histo.Print()

                # x axis is pT.
                # y axis is eta
                # z axis is NSegments
                # Last two axes need to get swapped to be compatible with 8TeV!!!!

                pTBinArray = []
                nPTBins = histo.GetXaxis().GetNbins()
                for bin in range(1,nPTBins+2) :
                  binEdge = histo.GetXaxis().GetBinLowEdge(bin)
                  pTBinArray.append(binEdge)
                pTBinArray = array('d',pTBinArray)

                etaBinArray = []
                nEtaBins = histo.GetYaxis().GetNbins()
                for bin in range(1,nEtaBins+2) :
                  binEdge = histo.GetYaxis().GetBinLowEdge(bin)
                  etaBinArray.append(binEdge)
                etaBinArray = array('d',etaBinArray)

                nSegmentsBinArray = []
                nNSegBins = histo.GetZaxis().GetNbins()
                for bin in range(1,nNSegBins+2) :
                  binEdge = histo.GetZaxis().GetBinLowEdge(bin)
                  nSegmentsBinArray.append(binEdge)
                nSegmentsBinArray = array('d',nSegmentsBinArray)

                outhisto = TH3D(aKey+"_"+aJetDefProv,aKey+"_"+aJetDefProv,nPTBins,pTBinArray,nNSegBins,nSegmentsBinArray,nEtaBins,etaBinArray)
                outhisto.SetDirectory(0)
             
                # Fix histo margins to make sure it only applies the calibration where it should.
                # Loop over all bins in x, y, z.
                for xbin in range(histo.GetNbinsX()+2) :
                  for ybin in range(histo.GetNbinsY()+2) :
                    for zbin in range(histo.GetNbinsZ()+2) :
                      # Permitted regions are only:
                      #  - pT > 50 GeV
                      #  - eta < 2.7
                      #  - NSegments > 20

                      if histo.GetXaxis().GetBinCenter(xbin) < 50 or \
                          histo.GetYaxis().GetBinCenter(ybin) > 2.7 or \
                          histo.GetZaxis().GetBinCenter(zbin) < 20 :
                        histo.SetBinContent(xbin,ybin,zbin,0.0)
#                        print "Setting bin at pT",histo.GetXaxis().GetBinCenter(xbin),\
#                          ", eta",histo.GetYaxis().GetBinCenter(ybin),\
#                          ", nSegments",histo.GetZaxis().GetBinCenter(zbin),"to zero"
                        outhisto.SetBinContent(xbin,zbin,ybin,0.0)
                      else :
                        outhisto.SetBinContent(xbin,zbin,ybin,histo.GetBinContent(xbin,ybin,zbin))


                histo.Print()
                outhisto.Print()

                histos[aJetDefProv][aKey] = outhisto

              # Add a blank histogram for AFII corresponding to this
              else :
                outhisto = TH3D(aKey+"_"+aJetDefProv,aKey+"_"+aJetDefProv,2,15.0,4500.0,2,0.0,1000.0,2,-4.5,4.5)
                outhisto.SetDirectory(0)
                histos[aJetDefProv][aKey] = outhisto

    print histos

    # Done reading, close the file
    rootFile.Close()

    return histos

