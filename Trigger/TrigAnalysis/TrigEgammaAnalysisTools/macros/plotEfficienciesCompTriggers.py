#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# plotEfficiencies.py -b <root file 1> <trigger name 1> <trigger name 2> <folder>"
#

from sys import argv, exit

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from AtlasStyle import *
from getHistograms import *


#########################################################################################
def makeCanvas(histo1,histo2,title,triggerName1,triggerName2,folder):
	h1 = histo1
	h2 = histo2
	h2.SetMarkerColor(2)
	h2.SetLineColor(2)
	leg = TLegend(0.5,0.2,0.9,0.30, "HLT "+folder+" - ")
	leg.SetBorderSize(0)
	leg.SetFillStyle(0)
	leg.SetTextSize(0.04)
	leg.AddEntry(h1, triggerName1,"l")
	leg.AddEntry(h2, triggerName2,"l")
	c = TCanvas("c","Efficiency",800,600)
	h1.Draw()
	h2.Draw("SAME")
	leg.Draw()
	c.SetGrid()
	ATLASLabel(0.2,0.2)
	if folder=="TPAnalysis":
		c.Print("plot_"+title+"_"+triggerName1+"_"+triggerName2+"_TPAnalysis.pdf")
	else:
		c.Print("plot_"+title+"_"+triggerName1+"_"+triggerName2+"_Analysis.pdf")


#########################################################################################
### Main program
#########################################################################################

if(len(argv)<5):
	print "usage: plotEfficiencies.py -b <root file 1> <trigger name 1> <trigger name 2> <folder>"
	exit(2)
elif(len(argv)==5):
  filename1 = argv[2]
  triggerName1 = argv[3]
  triggerName2 = argv[4]
  folder = "Analysis"
  print "ROOT File : ", filename1
  print "Trigger name :", triggerName1, triggerName2
  inputfile1 = TFile(filename1)
elif(len(argv)==6):
  filename1 = argv[2]
  triggerName1 = argv[3]
  triggerName2 = argv[4]
  folder = argv[5]
  if (folder!="TPAnalysis") or (folder!="Analysis"):
  	print "Last argument is wrong"
  print "ROOT File : ", filename1
  print "Trigger name :", triggerName1,triggerName2
  inputfile1 = TFile(filename1)
else:
    print "usage: plotEfficiencies.py -b <root file 1> <trigger name 1> <trigger name 2> <folder>"
    exit(2)

SetAtlasStyle()
hEt1, hEta1, hPhi1, hMu1 = getTH1FsfromFile(inputfile1,triggerName1,folder)
hEt2, hEta2, hPhi2, hMu2 = getTH1FsfromFile(inputfile1,triggerName2,folder)
makeCanvas(hEt1,hEt2,"Et",triggerName1,triggerName2,folder)
makeCanvas(hEta1,hEta2,"Eta",triggerName1,triggerName2,folder)
makeCanvas(hPhi1,hPhi2,"Phi",triggerName1,triggerName2,folder)
makeCanvas(hMu1,hMu2,"Mu",triggerName1,triggerName2,folder)
