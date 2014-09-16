#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ======================================================================
def extractBad(histo,sp,hi,bin):

  for ix in range(1,histo.GetNbinsX()+1):
    for iy in range (1,histo.GetNbinsY()+1):
      flag = histo.GetBinContent(ix,iy)
      if (flag == 5):
        sp[returnPart(histo,ix)][bin]=sp[returnPart(histo,ix)][bin]+1
      if (flag == 6):
        hi[returnPart(histo,ix)][bin]=hi[returnPart(histo,ix)][bin]+1
  histo.Print()

  return

# ======================================================================
def returnPart(h,xBin):

  partition = 0
  
  if (h.GetNbinsX()) == 448:
    if (xBin%14!=1): #EMB
      partition = 0      
    if (xBin%14==1): #EMB PS
      partition = 1
  else:
    if (xBin>90 and xBin<=105):
      partition=3 # FCAL
    else:
      if ((xBin>49 and xBin<56) or (xBin>154 and xBin<161) or (xBin>244 and xBin<251) or (xBin>334 and xBin<341)):
        partition=4 #HEC
      else:
        partition=2 


  return partition

# ======================================================================
def returnPartName(partNb):

  partName = "Unknown"
  if partNb == 0:
    partName = "EMB"
  if partNb == 1:
    partName = "EMB PS"
  if partNb == 2:
    partName = "EMEC"
  if partNb == 3:
    partName = "FCAL"
  if partNb == 4:
    partName = "HEC"
    
  return partName


# Main =================================================================
import os, sys  
import string
from numpy import *

import ROOT
from ROOT import gROOT, gDirectory
from ROOT import gStyle, TCanvas, TString
from ROOT import TFile, TTree, TRFIOFile
from ROOT import TH1F,TH2F,TBrowser,TProfile2D,TGraph,TLegend

gROOT.Reset()
gStyle.SetPalette(1)
gStyle.SetCanvasColor(0);
gStyle.SetOptStat(100110)

tag = "UPD4" # UPD4 (used for the bulk) is the default
if len(sys.argv) == 2:
  if (sys.argv[1] == "UPD1"):
    tag = "UPD1"

# Add new run here - Do not forget the AMI tag
# You should also remove runs that are no longer staged
if tag == "UPD1":
  runNumber = array([160387,160472,160479,160530])
  tagxf = array([33,33,33,33])
  tagm = array([565,565,565,565])
if tag == "UPD4":
  runNumber = array([159224,160387,160472,160479,160530])
  tagxf = array([275,280,280,280,280])
  tagm = array([545,565,565,565,565])

nbOfRun = len(runNumber)
#nbOfRun = 2

nbOfPart=5 # 0->EMB 1->EMBPS 2->EC 3->FCAL 4->HEC

sporadic=[]
highNoise=[]

for iPart in range(0,nbOfPart):
  sporadic.append(zeros(nbOfRun,dtype='int'))
  highNoise.append(zeros(nbOfRun,dtype='int'))

spEMBPS=zeros(nbOfRun,dtype='int')
hiEMBPS=zeros(nbOfRun,dtype='int')
spEMB=zeros(nbOfRun,dtype='int')
hiEMB=zeros(nbOfRun,dtype='int')

xAxis=zeros(nbOfRun,dtype='int')

# Read all the tier0 monitoring file
# You must define STAGE_SVCCLASS=atlcal

for index in range(0,nbOfRun):
  xAxis[index]=index
  if tag == "UPD1":
    nameFile = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0%d/data10_7TeV.00%d.physics_CosmicCalo.merge.HIST.x%d_m%d/data10_7TeV.00%d.physics_CosmicCalo.merge.HIST.x%d_m%d._0001.1"%(runNumber[index],runNumber[index],tagxf[index],tagm[index],runNumber[index],tagxf[index],tagm[index])
  if tag == "UPD4":
    nameFile = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0%d/data10_7TeV.00%d.physics_CosmicCalo.merge.HIST.f%d_m%d/data10_7TeV.00%d.physics_CosmicCalo.merge.HIST.f%d_m%d._0001.1"%(runNumber[index],runNumber[index],tagxf[index],tagm[index],runNumber[index],tagxf[index],tagm[index])
  print nameFile
  myfile = TRFIOFile(nameFile)

  emba = gDirectory.Get("run_%d/LAr/Coverage/BadChannels/DBBadChannelsBarrelA"%(runNumber[index]))
  extractBad(emba,sporadic,highNoise,index) 
  embc = gDirectory.Get("run_%d/LAr/Coverage/BadChannels/DBBadChannelsBarrelC"%(runNumber[index]))
  extractBad(embc,sporadic,highNoise,index) 
  eca  = gDirectory.Get("run_%d/LAr/Coverage/BadChannels/DBBadChannelsEndcapA"%(runNumber[index]))
  extractBad(eca,sporadic,highNoise,index) 
  ecc  = gDirectory.Get("run_%d/LAr/Coverage/BadChannels/DBBadChannelsEndcapC"%(runNumber[index]))
  extractBad(ecc,sporadic,highNoise,index)

  print "**EMB   : sporadicBurst=",sporadic[0][index]," - highNoise=",highNoise[0][index]
  print "**EMB PS: sporadicBurst=",sporadic[1][index]," - highNoise=",highNoise[1][index]
  print "**EMEC  : sporadicBurst=",sporadic[2][index]," - highNoise=",highNoise[2][index]
  print "**FCAL  : sporadicBurst=",sporadic[3][index]," - highNoise=",highNoise[3][index]
  print "**HEC   : sporadicBurst=",sporadic[4][index]," - highNoise=",highNoise[4][index]
  
#Create 1 TGraph for each errorType/run

spGraph=[]
hiGraph=[]
spLeg = TLegend(0.55,0.55,0.65,0.75)
hiLeg = TLegend(0.55,0.55,0.65,0.75)

markerStyle = [20,21,22,23,29]
markerColor = [1,2,3,4,6]

for iPart in range(0,nbOfPart):
  spGraph.append(TGraph(nbOfRun,xAxis,sporadic[iPart]))
  spGraph[iPart].SetMarkerStyle(markerStyle[iPart])
  spGraph[iPart].SetMarkerColor(markerColor[iPart])
  spLeg.AddEntry(spGraph[iPart],returnPartName(iPart),"p")
  hiGraph.append(TGraph(nbOfRun,xAxis,highNoise[iPart]))
  hiGraph[iPart].SetMarkerStyle(markerStyle[iPart])
  hiGraph[iPart].SetMarkerColor(markerColor[iPart])
  hiLeg.AddEntry(hiGraph[iPart],returnPartName(iPart),"p")

# Plot all the TGraph in a single canvas

c1 = TCanvas('c1','c1')
c1.Divide(1,2)
c1.cd(1)
for iPart in range(0,nbOfPart):
  if iPart == 0:
    spGraph[iPart].Draw("AP")
    spGraph[iPart].SetMinimum(0)
    spGraph[iPart].SetMinimum(90)
    (spGraph[iPart].GetXaxis()).SetLabelSize(0.045)
    spGraph[iPart].SetTitle("Number of channels flagged sporadicBurstNoise - %s"%(tag))
  else:
    spGraph[iPart].Draw("P")
spLeg.Draw()

c1.cd(2)
for iPart in range(0,nbOfPart):
  if iPart == 0:
    hiGraph[iPart].Draw("AP")
    hiGraph[iPart].SetMinimum(0)
    hiGraph[iPart].SetMaximum(60)
    (hiGraph[iPart].GetXaxis()).SetLabelSize(0.045)
    hiGraph[iPart].SetTitle("Number of channels flagged highNoise - %s"%(tag))
  else:
    hiGraph[iPart].Draw("P")
hiLeg.Draw()

for iRun in range(0,nbOfRun):
  axis = spGraph[0].GetXaxis()
  for iBin in range(1,axis.GetNbins()):
    if ((iRun>=axis.GetBinLowEdge(iBin)) and (iRun<axis.GetBinUpEdge(iBin))):
      (spGraph[0].GetXaxis()).SetBinLabel(iBin,"%d"%(runNumber[iRun]))
      (hiGraph[0].GetXaxis()).SetBinLabel(iBin,"%d"%(runNumber[iRun]))

c1.cd()

