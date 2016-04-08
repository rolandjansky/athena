# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###################################################
# Simple macro to test reco SF files
# 20.6.2015, goblirsc<at>cern.ch
###################################################

import ROOT 
import os
import commands
import math
import sys

ROOT.gROOT.Macro("rootlogon.C")
ROOT.gROOT.SetStyle("ATLAS")
ROOT.gROOT.SetBatch(1)
def GetHistos(histo, histosys, biny, binz):
    hname = histo.GetName()+"_%i_%i"%(biny,binz)
    hproj = histo.ProjectionX(hname,biny,biny,binz,binz)
    hproj.SetDirectory(0)
    hproj_sys = histosys.ProjectionX(hname+"Sys",biny,biny,binz,binz)
    hproj_sys.SetDirectory(0)
    for k in range (hproj_sys.GetNbinsX()+1):
        hproj.SetBinContent(k,hproj.GetBinContent(k) + 0.1 * (biny - 1))
        syserr = hproj_sys.GetBinContent(k)
        hproj_sys.SetBinContent(k,hproj.GetBinContent(k))
        hproj_sys.SetBinError(k,syserr)
    
        
    return [hproj, hproj_sys]

def DrawCheckPlot(histo, histo_sys):
    plots = []
    hname = histo.GetName()
    for h in range (1,histo.GetNbinsZ()+1):
        can = ROOT.TCanvas("can_%i"%h,"can",800,600)
        can.cd()
        leg = ROOT.TLegend(0.1,0.75,0.8,0.95)
        leg.SetBorderSize(0)
        leg.SetNColumns(3)
        leg.SetFillStyle(0)
        first = True
        colours = [ROOT.kBlue-1, ROOT.kGreen-2, ROOT.kRed-2,ROOT.kOrange-3,ROOT.kCyan, ROOT.kMagenta+1]
        for g in range (1,histo.GetNbinsY()+1):
                subhistos = GetHistos(histo,histo_sys,g,h)
                subhistos[0].SetMarkerStyle(ROOT.kDot)
                subhistos[1].SetFillColor(colours[g])
                plots.append(subhistos)
                if first:
                    subhistos[1].GetYaxis().SetRangeUser(0.9,2.)
                    subhistos[1].Draw("E2")
                    first = False
                else:
                    subhistos[1].Draw("E2same")
                subhistos[0].Draw("same")
                if g < histo.GetNbinsY():
                    leg.AddEntry(subhistos[1],"%.1f GeV < pt < %.1f GeV"%(histo.GetYaxis().GetBinLowEdge(g),(histo.GetYaxis().GetBinUpEdge(g))))
                else:
                    leg.AddEntry(subhistos[1],"%.1f GeV < pt"%(histo.GetYaxis().GetBinLowEdge(g)))
        chargesuff = "_posq"
        leg.Draw()
        if histo.GetZaxis().GetBinCenter(h) < 0:
            chargesuff = "_negq"
        can.SaveAs("Plots/SF_Check_%s%s.pdf"%(hname,chargesuff))
    
if __name__ == "__main__":
    inf = sys.argv[-1]
    rf = ROOT.TFile.Open(inf,"READ")
    hsf = rf.Get("SF_All")
    hsys = rf.Get("SF_sys_All")
    DrawCheckPlot(hsf,hsys)