# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Following in-situ components need to be combined in quadrature to make the histogram from which to subtract

from ParseInsituInput import ReadInSituHistograms

InSituCompDictionary = ReadInSituHistograms("/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/inputs/Moriond2016/AllInSitu/")
SingleParticleFile = TFile("/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/inputs/Moriond2016/HighPt2012/AggressiveHighPt_Final2014.root")

jetDefDict =    {
                    'EMJES_R4'  : 'AntiKt4Topo_EMJES',
#                    'EMJES_R6'  : 'AntiKt6Topo_EMJES',
#                    'LCJES_R4'  : 'AntiKt4Topo_LCJES',
#                    'LCJES_R6'  : 'AntiKt6Topo_LCJES'
                }

outputFile = TFile("2012HighPtTerm_2015adjusted.root","RECREATE")

for readJetDef in jetDefDict :

  jetDef = jetDefDict[readJetDef]

  inSituHist = InSituCompDictionary[jetDef]['InSituProp_PunchThroughMC'].Clone("InSitu_Quad") 
  inSituHist.Reset()
  inSituHist.SetDirectory(0)

  # Fill inSituHist with sum of squares of components.
  for histkey in InSituCompDictionary[jetDef] :

    hist = InSituCompDictionary[jetDef][histkey]

    for bin in range(0, hist.GetNbinsX()+2) :
      assert(inSituHist.GetBinLowEdge(bin) == hist.GetBinLowEdge(bin) and inSituHist.GetBinWidth(bin) == hist.GetBinWidth(bin))

      inSituHist.SetBinContent(bin,inSituHist.GetBinContent(bin)+math.pow(hist.GetBinContent(bin),2))

  # This is now (total in situ)^2

  # Fetch single particle hist
  spHist = SingleParticleFile.Get("SingleParticle_HighPt_{0}".format(jetDef))

  outputHist = spHist.Clone("adjustedInterpolatedSingleParticleUncertainty")
  outputHist.SetDirectory(0)
  outputHist.Reset()

  # Loop over in highpT bins and get an interpolated SP value in each;
  # calculate difference and fill result
  for bin in range(0,spHist.GetNbinsX()+2) :

    pT = spHist.GetXaxis().GetBinCenter(bin)
    inSituBin = inSituHist.FindBin(pT)
    if inSituBin > inSituHist.GetNbinsX() : inSituBin = inSituHist.GetNbinsX()
    inSituVal2 = inSituHist.GetBinContent(inSituBin)
    spVal = spHist.Interpolate(pT)

    diff = spVal*spVal - inSituVal2

    if diff < 0 :
      keepSPVal = 0
    else :
      keepSPVal = math.sqrt(spVal*spVal - inSituVal2)

    if pT < 2000 :
      smoothedSPVal = 0
    elif pT < 2200 :
      smoothedSPVal = keepSPVal * ((pT - 2000)/200)
    else :
      smoothedSPVal = keepSPVal

    outputHist.SetBinContent(bin,smoothedSPVal)    

  outputFile.cd()
  outputHist.Write("SingleParticle_HighPt_{0}".format(jetDef))

outputFile.Close()

