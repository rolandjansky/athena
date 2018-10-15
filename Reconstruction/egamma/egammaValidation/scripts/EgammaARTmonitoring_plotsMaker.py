#!/usr/bin/env python
#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

import sys
import os

#Import the ROOT libraries
from ROOT import gDirectory, gROOT, gStyle, kTRUE, TCanvas, TFile, TFileIter, TH1, TH1D, TLegend, TPad

def GetKeyNames(f0, dir = ""):
    f0.cd(dir)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]

f1 = TFile(sys.argv[1])
f2 = TFile(sys.argv[2])

particleType = sys.argv[3]

fO = TFile("BN_ComparisonPlots_"+particleType+".hist.root", "RECREATE")

gROOT.SetBatch(kTRUE)
gStyle.SetOptStat(0)
for folder in GetKeyNames(f1):
    for histo in GetKeyNames(f1,folder):

        c1 = TCanvas()
        
        mainPad =  TPad("mainPad", "top", 0.00, 0.254758, 1.00, 1.00)
        mainPad.SetLeftMargin(0.12)
        mainPad.SetRightMargin(0.04)
        mainPad.SetTopMargin(0.02)
        mainPad.SetBottomMargin(0.02)
        mainPad.SetTicky(0)
        mainPad.SetTickx(0)
        mainPad.Draw()

        c1.Update()
        
        ratioPad = TPad("ratioPad","bottom", 0.00, 0.00, 1.00, 0.25)
        ratioPad.SetLeftMargin(0.12)
        ratioPad.SetRightMargin(0.04)
        ratioPad.SetTopMargin(0.03)
        ratioPad.SetTickx(0)
        ratioPad.SetBottomMargin(0.36)
        ratioPad.Draw()
        
        c1.Update()

        h_Base = f1.Get(folder+'/'+histo)
        h_Base.SetLineColor(4)
        h_Base.SetLineWidth(2)
        h_Base.GetXaxis().SetLabelOffset(1.20)
        h_Base.GetYaxis().SetTitleSize(0.045)
        h_Base.GetYaxis().SetTitleOffset(0.95)
        
        h_Night = f2.Get(folder+'/'+histo)
        h_Night.SetMarkerStyle(8)
        h_Night.SetMarkerSize(0.5)
 
        mainPad.cd()
        
        h_Base.Draw()
        h_Night.Draw("same p")

        c1.Update()

        var_name = histo.split("_", 1)[1]
        
        leg = TLegend(0.330986, 0.884087, 0.879499, 0.97053)
        leg.SetHeader(folder+''+var_name, "C")
        leg.SetNColumns(2)
        leg.SetFillStyle(0)
        leg.SetBorderSize(0)
        leg.AddEntry(h_Base , "Baseline", "l")
        leg.AddEntry(h_Night, "Nightly" , "p")
        leg.Draw()
        
        c1.Update()
                
        ratioPad.cd()
                
        h1clone = h_Night.Clone()
        h1clone.Sumw2()
        h1clone.SetStats(0)
        h1clone.Divide(h_Base)
        h1clone.SetMarkerColor(1)
        h1clone.SetMarkerStyle(20)
        if "Efficiency" in folder: 
          h1clone.GetYaxis().SetRangeUser(h1clone.GetMinimum()*0.7,h1clone.GetMaximum()*1.3)
          gStyle.SetOptStat(0)
        h1clone.GetXaxis().SetLabelSize(0.10)
        h1clone.GetXaxis().SetTitleSize(0.17)
        h1clone.GetYaxis().SetLabelSize(0.10)
        h1clone.GetYaxis().SetRangeUser(0.75, 1.25)
        h1clone.GetYaxis().SetTitle("Ratio")
        h1clone.GetYaxis().CenterTitle(1)
        h1clone.GetYaxis().SetTitleSize(0.15)
        h1clone.GetYaxis().SetTitleOffset(0.3)
        h1clone.GetYaxis().SetNdivisions(505)

        h1clone.Draw("p")
        c1.Update()
        
        c1.SaveAs(folder+'_'+var_name+".png" )
        
        fO.cd()
        c1.Write(folder+'_'+var_name)        

fO.Write()
fO.Close()

