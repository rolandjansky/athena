#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Script to browse the unmerged HIST files and extract LBs for which at least N occurences of an object is found
# at a position foundas noisy
# Uses the pathExtract library to extract the EOS path
# See the twiki: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/UsingDQInframerge
#Options:
#  -h, --help            show this help message and exit
#  -r RUN, --run=RUN     Run number
#  -s STREAMS, --stream=STREAMS Data stream : express/CosmicCalo/JetTauEtmiss/Egamma
#  -a AMI, --amiTag=AMI  ami Tag - Simply set x/f to choose express/bulk processing
#  -e ETA, --eta=ETA     Eta of hot spot
#  -p PHI, --phi=PHI     Phi of hot spot (or MET bump)
#  -c THRESHOLD, --treshold=THRESHOLD Et/pt threshold (in MeV)
#  -d DELTA, --delta=DELTA Distance to look around hot spot (or MET bump)
#  -o OBJECT, --object=OBJECT TopoCluster/Jet
#  -m MIN, --min=MIN     Min number of object in a LB
#  -n, --noplot          Do not plot LB map
# Author : Benjamin Trocme (LPSC Grenoble) / Winter 2015-Spring 2016


import os, sys  
import string
from optparse import OptionParser
import argparse

import pathExtract         

import ROOT
from ROOT import *
from ROOT import gROOT, gDirectory
from ROOT import gStyle, TCanvas, TString
from ROOT import TFile, TTree
from ROOT import TH1F,TH2F,TBrowser
from ROOT import TPaveText

gROOT.Reset()
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
parser = argparse.ArgumentParser()
parser.add_argument('-r','--run',type=int,dest='runNumber',default='267599',help="Run number",action='store')
parser.add_argument('-ll','--lowerlb',type=int,dest='lowerLB',default='0',help="Lower lb",action='store')
parser.add_argument('-ul','--upperlb',type=int,dest='upperLB',default='999999',help="Upper lb",action='store')
parser.add_argument('-s','--stream',dest='stream',default='express',help="Stream without prefix: express, CosmicCalo, Egamma...",action='store')
parser.add_argument('-t','--tag',dest='tag',default='data16_13TeV',help="DAQ tag: data12_8TeV, data12_calocomm...",action='store')
parser.add_argument('-a','--amiTag',dest='amiTag',default='x',help="First letter of AMI tag: x->express / f->bulk",action='store')
parser.add_argument('-e','--eta',type=float,dest='etaSpot',default='0',help='Eta of hot spot',action='store')
parser.add_argument('-p','--phi',type=float,dest='phiSpot',default='0.',help='Phi of hot spot (or MET bump)',action='store')
parser.add_argument('-d','--delta',type=float,dest='deltaSpot',default='0.1',help='Distance to look around hot spot (or MET bump)',action='store')
parser.add_argument('-o','--object',dest='objectType',default='TopoCluster',help='TopoCluster,JetsHI',action='store')
#parser.add_argument('-c','--cut',type=int,dest='upperLB',default='999999',help="Upper lb",action='store')
parser.add_argument('-m','--min',type=int,dest='minInLB',default='5',help='Min number of object in a LB',action='store')
parser.add_argument('-n','--noplot',dest='noplot',help='Do not plot LB map',action='store_true')


args = parser.parse_args()

parser.print_help()

run = args.runNumber
lowerLumiBlock = args.lowerLB
upperLumiBlock = args.upperLB
stream = args.stream
tag = args.tag
amiTag = args.amiTag
etaSpot = args.etaSpot
phiSpot = args.phiSpot
deltaSpot = args.deltaSpot
objectType = args.objectType
minInLB = args.minInLB

# Histo path definition base on object type
if (objectType == "TopoCluster"):
  histoPath  = {"Et10GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh1"%(run),
                "Et25GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh2"%(run),
                "Et50GeV":"run_%d/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh3"%(run)}
  histoLegend = {"Et10GeV":"Et > 10GeV",
                 "Et25GeV":"Et > 25GeV",
                 "Et50GeV":"Et > 50GeV"}
  histoColor = {"Et10GeV":kBlack,
                "Et25GeV":kRed,
                "Et50GeV":kBlue}
  histoKeys = ["Et10GeV",
               "Et25GeV",
               "Et50GeV"]
  b_2dHisto = True # Draw with "COLZ"

if (objectType == "EMTopoJets"):
  histoPath  = {"noCut":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhi"%(run),
                "cut1":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_20000_inf_pt_inf_500000"%(run),
                "cut2":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_500000_inf_pt_inf_1000000"%(run),
                "cut3":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_1000000_inf_pt_inf_2000000"%(run),
                "cut4":"run_%d/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_2000000_inf_pt_inf_8000000"%(run)}
  histoLegend = {"noCut":"No cut",
                 "cut1":"20GeV-500GeV",
                 "cut2":"500GeV-1TeV",
                 "cut3":"1TeV-2TeV",
                 "cut4":"2TeV-8TeV"}
  histoColor = {"noCut":kBlack,
                "cut1":kRed,
                "cut2":kBlue,
                "cut3":kGreen,
                "cut4":kPink}
  histoKeys = ["noCut",
               "cut1",
               "cut2",
               "cut3",
               "cut4"]
  b_2dHisto = True # Draw with "COLZ"

# Look for the final merged HIST file
# and plot the histogram
runFilePath = "root://eosatlas.cern.ch/%s"%(pathExtract.returnEosHistPath(run,stream,amiTag,tag)).rstrip()
if ("FILE NOT FOUND" in runFilePath):
  print "No merged file found..."
  sys.exit()

f = TFile.Open(runFilePath)
histo = {}
for iHisto in histoKeys:
  histo[iHisto] = f.Get(histoPath[iHisto])

if (b_2dHisto):
  gStyle.SetPalette(1)
  gStyle.SetOptStat("")
  drawOption="COLZ"
else:
  drawOption=""

c = {}
for iHisto in histoKeys:
  c[iHisto] = TCanvas(histoLegend[iHisto])
  histo[iHisto].Draw(drawOption)

# Extract the bin number of the noisy region. the position should be precisely defined (facility of
# using a delta around the position not yet implemented)
# Assume that eta is on x axis
#hotSpotBin = histo[iHisto].FindBin(etaSpot,phiSpot)
# Scans the window to find all bins that fall in the window
regionBins = []
nSteps = 1000
subStep = 2*deltaSpot/nSteps
for ix in range(nSteps):
  iEta = etaSpot - deltaSpot + ix * subStep
  for iy in range (nSteps):
    iPhi = phiSpot - deltaSpot + iy * subStep
    tmpBin = histo[iHisto].FindBin(iEta,iPhi)
    if (tmpBin not in regionBins):
      regionBins.append(tmpBin)

# Extract all the unmerged files available withthe LB range
lbFilePathList = pathExtract.returnEosHistPathLB(args.runNumber,lowerLumiBlock,upperLumiBlock,args.stream,args.amiTag,args.tag)
nbHitInHot = []

nLB=2500
nbHitInHot = {}
for iHisto in histoKeys:
  nbHitInHot[iHisto] = [0] * nLB
lowerLB = 2500
upperLB = 0
lbCanvas = []
histoLBNoisy = []
fLB = {}

print "I have found the merged HIST file %s"%(runFilePath)
print "I have found %d unmerged HIST files"%(len(lbFilePathList))
print "The first one is root://eosatlas.cern.ch/%s"%(lbFilePathList[0])
sys.stdout.write("Start scanning the LBs:")

# Loop on all unmerged files
for lbFile in lbFilePathList:
  lbFilePath = "root://eosatlas.cern.ch/%s"%(lbFile).rstrip()
  ilb = int((lbFile.split("_lb")[1]).split("._SFO")[0])
  sys.stdout.write("%d "%(ilb))
  sys.stdout.flush()
  fLB[lbFile] = TFile.Open(lbFilePath)
  histoLB = {}
  for iHisto in histoKeys:
    histoLB[iHisto] = fLB[lbFile].Get(histoPath[iHisto])
    for iBin in regionBins:
      nbHitInHot[iHisto][ilb] = nbHitInHot[iHisto][ilb] + histoLB[iHisto].GetBinContent(iBin)
#  sys.stdout.write("->%d "%(nbHitInHot[ilb]))

  fLB[lbFile].Close()

for iHisto in histoKeys:
  for ilb in range(len(nbHitInHot[iHisto])):
    if (nbHitInHot[iHisto][ilb]>=minInLB):
      if ilb<lowerLB : lowerLB = ilb
      if ilb>upperLB : upperLB = ilb  

print ""
print "I have looked for LBs with at least %d entries at position (%.2f,%.2f) in %s histogram"%(minInLB,etaSpot,phiSpot,objectType)

maxNbInHot = 0
totalInRegionRecomp = {} 
totalInRegion = {} 

for iHisto in histoKeys:
  totalInRegionRecomp[iHisto] = 0
  totalInRegion[iHisto] = 0

for iHisto in histoKeys:
  print "======= ",histoLegend[iHisto]
  for iBin in regionBins:
    totalInRegion[iHisto] = totalInRegion[iHisto] + histo[iHisto].GetBinContent(iBin)
  for i in range(nLB):
    totalInRegionRecomp[iHisto] = totalInRegionRecomp[iHisto] + nbHitInHot[iHisto][i]
    if (nbHitInHot[iHisto][i]>=minInLB):
      print "LB: %d -> %d hits"%(i,nbHitInHot[iHisto][i])
    if (nbHitInHot[iHisto][i]>maxNbInHot):
      maxNbInHot = nbHitInHot[iHisto][i]

  print "In the whole run, there are %d entries"%(totalInRegion[iHisto])
  if (totalInRegionRecomp[iHisto] != totalInRegion[iHisto]):
    print "To be compared with %d entries cumulated from unmerged files"%(totalInRegionRecomp[iHisto])
    if (totalInRegionRecomp[iHisto] < totalInRegion[iHisto]):
      print "This is normal only if you restricted the LB range..."
    if (totalInRegionRecomp[iHisto] > totalInRegion[iHisto]):
      print "This can be also caused by multiple processing, try to filter with -a option"
      print "File path of the first file:",lbFilePathList[0]

## Plot evolution vs LB
leg = TLegend(0.5,0.75,0.95,0.92)

if (upperLB>=lowerLB): # check that at least one noisy LB was found
  c0 = TCanvas()
  h0Evol = {}
  first = True
  for iHisto in histoKeys:
    h0Evol[iHisto] = TH1I("h0Evol%s"%(iHisto),"Nb of hits in a region of %.2f around the position (%.2f,%.2f) - %s"%(deltaSpot,etaSpot,phiSpot,objectType),upperLB-lowerLB+1,lowerLB-0.5,upperLB+0.5)
    h0Evol[iHisto].SetXTitle("LumiBlock (Only LB with >= %d entries)"%(minInLB))
    h0Evol[iHisto].SetLineColor(histoColor[iHisto])
    leg.AddEntry(h0Evol[iHisto],"%s (%d entries in the run)"%(histoLegend[iHisto],totalInRegion[iHisto]))
    for i in range(lowerLB,upperLB+1):
      h0Evol[iHisto].Fill(i,nbHitInHot[iHisto][i])  
    if first:
      h0Evol[iHisto].Draw()
      h0Evol[iHisto].SetMinimum(minInLB-1)
      h0Evol[iHisto].SetMaximum(maxNbInHot*1.25)
      first = False
    else:
      h0Evol[iHisto].Draw("SAME")
  leg.Draw()
  c0.Update()
