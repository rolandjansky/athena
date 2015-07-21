#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# plotEfficiencies.py -b <root file 1> <root file 2> <trigger name> <folder>
#

from sys import argv, exit

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from AtlasStyle import *
from getHistograms import *

#########################################################################################
def makeCanvas(histo1,histo2,title,triggerName,folder):
	h1 = histo1
	h2 = histo2
	h2.SetMarkerColor(2)
	h2.SetLineColor(2)
	leg = TLegend(0.5,0.2,0.9,0.30, "HLT "+folder+" - "+triggerName)
	leg.SetBorderSize(0)
	leg.SetFillStyle(0)
	leg.SetTextSize(0.04)
	leg.AddEntry(h1, filename1,"l")
	leg.AddEntry(h2, filename2,"l")
	c = TCanvas("c","Efficiency",800,600)
	h1.Draw()
	h2.Draw("SAME")
	leg.Draw()
	c.SetGrid()
	ATLASLabel(0.2,0.2)
	if folder=="TPAnalysis":
		c.Print("plot_"+title+"_"+triggerName+"_TPAnalysis.pdf")
	else:
		c.Print("plot_"+title+"_"+triggerName+"_Analysis.pdf")


#########################################################################################
### Main program
#########################################################################################

if(len(argv)<5):
	print "usage: plotEfficiencies.py -b <root file 1> <root file 2> <trigger name> <folder>"
	exit(2)
elif(len(argv)==5):
  filename1 = argv[2]
  filename2 = argv[3]
  triggerName = argv[4]
  folder = "Analysis"
  print "ROOT File : ", filename1, filename2
  print "Trigger name :", triggerName
  inputfile1 = TFile(filename1)
  inputfile2 = TFile(filename2)
elif(len(argv)==6):
  filename1 = argv[2]
  filename2 = argv[3]
  triggerName = argv[4]
  folder = argv[5]
  if (folder!="TPAnalysis") or (folder!="Analysis"):
  	print "Last argument is wrong"
  print "ROOT File : ", filename1, filename2
  print "Trigger name :", triggerName
  inputfile1 = TFile(filename1)
  inputfile2 = TFile(filename2)
else:
    print "usage: plotEfficiencies.py -b <root file 1> <root file 2> <trigger name> <folder>"
    exit(2)

SetAtlasStyle()
hEt1, hEta1, hPhi1, hMu1 = getTH1FsfromFile(inputfile1,triggerName,folder)
hEt2, hEta2, hPhi2, hMu2 = getTH1FsfromFile(inputfile2,triggerName,folder)
makeCanvas(hEt1,hEt2,"Et",triggerName,folder)
makeCanvas(hEta1,hEta2,"Eta",triggerName,folder)
makeCanvas(hPhi1,hPhi2,"Phi",triggerName,folder)
makeCanvas(hMu1,hMu2,"Mu",triggerName,folder)
