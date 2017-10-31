#!/usr/bin env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Script to browse the unmerged HIST files and extract LBs for which at least N occurences of an object is found
# at a position foundas noisy
# Uses the pathExtract library to extract the EOS path
# See the twiki: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/UsingDQInframerge
#Options:
#  -r ARG1, --run ARG1   Run number
#  -ll ARG2, --lowerlb ARG2
#                        Lower lb
#  -ul ARG3, --upperlb ARG3
#                        Upper lb
#  -s ARG4, --stream ARG4
#                        Stream without prefix: express/CosmicCalo/Main/ZeroBias/MinBias
#  -t ARG5, --tag ARG5   DAQ tag: data16_13TeV, data16_cos...
#  -a ARG6, --amiTag ARG6
#                        First letter of AMI tag: x->express / f->bulk
#  -e ARG7, --eta ARG7   Eta of hot spot
#  -p ARG8, --phi ARG8   Phi of hot spot
#  -ia ARG9, --integralAbove ARG9
#                        Lower bound of integral
#  -d ARG10, --delta ARG10
#                        Distance to look around hot spot
#  -o ARG11, --object ARG11
#                        2D OCCUPANCY: TopoClusters,EMTopoClusters,
#                                      EMTopoJets,TightFwdElectrons 
#                        1D OCCUPANCY: EMTopoJets_eta 
#                        INTEGRAL    : NumberTau,NumberTightElectrons,NumberHLTJet
#  -m ARG12, --min ARG12
#                        Min number of occurences in a LB
#  -g, --grl             Look for Calo/LAr/Tile defects set in suspicious LBs
# Author : Benjamin Trocme (LPSC Grenoble) / 2015-2016

import os, sys  
import string
import argparse

import pathExtract         

import ROOT
from ROOT import *

# Line below commented to work with release 21. 
# Not sure what was its purpose...
#gROOT.Reset()
gStyle.SetPalette(1)
gStyle.SetOptStat("em")

def lbStr(lb):
  if (lb<10):
    lbString = "_lb000%d"%(lb)
  elif (lb<100):
    lbString = "_lb00%d"%(lb)
  elif  (lb<1000):
    lbString = "_lb0%d"%(lb)
  else:
    lbString = "_lb%d"%(lb)
  return lbString
  

# Main===========================================================================================================
parser = argparse.ArgumentParser(description='Process some integers.',formatter_class=RawTextHelpFormatter)
parser.add_argument('-r','--run',type=int,dest='arg1',default='267599',help="Run number",action='store')
parser.add_argument('-ll','--lowerlb',type=int,dest='arg2',default='0',help="Lower lb",action='store')
parser.add_argument('-ul','--upperlb',type=int,dest='arg3',default='999999',help="Upper lb",action='store')
parser.add_argument('-s','--stream',dest='arg4',default='Main',help="Stream without prefix: express/CosmicCalo/Main/ZeroBias/MinBias",action='store')
parser.add_argument('-t','--tag',dest='arg5',default='data17_13TeV',help="DAQ tag: data16_13TeV, data16_cos...",action='store')
parser.add_argument('-a','--amiTag',dest='arg6',default='f',help="First letter of AMI tag: x->express / f->bulk",action='store')
parser.add_argument('-e','--eta',type=float,dest='arg7',default='-999.',help='Eta of hot spot',action='store')
parser.add_argument('-p','--phi',type=float,dest='arg8',default='-999.',help='Phi of hot spot',action='store')
parser.add_argument('-x','--x',type=float,dest='arg71',default='-999.',help='X of hot spot',action='store')
parser.add_argument('-y','--y',type=float,dest='arg81',default='-999.',help='Y of hot spot',action='store')
parser.add_argument('-ia','--integralAbove',type=float,dest='arg9',default='-999.',help='Lower bound of integral',action='store')
parser.add_argument('-d','--delta',type=float,dest='arg10',default='0.1',help='Distance to look around hot spot',action='store')
parser.add_argument('-o','--object',dest='arg11',default='TopoClusters',help='2D OCCUPANCY: TopoClusters,EMTopoClusters,\n              EMTopoJets,TightFwdElectrons,Tau \n1D OCCUPANCY: EMTopoJets_eta,Tau_eta,Tau_phi \nINTEGRAL    : NumberTau,NumberTightFwdElectrons,NumberHLTJet',action='store')
parser.add_argument('-m','--min',type=float,dest='arg12',default='5',help='Min number of occurences in a LB',action='store')
parser.add_argument('-g','--grl',dest='arg13',help='Look for Calo/LAr/Tile defects set in suspicious LBs',action='store_true')

args = parser.parse_args()

parser.print_help()

runNumber = args.arg1
lowerLumiBlock = args.arg2
upperLumiBlock = args.arg3
stream = args.arg4
tag = args.arg5
amiTag = args.arg6
etaSpot = args.arg7
phiSpot = args.arg8
xSpot = args.arg71
ySpot = args.arg81
integralAbove = args.arg9
deltaSpot = args.arg10
objectType = args.arg11
minInLB = args.arg12
defectQuery = args.arg13

#########################################################################
# Histo path definition base on object type
color0 = kBlue+1
color1 = kGreen+1
color2 = kOrange+7
color3 = kMagenta+2
color4 = kCyan-3

# TopoClusters and EMTopoClusters
if (objectType == "TopoClusters"):
  # Histogram path in the ROOT file
  histoPath  = {"Et10GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh1"%(runNumber),
                "Et25GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh2"%(runNumber),
                "Et50GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh3"%(runNumber)}
  # Histogram legend
  histoLegend = {"Et10GeV":"Et > 10GeV",
                 "Et25GeV":"Et > 25GeV",
                 "Et50GeV":"Et > 50GeV"}
  # Colors for the final summary plot
  histoColor = {"Et10GeV":color0,
                "Et25GeV":color1,
                "Et50GeV":color2}
  # Keys of histogram to be considered. Can be a subset of histoPath.keys()
  histoKeys = ["Et10GeV",
               "Et25GeV",
               "Et50GeV"]
  # Types of plot
  # 2d_etaPhiHotSpot   : 2D occupancy plots: (eta/phi) required
  # 2d_xyHotSpot       : any 2D plots: (x/y) required
  # 1d_etaHotSpot      : 1D occupancy plot along eta: (eta) required
  # 1d_phiHotSpot      : 1D occupancy plot along phi: (phi) required
  # 1d_integralAbove   : integral between (integralAbove) and infinity. (integralAbove) required
  histoType = "2d_etaPhiHotSpot"
  # Name of object
  histoName = "TopoClusters"
if (objectType == "EMTopoClusters"):
  histoPath  = {"Et4GeV":"run_%d/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh1"%(runNumber),
                "Et10GeV":"run_%d/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh2"%(runNumber),
                "Et25GeV":"run_%d/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh3"%(runNumber)}
  histoLegend = {"Et4GeV":"Et > 4GeV",
                 "Et10GeV":"Et > 10GeV",
                 "Et25GeV":"Et > 25GeV"}
  histoColor = {"Et4GeV":color0,
                "Et10GeV":color1,
                "Et25GeV":color2}
  histoKeys = ["Et4GeV",
               "Et10GeV",
               "Et25GeV"]
  histoType = "2d_etaPhiHotSpot"
  histoName = "EMTopoClusters"
# EMTopojets
if (objectType == "EMTopoJets"):
  histoPath  = {"noCut":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhi"%(runNumber),
                "cut1":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_20000_inf_pt_inf_500000"%(runNumber),
                "cut2":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_500000_inf_pt_inf_1000000"%(runNumber),
                "cut3":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_1000000_inf_pt_inf_2000000"%(runNumber),
                "cut4":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_2000000_inf_pt_inf_8000000"%(runNumber)}
  histoLegend = {"noCut":"No cut",
                 "cut1":"20GeV-500GeV",
                 "cut2":"500GeV-1TeV",
                 "cut3":"1TeV-2TeV",
                 "cut4":"2TeV-8TeV"}
  histoColor = {"noCut":color0,
                "cut1":color1,
                "cut2":color2,
                "cut3":color3,
                "cut4":color4}
  histoKeys = ["noCut",
               "cut1",
               "cut2",
               "cut3",
               "cut4"]
  histoType = "2d_etaPhiHotSpot"
  histoName = "EMTopoJets"
if (objectType == "EMTopoJets_eta"):
  histoPath  = {"cut1":"run_%d/Jets/AntiKt4EMTopoJets/etasel_20000_inf_pt_inf_500000"%(runNumber),
                "cut2":"run_%d/Jets/AntiKt4EMTopoJets/etasel_500000_inf_pt_inf_1000000"%(runNumber),
                "cut3":"run_%d/Jets/AntiKt4EMTopoJets/etasel_1000000_inf_pt_inf_2000000"%(runNumber),
                "cut4":"run_%d/Jets/AntiKt4EMTopoJets/etasel_2000000_inf_pt_inf_8000000"%(runNumber)}
  histoLegend = {"cut1":"20GeV-500GeV",
                 "cut2":"500GeV-1TeV",
                 "cut3":"1TeV-2TeV",
                 "cut4":"2TeV-8TeV"}
  histoColor = {"cut1":color0,
                "cut2":color1,
                "cut3":color2,
                "cut4":color3}
  histoKeys = ["cut1",
               "cut2",
               "cut3",
               "cut4"]
  histoType = "1d_etaHotSpot"
  histoName = "EMTopoJets"
# Tau
if (objectType == "Tau"):
  histoPath  = {"NoCut":"run_%d/Tau/tauPhiVsEta"%(runNumber),
                "Et15GeV":"run_%d/Tau/tauPhiVsEta_et15"%(runNumber),
                "Et15GeVBdtLoose":"run_%d/Tau/tauPhiVsEta_et15_BDTLoose"%(runNumber)}
  histoLegend = {"NoCut":"Et > 4GeV",
                 "Et15GeV":"Et > 10GeV",
                 "Et15GeVBdtLoose":"Et > 15GeV-BDT loose"}
  histoColor = {"NoCut":color0,
                "Et15GeV":color1,
                "Et15GeVBdtLoose":color2}
  histoKeys = ["NoCut",
               "Et15GeV",
               "Et15GeVBdtLoose"]
  histoType = "2d_etaPhiHotSpot"
  histoName = "Tau"
if (objectType == "Tau_phi"):
  histoPath  = {"single":"run_%d/Tau/tauPhi"%(runNumber)}
  histoLegend = {"single":"All candidates"}
  histoColor = {"single":color0}
  histoKeys = ["single"]
  histoType = "1d_phiHotSpot"
  histoName = "Tau"
if (objectType == "Tau_eta"):
  histoPath  = {"single":"run_%d/Tau/tauEta"%(runNumber)}
  histoLegend = {"single":"All candidates"}
  histoColor = {"single":color0}
  histoKeys = ["single"]
  histoType = "1d_etaHotSpot"
  histoName = "Tau"
if (objectType == "NumberTau"):
  histoPath  = {"allTau":"run_%d/Tau/nTauCandidates"%(runNumber),
                "highPt":"run_%d/Tau/nHighPtTauCandidates"%(runNumber)}
  histoLegend = {"allTau":"All candidates",
                 "highPt":"High Pt (>100GeV) candidates"}
  histoColor = {"allTau":color1,
                "highPt":color2}
  histoKeys = ["allTau",
               "highPt"]
  histoType = "1d_integralAbove"
  histoName = "Number of Tau candidates"
# Electrons
if (objectType == "TightFwdElectrons"):
  histoPath  = {"single":"run_%d/egamma/forwardElectrons/forwardElectronTightEtaPhi"%(runNumber)}
  histoLegend = {"single":"10GeV"}
  histoColor = {"single":color0}
  histoKeys = ["single"]
  histoType = "2d_etaPhiHotSpot"
  histoName = "Tight electrons"
if (objectType == "NumberTightFwdElectrons"):
  histoPath  = {"single":"run_%d/egamma/forwardElectrons/forwardElectronTightN"%(runNumber)}
  histoLegend = {"single":"All candidates"}
  histoColor = {"single":color0}
  histoKeys = ["single"]
  histoType = "1d_integralAbove"
  histoName = "Number of tight forward electrons"
# HLT Jet
if (objectType == "NumberHLTJet"):
  histoPath  = {"HLTJet":"run_%d/HLT/JetMon/HLT/10j40_L14J20/HLTJet_n"%(runNumber)}
  histoLegend = {"HLTJet":"All candidates"}
  histoColor = {"HLTJet":color1}
  histoKeys = ["HLTJet"]
  histoType = "1d_integralAbove"
  histoName = "Number of HLT jets - 10J40_L14J20 trigger"
# LAr digits
if (objectType == "LArDigits"):
  histoPath  = {"Null-EMBA":"run_%d/LAr/Digits/Barrel/NullDigitChan_BarrelA"%(runNumber),
                "Satu-EMBA":"run_%d/LAr/Digits/Barrel/SaturationChan_BarrelA"%(runNumber),
                "Null-EMBC":"run_%d/LAr/Digits/Barrel/NullDigitChan_BarrelC"%(runNumber),
                "Satu-EMBC":"run_%d/LAr/Digits/Barrel/SaturationChan_BarrelC"%(runNumber),
                }
  histoLegend = {"Null-EMBA":"Null digit - EMBA",
                 "Satu-EMBA":"Saturated digit - EMBA",
                 "Null-EMBC":"Null digit - EMBC",
                 "Satu-EMBC":"Saturated digit - EMBC",}
  histoColor = {"Null-EMBA":color0,
                "Satu-EMBA":color1,
                "Null-EMBC":color2,
                "Satu-EMBC":color3}
  histoKeys = ["Null-EMBA",
               "Satu-EMBA",
               "Null-EMBC",
               "Satu-EMBC"]
  histoType = "2d_xyHotSpot"
  histoName = "LAr saturated/null digits"


# Depending of the histo/check type, define the summary title and
# check that the position of the "hot spot" (or lower bound of the integral) is defined
b_wholeHisto = False
b_ValueNotEntries = False

if histoType == "2d_etaPhiHotSpot":
  summaryTitle = "Nb of hits in a region of %.2f around the position (%.2f,%.2f) - %s"%(deltaSpot,etaSpot,phiSpot,histoName)
  statement = "I have looked for LBs with at least %.0f entries at position (%.2f,%.2f) in %s histogram"%(minInLB,etaSpot,phiSpot,histoName)
  if (etaSpot==-999. or phiSpot==-999.):
    print "No eta/phi defined -> whole histogram considered!"
    b_wholeHisto = True
if histoType == "2d_xyHotSpot":
  b_ValueNotEntries = True
  if (deltaSpot != 0):
    print "Warning: you have been summing over several bins a variable that may be not summable (different from summing hits!)"
  summaryTitle = "Value in a region of %.2f around the position (%.2f,%.2f) - %s"%(deltaSpot,xSpot,ySpot,histoName)
  statement = "I have looked for LBs with at least variable > %.2f at position (%.2f,%.2f) in %s histogram"%(minInLB,xSpot,ySpot,histoName)
  if (xSpot==-999. or ySpot==-999.):
    print "No x/y defined -> whole histogram considered!"
    print "Warning: you have been summing over several bins a variable that may be not summable (different from summing hits!)"
    b_wholeHisto = True
elif histoType == "1d_etaHotSpot":
  summaryTitle = "Nb of hits in a region of %.2f around the eta position %.2f - %s"%(deltaSpot,etaSpot,histoName)
  statement = "I have looked for LBs with at least %.0f entries at eta position %.2f in %s histogram"%(minInLB,etaSpot,histoName)
  if (etaSpot==-999.):
    print "No eta/phi -> whole histogram considered!"
    b_wholeHisto = True
elif histoType == "1d_phiHotSpot":
  summaryTitle = "Nb of hits in a region of %.2f around the phi position %.2f - %s"%(deltaSpot,phiSpot,histoName)
  statement = "I have looked for LBs with at least %.0f entries at phi position %.2f in %s histogram"%(minInLB,phiSpot,histoName)
  if (phiSpot==-999.):
    print "No eta/phi defined -> whole histogram considered!"
    b_wholeHisto = True
elif histoType == "1d_integralAbove":
  summaryTitle = "Nb of hits in the band above %.2f - %s"%(integralAbove,histoName)
  statement = "I have looked for LBs with at least %.0f entries in band above %.2f in %s histogram"%(minInLB,integralAbove,histoName)
  if (integralAbove==-999.):
    print "No lwoer bound defined -> whole histogram considered!"
    b_wholeHisto = True
#    print "You must define the lower bound of your integral"
#    sys.exit()
# Definition of Canvas option depending on histogram type
if (objectType == "NumberTightFwdElectrons" or objectType == "NumberTau"):
  canvasOption = "logy"
else:
  canvasOption = ""

#########################################################################
# Look for the final merged HIST file
# and plot the histogram
runFilePath = "root://eosatlas.cern.ch/%s"%(pathExtract.returnEosHistPath(runNumber,stream,amiTag,tag)).rstrip()
if ("FILE NOT FOUND" in runFilePath):
  print "No merged file found..."
  sys.exit()

f = TFile.Open(runFilePath)
histo = {}
for iHisto in histoKeys:
  histo[iHisto] = f.Get(histoPath[iHisto])
  histo[iHisto].SetTitle("%s - Run %d"%(histo[iHisto].GetTitle(),runNumber))

# Display the Tier0 merged histograms
c = {}
box = {}
line = {}
line2 = {}
arrow = {}
for iHisto in histoKeys:
  c[iHisto] = TCanvas(histoLegend[iHisto])
  if "logy" in canvasOption:
    c[iHisto].SetLogy(1)
  # draw line, arrows, box to highlight the suspicious region considered
  if (histoType == "2d_etaPhiHotSpot"):
    gStyle.SetPalette(1)
    gStyle.SetOptStat("")
    histo[iHisto].Draw("COLZ")
    if not b_wholeHisto:
      box[iHisto] = TBox(etaSpot-deltaSpot,phiSpot-deltaSpot,etaSpot+deltaSpot,phiSpot+deltaSpot)
      box[iHisto].SetLineColor(kRed+1)
      box[iHisto].SetLineWidth(3)
      box[iHisto].SetFillStyle(0)
      box[iHisto].Draw()
  elif (histoType == "2d_xyHotSpot"):
    gStyle.SetPalette(1)
    gStyle.SetOptStat("")
    histo[iHisto].Draw("COLZ")
    if not b_wholeHisto:
      box[iHisto] = TBox(xSpot-deltaSpot,ySpot-deltaSpot,xSpot+deltaSpot,ySpot+deltaSpot)
      box[iHisto].SetLineColor(kRed+1)
      box[iHisto].SetLineWidth(3)
      box[iHisto].SetFillStyle(0)
      box[iHisto].Draw()
  elif (histoType == "1d_etaHotSpot" or histoType == "1d_phiHotSpot"):
    minH = histo[iHisto].GetMinimum()*0.8
    maxH = histo[iHisto].GetMaximum()*1.2
    histo[iHisto].SetMinimum(minH)
    histo[iHisto].SetMaximum(maxH)
    histo[iHisto].Draw()
    if not b_wholeHisto:
      if maxH >0.:
        if histoType == "1d_etaHotSpot": 
          box[iHisto] = TBox(etaSpot-deltaSpot,minH,etaSpot+deltaSpot,maxH)
        if histoType == "1d_phiHotSpot": 
          box[iHisto] = TBox(phiSpot-deltaSpot,minH,phiSpot+deltaSpot,maxH)
        box[iHisto].SetLineColor(kRed+1)
        box[iHisto].SetLineWidth(3)
        box[iHisto].SetFillStyle(0)
        box[iHisto].Draw()
  elif (histoType == "1d_integralAbove"):
    maxH = histo[iHisto].GetMaximum()*1.2
    histo[iHisto].SetMaximum(maxH)
    histo[iHisto].Draw()
    if not b_wholeHisto:
      line[iHisto] = TLine(integralAbove,0,integralAbove,maxH)
      line[iHisto].SetLineColor(kRed+1)
      line[iHisto].SetLineWidth(3)
      line[iHisto].Draw()
      arrow[iHisto] = TArrow(integralAbove,0.2*histo[iHisto].GetMaximum(),histo[iHisto].GetBinLowEdge(histo[iHisto].GetNbinsX()),0.2*histo[iHisto].GetMaximum(),0.02,">")
      arrow[iHisto].SetLineColor(kRed+1)
      arrow[iHisto].SetLineWidth(3)
      arrow[iHisto].Draw()

#########################################################################
# Extract the list of bins where to count.
# Scans the window to find all bins that fall in the window
# The regionBins is defined for each histogram allowing different binning
regionBins = {}
for iHisto in histoKeys:
  if b_wholeHisto:
    regionBins[iHisto] = []
    if ("2d" in histoType):  
      maxBin = (histo[iHisto].GetNbinsX()+2)*(histo[iHisto].GetNbinsY()+2)
    else:
      maxBin = (histo[iHisto].GetNbinsX()+2)
    for iBin in range(maxBin):
      regionBins[iHisto].append(iBin)
  else:
    if (histoType == "2d_etaPhiHotSpot"):
      nSteps = 1000
      subStep = 2*deltaSpot/nSteps
      regionBins[iHisto] = []
      for ix in range(nSteps):# Assume that eta is on x axis
        iEta = etaSpot - deltaSpot + ix * subStep 
        for iy in range (nSteps):
          iPhi = phiSpot - deltaSpot + iy * subStep
          tmpBin = histo[iHisto].FindBin(iEta,iPhi)
          if (tmpBin not in regionBins[iHisto]):
            regionBins[iHisto].append(tmpBin)
    elif (histoType == "2d_xyHotSpot"):
      nSteps = 1000
      subStep = 2*deltaSpot/nSteps
      regionBins[iHisto] = []
      for ix in range(nSteps):
        iX = xSpot - deltaSpot + ix * subStep 
        for iy in range (nSteps):
          iY = ySpot - deltaSpot + iy * subStep
          tmpBin = histo[iHisto].FindBin(iX,iY)
          if (tmpBin not in regionBins[iHisto]):
            regionBins[iHisto].append(tmpBin)
    elif (histoType == "1d_etaHotSpot"):
      nSteps = 1000
      subStep = 2*deltaSpot/nSteps
      regionBins[iHisto] = []
      for ix in range(nSteps):
        iEta = etaSpot - deltaSpot + ix * subStep
        tmpBin = histo[iHisto].FindBin(iEta)
        if (tmpBin not in regionBins[iHisto]):
            regionBins[iHisto].append(tmpBin)
    elif (histoType == "1d_phiHotSpot"):
      nSteps = 1000
      subStep = 2*deltaSpot/nSteps
      regionBins[iHisto] = []
      for ix in range(nSteps):
        iPhi = phiSpot - deltaSpot + ix * subStep
        tmpBin = histo[iHisto].FindBin(iPhi)
        if (tmpBin not in regionBins[iHisto]):
            regionBins[iHisto].append(tmpBin)
    elif (histoType == "1d_integralAbove"):
      regionBins[iHisto] = []
      for iBin in range(histo[iHisto].FindBin(integralAbove),histo[iHisto].GetNbinsX()):
        regionBins[iHisto].append(iBin)          

# Extract all the unmerged files available with the LB range
lbFilePathList = pathExtract.returnEosHistPathLB(runNumber,lowerLumiBlock,upperLumiBlock,stream,amiTag,tag)
nbHitInHot = []

nLB=2500
nbHitInHot = {}
for iHisto in histoKeys:
  nbHitInHot[iHisto] = [0.] * nLB
lowerLB = 2500
upperLB = 0
lbCanvas = []
histoLBNoisy = []
fLB = {}

print "I have found the merged HIST file %s"%(runFilePath)
print "I have found %d unmerged HIST files"%(len(lbFilePathList))
print "The first one is root://eosatlas.cern.ch/%s"%(lbFilePathList[0])
print "The last one is root://eosatlas.cern.ch/%s"%(lbFilePathList[len(lbFilePathList)-1])

# Loop on all unmerged files

for count,lbFile in enumerate(lbFilePathList):
  lbFilePath = "root://eosatlas.cern.ch/%s"%(lbFile).rstrip()
  # Extract lb from the filename and display it
  ilb = int((lbFile.split("_lb")[1]).split("._")[0])
  if (count%100 == 0):
    sys.stdout.write("\n I processed %d/%d files \n LBs:"%(count,len(lbFilePathList)))
  sys.stdout.write("%d "%(ilb))
  sys.stdout.flush()
  fLB[lbFile] = TFile.Open(lbFilePath)
  histoLB = {}
  for iHisto in histoKeys:
    histoLB[iHisto] = fLB[lbFile].Get(histoPath[iHisto])
    for iBin in regionBins[iHisto]:
      nbHitInHot[iHisto][ilb] = nbHitInHot[iHisto][ilb] + histoLB[iHisto].GetBinContent(iBin)
#  sys.stdout.write("->%d "%(nbHitInHot[ilb]))

  fLB[lbFile].Close()

#########################################################################
# Loop on all histos and extract the luminosity block range to be displayed
for iHisto in histoKeys:
  for ilb in range(len(nbHitInHot[iHisto])):
    if (nbHitInHot[iHisto][ilb]>=minInLB):
      if ilb<lowerLB : lowerLB = ilb
      if ilb>upperLB : upperLB = ilb  

if (lowerLB == upperLB):
  lowerLB = lowerLB - 1
  upperLB = upperLB + 4

print ""
print statement

maxNbInHot = 0
totalInRegionRecomp = {} 
totalInRegion = {} 
suspiciousLBlist = []

# Initialize the number of events in suspicious regions for both the merged
# and the remerged file. 
for iHisto in histoKeys:
  totalInRegionRecomp[iHisto] = 0
  totalInRegion[iHisto] = 0
# Then count the number of events and check if equal
# Also sort the LB to highlight most problematic LB
sortedLB = {}

for iHisto in histoKeys:
  print "======= ",histoLegend[iHisto]
  for iBin in regionBins[iHisto]:
    totalInRegion[iHisto] = totalInRegion[iHisto] + histo[iHisto].GetBinContent(iBin)
  
  sortedLB[iHisto] = [0] * nLB
  for i in range(nLB):
    totalInRegionRecomp[iHisto] = totalInRegionRecomp[iHisto] + nbHitInHot[iHisto][i]
    
    sortedLB[iHisto][i] = i
    if (nbHitInHot[iHisto][i]>=minInLB):
      suspiciousLBlist.append(i)
    if (nbHitInHot[iHisto][i]>maxNbInHot):
      maxNbInHot = nbHitInHot[iHisto][i]
      
  sortedLB[iHisto].sort(key=dict(zip(sortedLB[iHisto],nbHitInHot[iHisto])).get,reverse=True)
  for i in range(nLB):
    if nbHitInHot[iHisto][sortedLB[iHisto][i]]>=minInLB:
      if not b_ValueNotEntries:
        print "%d-LB: %d -> %d hits"%(i,sortedLB[iHisto][i],nbHitInHot[iHisto][sortedLB[iHisto][i]])
      else:
        print "%d-LB: %d -> %.2f"%(i,sortedLB[iHisto][i],nbHitInHot[iHisto][sortedLB[iHisto][i]])

  if not b_ValueNotEntries:
    print "In the whole run, there are %d entries"%(totalInRegion[iHisto])
    if (totalInRegionRecomp[iHisto] != totalInRegion[iHisto]):
      print "To be compared with %d entries cumulated from unmerged files"%(totalInRegionRecomp[iHisto])
      if (totalInRegionRecomp[iHisto] < totalInRegion[iHisto]):
        print "This is normal only if you restricted the LB range..."
      if (totalInRegionRecomp[iHisto] > totalInRegion[iHisto]):
        print "This can be also caused by multiple processing, try to filter with -a option"
        print "File path of the first file:",lbFilePathList[0]
  else:
    print "In the whole run, the value is %.2f"%(totalInRegion[iHisto])

#########################################################################
## Plot evolution vs LB
leg = TLegend(0.52,0.90-0.05*len(histoKeys),0.98,0.95)
leg.SetHeader("Run %d"%runNumber)

if (upperLB>=lowerLB): # check that at least one noisy LB was found
  c0 = TCanvas()
  gStyle.SetOptStat("")
  if histoType != "2d_xyHotSpot":
    c0.SetLogy(1)
  h0Evol = {}
  first = True
  for iHisto in histoKeys:
    h0Evol[iHisto] = TH1I("h0Evol%s"%(iHisto),summaryTitle,upperLB-lowerLB+1,lowerLB-0.5,upperLB+0.5)
    h0Evol[iHisto].SetXTitle("LumiBlock (Only LB with >= %.0f entries)"%(minInLB))
    h0Evol[iHisto].SetLineColor(histoColor[iHisto])
    h0Evol[iHisto].SetMarkerColor(histoColor[iHisto])
    h0Evol[iHisto].SetMarkerStyle(20)
    leg.AddEntry(h0Evol[iHisto],"%s (%d entries in the run)"%(histoLegend[iHisto],totalInRegion[iHisto]))
    for i in range(lowerLB,upperLB+1):
      h0Evol[iHisto].Fill(i,nbHitInHot[iHisto][i])  
    if first:
      h0Evol[iHisto].Draw("P HIST")
      if histoType != "2d_xyHotSpot":
        h0Evol[iHisto].SetMinimum(minInLB-0.8)
        h0Evol[iHisto].SetMaximum(maxNbInHot*1.5)
      first = False
    else:
      h0Evol[iHisto].Draw("PSAME HIST")
  leg.Draw()
  c0.Update()

if defectQuery:
  print "I am looking for LAr/Tile/Calo defects defined for the suspicious LB"
  from DQDefects import DefectsDB
  db = DefectsDB()
  defectList = [d for d in (db.defect_names | db.virtual_defect_names) if ((d.startswith("LAR") and "SEV" in d) or (d.startswith("TILE")) or (d.startswith("CALO")))]
  defects = db.retrieve((runNumber, 1), (runNumber+1, 0), defectList)
  for iDef in defects:
    associatedSuspicious = False
    for iLB in range(iDef.since.lumi,iDef.until.lumi):
      if iLB in suspiciousLBlist:
        associatedSuspicious = True
    if associatedSuspicious:
      if (iDef.since.lumi == iDef.until.lumi-1):
        print "%s: %d set by %s - %s"%(iDef.channel,iDef.since.lumi,iDef.user,iDef.comment)
      else:
        print "%s: %d->%d set by %s - %s"%(iDef.channel,iDef.since.lumi,iDef.until.lumi-1,iDef.user,iDef.comment)
