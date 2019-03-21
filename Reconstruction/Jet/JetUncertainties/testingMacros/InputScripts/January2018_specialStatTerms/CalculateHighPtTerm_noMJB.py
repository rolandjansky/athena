from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Following in-situ components need to be combined in quadrature to make the histogram from which to subtract

from ParseInsituInput_MJB import ReadInSituHistograms

InSituCompDictionary = ReadInSituHistograms("/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/inputs/Moriond2018/forMJBInputs_GammaZJet")
SingleParticleFile = TFile("/afs/cern.ch/work/k/kpachal/JetUncertainties/JetUncertainties/athena/Reconstruction/Jet/JetUncertainties/inputs/Moriond2018/HighPt2012/AggressiveHighPt_Final2014.root")

jetDefList =    [ 'AntiKt4Topo_EMJES',
                  'AntiKt4PFlow_EMJES',
                ]

outputFile = TFile("2012HighPtTerm_2017Moriond2018adjusted.root","RECREATE")

for jetDef in jetDefList :

  inSituHist = InSituCompDictionary[jetDef]['Zjet_dPhi'].Clone("InSitu_Quad") 
  inSituHist.Reset()
  inSituHist.SetDirectory(0)

  # Fill inSituHist with sum of squares of components.
  for histkey in InSituCompDictionary[jetDef] :

    hist = InSituCompDictionary[jetDef][histkey]

    for bin in range(1, hist.GetNbinsX()+1) :
      #print "Comparing Zjet_dPhi with",histkey,":",inSituHist.GetBinLowEdge(bin),"==",hist.GetBinLowEdge(bin),"and",inSituHist.GetBinWidth(bin),"==",hist.GetBinWidth(bin)
      assert(inSituHist.GetBinLowEdge(bin) == hist.GetBinLowEdge(bin) and inSituHist.GetBinWidth(bin) == hist.GetBinWidth(bin))

      inSituHist.SetBinContent(bin,inSituHist.GetBinContent(bin)+math.pow(hist.GetBinContent(bin),2))

  # This is now (total in situ)^2

  # Fetch single particle hist
  # For PFlow, we are using EM hist
  spHist = SingleParticleFile.Get("SingleParticle_HighPt_AntiKt4Topo_EMJES")
  print "Setting all to",spHist.GetBinContent(spHist.GetNbinsX())
  for bin in range(1,spHist.GetNbinsX()+1) :
    spHist.SetBinContent(bin,spHist.GetBinContent(spHist.GetNbinsX()))

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

    if pT < 800 :
      smoothedSPVal = 0
    elif pT < 1000 :
      smoothedSPVal = keepSPVal * ((pT - 800)/200)
    else :
      smoothedSPVal = keepSPVal

    outputHist.SetBinContent(bin,smoothedSPVal)    

  outputFile.cd()
  outputHist.Write("SingleParticle_HighPt_{0}".format(jetDef))

outputFile.Close()

