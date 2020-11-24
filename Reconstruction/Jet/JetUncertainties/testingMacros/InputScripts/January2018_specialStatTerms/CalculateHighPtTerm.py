from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Following in-situ components need to be combined in quadrature to make the histogram from which to subtract

from ParseInsituInput import ReadInSituHistograms

InSituCompDictionary = ReadInSituHistograms("/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/inputs/Fall2018/AllInSitu/")
SingleParticleFile = TFile("/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/inputs/Fall2018/HighPt/Release20p7_SingleParticle_FullRange.root")


jetDefList =    [
                    'AntiKt4Topo_EMJES',
                    'AntiKt4PFlow_EMJES',
                ]

outputFile = TFile("Release20p7_HighPtTerm_Fall2018adjusted.root","RECREATE")

for jetDef in jetDefList :

  print InSituCompDictionary.keys()
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
  spHist = SingleParticleFile.Get("totalHist")

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

    if pT < 2200 :
      smoothedSPVal = 0
    elif pT < 2400 :
      smoothedSPVal = keepSPVal * ((pT - 2200)/200)
    else :
      smoothedSPVal = keepSPVal

    outputHist.SetBinContent(bin,smoothedSPVal)    

  outputFile.cd()
  outputHist.Write("SingleParticle_HighPt_{0}".format(jetDef))

outputFile.Close()

