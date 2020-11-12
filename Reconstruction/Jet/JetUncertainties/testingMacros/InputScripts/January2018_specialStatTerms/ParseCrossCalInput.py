from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

SystematicNameDictionary = {
  'Xcalib_50nsVs25ns_AntiKt4EMTopo':'Xcalib_50nsVs25ns',
  'Xcalib_5Vs4sample_AntiKt4EMTopo':'Xcalib_5Vs4sample',
  'Xcalib_TopoClustering_AntiKt4EMTopo':'Xcalib_TopoClustering',
  'Xcalib_EarlyData_AntiKt4EMTopo':'Xcalib_EarlyData',
  'Xcalib_NoiseThreshold_AntiKt4EMTopo':'Xcalib_NoiseThreshold',
  'Xcalib_UnderlyingEvent_AntiKt4EMTopo':'Xcalib_UnderlyingEvent'
  }

jetDefDict =    {
                    '' : 'AntiKt4Topo_EMJES',
                }

def ReadCrossCalHistograms(dirName,scaleCrossCalVal=1.0,freezeCrossCalAll=False,freezeCrossCalBarrel=False,freezeCrossCalCentral=False):
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

    for histName in inFile.GetKeyNames():

      # Skip validity histograms.
      if 'valid' in histName :
        continue

      systematicName = SystematicNameDictionary[histName]+"_"+jetDef
      histo = inFile.Get(histName)
      if histo is None:
        print "Failed to get histogram:",systematicName
        return None
      histo.SetName(systematicName+"_old")
 
      if freezeCrossCalAll or freezeCrossCalBarrel or freezeCrossCalCentral :
        # Loop over pT bins.
        for xbin in range(histo.GetNbinsX()+2) :
          # Loop out in eta.
          freezeval = 0
          for ybin in range(histo.GetNbinsY()+2) :
            #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
            # Store bin contents as we go out: last one is one we want as frozen value.
            eta = abs(histo.GetYaxis().GetBinCenter(ybin))
            if eta < 0.8 :
              freezeval = histo.GetBinContent(xbin,ybin)
            else :
              if (eta < 2.5 and freezeCrossCalBarrel) or freezeCrossCalAll :
                histo.SetBinContent(xbin,ybin,freezeval*scaleCrossCalVal)
              else :
                histo.SetBinContent(xbin,ybin,0)

                   
      # Histo has different range, extend it
      histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,systematicName)
      histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histoNew,systematicName)
      histoSym.SetDirectory(0)

      # If this is the 50 vs 25 ns histogram, we need to separate it by eta region.
      if '50nsVs25ns' in histName :
        CentralHist = histo.Clone()
        CentralHist.SetName("Central_holder")
        ForwardHist = histo.Clone()
        ForwardHist.SetName("Forward_holder")
        # Loop over pT bins.
        for xbin in range(histo.GetNbinsX()+2) :
          # Loop out in eta.
          freezeval = 0
          for ybin in range(histo.GetNbinsY()+2) :
            #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
            # Store bin contents as we go out: last one is one we want as frozen value.
            eta = abs(histo.GetYaxis().GetBinCenter(ybin))
            if eta < 0.8 :
              freezeval = histo.GetBinContent(xbin,ybin)
              ForwardHist.SetBinContent(xbin,ybin,0.0)
              CentralHist.SetBinContent(xbin,ybin,freezeval)
            else :
              ForwardHist.SetBinContent(xbin,ybin,freezeval)
              CentralHist.SetBinContent(xbin,ybin,0.0)
      
        # Now do symmetrization for these hists and write to file.
        NewCentral = ProviderHistoHelpers.ExtendPtRangeOfHisto(CentralHist,systematicName+"holder")
        CentralSym = ProviderHistoHelpers.SymmetrizeHistoInEta(NewCentral,SystematicNameDictionary[histName]+"_Central_"+jetDef)
        CentralSym.SetDirectory(0)
        histos[jetDef][SystematicNameDictionary[histName]+"_Central"] = CentralSym
        NewForward = ProviderHistoHelpers.ExtendPtRangeOfHisto(ForwardHist,systematicName+"holder")
        ForwardSym = ProviderHistoHelpers.SymmetrizeHistoInEta(NewForward,SystematicNameDictionary[histName]+"_Forward_"+jetDef)
        ForwardSym.SetDirectory(0)
        histos[jetDef][SystematicNameDictionary[histName]+"_Forward"] = ForwardSym

      # If this is the early data histogram, we need to deactivate it in the central region for 25 ns data.
      if 'EarlyData' in histName :
        ForwardHist = histo.Clone()
        ForwardHist.SetName("EarlyData_Forward_holder")
        # Loop over pT bins.
        for xbin in range(histo.GetNbinsX()+2) :
          # Loop out in eta.
          for ybin in range(histo.GetNbinsY()+2) :
            #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
            # Store bin contents as we go out: last one is one we want as frozen value.
            eta = abs(histo.GetYaxis().GetBinCenter(ybin))
            if eta < 0.8 :
              ForwardHist.SetBinContent(xbin,ybin,0.0)
      
        # Now do symmetrization and write to file.
        NewForward = ProviderHistoHelpers.ExtendPtRangeOfHisto(ForwardHist,systematicName+"holder")
        ForwardSym = ProviderHistoHelpers.SymmetrizeHistoInEta(NewForward,SystematicNameDictionary[histName]+"_Forward_"+jetDef)
        ForwardSym.SetDirectory(0)
        histos[jetDef][SystematicNameDictionary[histName]+"_Forward"] = ForwardSym
        histos[jetDef][SystematicNameDictionary[histName]] = histoSym

      else : 
        histos[jetDef][SystematicNameDictionary[histName]] = histoSym

    # Done reading, close the file
    inFile.Close()
   
  return histos

def ReadCrossCalValidityHistograms(dirName):
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
        for histName in inFile.GetKeyNames():
          if 'valid' not in histName :
            continue
          dictkey = histName.replace("valid_","") 
          systematicName = "valid_"+SystematicNameDictionary[dictkey]+"_"+jetDef
          histo = inFile.Get(histName)
          if histo is None:
            print "Failed to get histogram:",systematicName
            return None
          histo.SetName(systematicName+"_old")
            
          # Histo has different range, extend it
          histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,"valid_"+systematicName)
          histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histoNew,"valid_"+systematicName)
          histoSym.SetDirectory(0)
          histos[jetDef][SystematicNameDictionary[dictkey]] = histoSym

        # Done reading, close the file
        inFile.Close()
    
    return histos
