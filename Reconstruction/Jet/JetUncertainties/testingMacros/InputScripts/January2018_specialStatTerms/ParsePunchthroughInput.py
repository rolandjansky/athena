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
                    'AntiKt4EMPFlow' : 'AntiKt4PFlow_EMJES',
                }

SystematicNameList = ['PunchThrough_MC16','PunchThrough_AFII']

def ReadPunchthroughHistograms(dirName):
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Run over the one file
    rootFileList = sorted(glob.glob(dirName+"*.root"))
    fileDict = {}
    if len(rootFileList) != 1:
        print "Found a number of root files not equal to 1 in dir:",dirName
        print "Assume separate files for",
        for i in jetDefDict.keys() : print i,
        print
        for aJetDef in jetDefDict.keys() :
          for file in rootFileList :
            if aJetDef in file :
              if aJetDef not in fileDict.keys() :
                fileDict[aJetDef] = file
              else :
                print "Oh no! More than one file corresponding to this jet definition:",aJetDef
                return None
    else :
        print "Found only one root file: assume contents can be used for all jet collections."
        for aJetDef in jetDefDict.keys() :
          fileDict[aJetDef] = rootFileList[0]

    histos = {}
    for aJetDef,aJetDefProv in jetDefDict.iteritems():
        rootFile = TFile(fileDict[aJetDef],"READ")
        histos[aJetDefProv] = {}

        for histName in rootFile.GetKeyNames():
            if aJetDef not in histName: continue
            for aKey in SystematicNameList:
              if "_MC" in aKey :
                gethistName = aKey+"_"+aJetDef
                histo = rootFile.Get(gethistName)
                if histo is None:
                    print "Failed to get histogram:",histName
                    return None
                histo.SetName(aKey+"_"+aJetDefProv)
                histo.SetDirectory(0)

                # In Lydia's histogram:
                # x axis is pT.
                # y axis is eta
                # z axis is NSegments
                # Last two axes need to get swapped to be compatible with 8TeV!!!!

                # In Christopher's histogram,
                # these are back to being as requested.

                pTBinArray = []
                nPTBins = histo.GetXaxis().GetNbins()
                for bin in range(1,nPTBins+2) :
                  binEdge = histo.GetXaxis().GetBinLowEdge(bin)
                  pTBinArray.append(binEdge)
                pTBinArray = array('d',pTBinArray)

                etaBinArray = []
#                if "EM" in aJetDef :
#                  nEtaBins = histo.GetYaxis().GetNbins()
#                  for bin in range(1,nEtaBins+2) :
#                    binEdge = histo.GetYaxis().GetBinLowEdge(bin)
#                    etaBinArray.append(binEdge)
#                else :
                nEtaBins = histo.GetZaxis().GetNbins()
                for bin in range(1,nEtaBins+2) :
                  binEdge = histo.GetZaxis().GetBinLowEdge(bin)
                  etaBinArray.append(binEdge)
                etaBinArray = array('d',etaBinArray)

                nSegmentsBinArray = []
#                if "EM" in aJetDef :
#                  nNSegBins = histo.GetZaxis().GetNbins()
#                  for bin in range(1,nNSegBins+2) :
#                    binEdge = histo.GetZaxis().GetBinLowEdge(bin)
#                    nSegmentsBinArray.append(binEdge)
#                else :
                nNSegBins = histo.GetYaxis().GetNbins()
                for bin in range(1,nNSegBins+2) :
                  binEdge = histo.GetYaxis().GetBinLowEdge(bin)
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

#                      if "EM" in aJetDef :
#                        if histo.GetXaxis().GetBinCenter(xbin) < 50 or \
#                            histo.GetYaxis().GetBinCenter(ybin) > 2.7 or \
#                            histo.GetZaxis().GetBinCenter(zbin) < 20 :
#                          histo.SetBinContent(xbin,ybin,zbin,0.0)
#                          outhisto.SetBinContent(xbin,zbin,ybin,0.0)
#                        else :
#                          outhisto.SetBinContent(xbin,zbin,ybin,histo.GetBinContent(xbin,ybin,zbin))

#                      elif "LC" in aJetDef :
                        if histo.GetXaxis().GetBinCenter(xbin) < 50 or \
                            histo.GetZaxis().GetBinCenter(zbin) > 2.7 or \
                            histo.GetYaxis().GetBinCenter(ybin) < 20 :
                          histo.SetBinContent(xbin,ybin,zbin,0.0)
                          outhisto.SetBinContent(xbin,ybin,zbin,0.0)
                        else :
                          outhisto.SetBinContent(xbin,ybin,zbin,histo.GetBinContent(xbin,ybin,zbin))


                histos[aJetDefProv][aKey] = outhisto

              # Add a blank histogram for AFII corresponding to this
              else :
                outhisto = TH3D(aKey+"_"+aJetDefProv,aKey+"_"+aJetDefProv,2,15.0,4500.0,2,0.0,1000.0,2,-4.5,4.5)
                outhisto.SetDirectory(0)
                histos[aJetDefProv][aKey] = outhisto

    # Done reading, close the file
    rootFile.Close()
    print "Done punchthrough.\n"

    return histos

