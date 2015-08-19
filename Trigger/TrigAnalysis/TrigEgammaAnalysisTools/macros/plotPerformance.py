#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# python plotEfficiencies.py -b -i histo.root -t e24_lhtight_iloose
#

from sys import argv, exit
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--file', type=str,help='input file')
parser.add_argument('--folder', type=str,choices=["Analysis","TPAnalysis"],help='folder: Analysis or TPAnalysis')
parser.add_argument('--fileformat', type=str,help='filetype: eps, pdf, ps, ...')
parser.add_argument('--trigger', type=str,help='trigger')
parser.add_argument('--trigger2', type=str,help='trigger to compare')
parser.add_argument('--hist',type=str,choices=["HLT:et","eta","phi","Reta",
    "Rhad","Rhad1","Rphi","charge",
    "d0","d0sig","deta1","deta2","dphi2",
    "dphiresc","e011","e132","e237","e277",
    "e2tsts1","eprobht","eratio","ethad","ethad1",
    "f1","f3","npixhits","nscthits","weta1","weta2",
    "EFCalo:energy","energyBE0","energyBE1","energyBE2","energyBE3","eta","eta_calo","phi","phi_calo"],help='histograms')
parser.add_argument('--level',type=str,help='trigger level: HLT, L1Calo, L2, L2Calo, EFCalo')
parser.add_argument('--type',type=str,help='Choices: Distributions or Resolutions')
args = parser.parse_args()

import ROOT
from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership
ROOT.gROOT.SetBatch(True)

from AtlasStyle import *
from getHistograms import *

inputfile=TFile(args.file)
triggerName=args.trigger
triggerName2=args.trigger2
folder=args.folder
fileformat=args.fileformat
level=args.level
hist = args.hist
type = args.type

#########################################################################################
def makeCanvas(histo1,title,triggerName,level,folder):
	h1 = histo1
	leg = TLegend(0.5,0.2,0.9,0.30, "HLT "+folder)
	leg.SetBorderSize(0)
	leg.SetFillStyle(0)
	leg.SetTextSize(0.04)
	leg.AddEntry(h1, triggerName,"l")
	c = TCanvas("c","Efficiency",800,600)
	h1.Draw()
	leg.Draw()
	c.SetGrid()
	ATLASLabel(0.2,0.2)
	if folder=="TPAnalysis":
		c.Print("plot_"+title+"_"+triggerName+"_"+level+"_TPAnalysis."+fileformat)
	else:
		c.Print("plot_"+title+"_"+triggerName+"_"+level+"_Analysis."+fileformat)

#########################################################################################
def makeCanvas2(histo1,histo2,title,triggerName,level,folder):
	h1 = histo1
        h2 = histo2
        h1.SetMarkerStyle(20)
        h1.SetMarkerColor(2)
        h1.SetLineColor(2)
        h2.SetMarkerStyle(22)
        h2.SetMarkerColor(4)
        h2.SetLineColor(4)
	leg = TLegend(0.5,0.2,0.9,0.30, "HLT "+folder)
	leg.SetBorderSize(0)
	leg.SetFillStyle(0)
	leg.SetTextSize(0.04)
	leg.AddEntry(h1, triggerName,"p")
	leg.AddEntry(h2, triggerName2,"p")
	c = TCanvas("c","Efficiency",800,600)
	h1.Draw()
        h2.Draw("SAME")
	leg.Draw()
	c.SetGrid()
	ATLASLabel(0.2,0.2)
	if folder=="TPAnalysis":
		c.Print("plot_"+title+"_"+triggerName+"_"+triggerName2+"_"+level+"_TPAnalysis."+fileformat)
	else:
		c.Print("plot_"+title+"_"+triggerName+"_"+triggerName2+"_"+level+"_Analysis."+fileformat)
#########################################################################################
### Main program
#########################################################################################


SetAtlasStyle()
if(args.hist):    
    h=getSingleTH1F(inputfile,triggerName,hist,folder,type,level)
    makeCanvas(h,args.hist+"_"+type,triggerName,level,folder)
elif(args.trigger2):
    hEt, hEta, hPhi, hMu = getTH1FsfromFile(inputfile,triggerName,folder,level)
    h2Et, h2Eta, h2Phi, h2Mu = getTH1FsfromFile(inputfile,triggerName2,folder,level)
    makeCanvas2(hEt,h2Et,"Et",triggerName,level,folder)
    makeCanvas2(hEta,h2Eta,"Eta",triggerName,level,folder)
    makeCanvas2(hPhi,h2Phi,"Phi",triggerName,level,folder)
    makeCanvas2(hMu,h2Mu,"Mu",triggerName,level,folder)
else:
    hEt, hEta, hPhi, hMu = getTH1FsfromFile(inputfile,triggerName,folder,level)
    makeCanvas(hEt,"Et",triggerName,level,folder)
    makeCanvas(hEta,"Eta",triggerName,level,folder)
    makeCanvas(hPhi,"Phi",triggerName,level,folder)
    makeCanvas(hMu,"Mu",triggerName,level,folder)
