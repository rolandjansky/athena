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
elif(len(argv)==5):
  filename = argv[2]
  folder = argv[3]
  fileformat = argv[4]
  if (folder!="TPAnalysis") or (folder!="Analysis"):
    print "Last argument is wrong"
  print "ROOT File : ", filename
  inputfile = TFile(filename)
else:
    print "usage: plotEfficiencies.py -b <root file> <trigger name> <folder> <file format>"
    exit(2)

#SetAtlasStyle()
# I want titles
#atlasStyle = AtlasStyle()
#atlasStyle.SetOptTitle(1)
#gROOT.SetStyle("ATLAS")
#gROOT.ForceStyle()

basePath1="HLT/Egamma/Analysis/"
basePath2="HLT/Egamma/TPAnalysis/"
plots=["hlt_el_et","l1_rot_et"]
tpplots=["nProbes","nProbesL1","nProbesL2Calo","nProbesEFCalo","nProbesHLT","EffL1","EffL2","EffEFCalo","EffHLT"]
name="Analysis_Event"+".ps"
c=TCanvas()
c.Print(name+"[")
c=TCanvas()
c.SetBottomMargin(0.5)
print basePath1+"trigger_counts"
h=inputfile.Get(basePath1+"trigger_counts")
h.SetStats(0)
h.GetXaxis().SetTitle("")
h.Draw()
c.Print(name)
h=inputfile.Get(basePath1+"electrons")
h.Draw()
c.Print(name)
for iplot in tpplots:
    print basePath2+"/"+iplot
    h=inputfile.Get(basePath2+"/"+iplot)
    h.SetStats(0)
    h.GetXaxis().SetTitle("")
    h.Draw()
    c.Print(name)
c2=TCanvas()
c2.Divide(2,2)
c2.cd(1)
h=inputfile.Get(basePath1+"hlt_el_et")
h.Draw()
c2.cd(2)
h=inputfile.Get(basePath1+"l1_roi_et")
h.Draw()
c2.cd(3)
h=inputfile.Get(basePath1+"hlt_el_eta_phi")
h.Draw("colz")
c2.cd(4)
h=inputfile.Get(basePath1+"l1_eta_phi")
h.Draw("colz")
c2.Print(name)
c.Print(name+"]"); 

