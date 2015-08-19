#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# python plotDistributions.py -b -i histo.root -t e24_lhtight_iloose
#

from sys import argv, exit

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from AtlasStyle import *
from getHistograms import *

#########################################################################################
### Main program
#########################################################################################

if(len(argv)<4):
	print "usage: plotEfficiencies.py -b <root file> <trigger name> <folder> <file format>"
	exit(2)
elif(len(argv)==6):
  filename = argv[2]
  triggerName = argv[3]
  folder = argv[4]
  fileformat = argv[5]
  if (folder!="TPAnalysis") or (folder!="Analysis"):
    print "Last argument is wrong"
  print "ROOT File : ", filename
  print "Trigger name :", triggerName
  inputfile = TFile(filename)
else:
    print "usage: plotEfficiencies.py -b <root file> <trigger name> <folder> <file format>"
    exit(2)


#SetAtlasStyle()
Level1=[
"pt","et",
"eta","phi","e011",
"e132","e237","e277",
]
Level2=["ethad","ethad1","weta1",
"weta2","f1","f3",
"e2tsts1","Reta","Rphi",
]
Level3=["Rhad","Rhad1","deta1",
"deta2","dphi2","dphiresc",
"d0","d0sig","eratio",
]
Level4=["eprobht","nscthits","npixhits",
]

EFCaloLevels=["energyBE0","energyBE1","energyBE2",
"energyBE3","energy","eta",
"phi","eta_calo","phi_calo",
]

L1CaloIdd=[
"energy",
"roi_et",
"emIso",
"hadCore",
"eta",
"phi",
]

L2ElectronIdd = ["trkClusDeta","trkClusDphi"]

ListAll = [Level1,Level2,Level3,Level4,EFCaloLevels,L2ElectronIdd,L1CaloIdd]
NameList = ['HLT','HLT','HLT','HLT','EFCalo','L2Electron','L1Calo']
def makeCanvas(idd,idx,idy,level,type):
	c=TCanvas()
	c.Divide(idx,idy)
	i=1
	for histoname in idd:
		h = getSingleTH1F(inputfile,triggerName,histoname,folder,type,level)
		c.cd(i)
		h.Draw()
		i=i+1
	return c

# I want titles
atlasStyle = AtlasStyle()
atlasStyle.SetOptTitle(1)
gROOT.SetStyle("ATLAS")
gROOT.ForceStyle()

name=triggerName+"_Distributions"+".ps"
iname=0
c1=TCanvas()
c1.Print(name+"[")
for ilist in ListAll:
    c1 = makeCanvas(ilist,3,3,NameList[iname],"Distributions")
    c1.Print(name)
    if "HLT" in NameList[iname]:
        c1 = makeCanvas(ilist,3,3,NameList[iname],"Resolutions")
    c1.Print(name)
    iname += 1

c1.Print(name+"]"); 
