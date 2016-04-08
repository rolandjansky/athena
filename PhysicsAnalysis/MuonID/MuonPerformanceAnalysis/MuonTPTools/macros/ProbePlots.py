# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import Histos
import ROOT
import DSConfig
import Utils
import sys
from Defs import *
from PlotUtils import PlotUtils
ROOT.gROOT.Macro("rootlogon.C")
ROOT.gROOT.SetStyle("ATLAS")
ROOT.gROOT.SetBatch(1)

histos = []
eff_axis_ranges = []
sf_axis_ranges = []

def draw2D (hist, var, probe, match, region, conf_Data):
    
    pu = PlotUtils()
    pu.Size = 0.06*(1-pu.VerticalCanvasSplit)
    pu.Lumi = conf_Data.Lumi/1000.
    
    ROOT.gStyle.SetPalette(54)
    
    can = ROOT.TCanvas("ProbeCheck_%s_%s_%s_%s"%(var,probe,region,hist[0]),PlotKinds.Probes,800,600)
    can.cd()
    can.SetTopMargin(0.15)
    can.SetRightMargin(0.2)
    can.SetLeftMargin(0.12)

    if "DataSM" in hist[0]:
        hist[1].GetZaxis().SetTitle("Data/SM")
    else:
        hist[1].GetZaxis().SetTitle(PlotKinds.Probes+" "+hist[0])
        
    hist[1].Draw("colz")
    hist[1].GetYaxis().SetTitleOffset(0.7*hist[1].GetYaxis().GetTitleOffset())
    hist[1].GetZaxis().SetTitleOffset(1.5*hist[1].GetZaxis().GetTitleOffset())
    # need to find a better way to do this:
    hist[1].GetZaxis().SetLabelSize(hist[1].GetYaxis().GetLabelSize())
    hist[1].GetZaxis().SetTitleSize(hist[1].GetYaxis().GetTitleSize())
    
    pu.DrawAtlas(can.GetLeftMargin(),0.92)
    pu.DrawLumiSqrtS(can.GetLeftMargin(),0.87)
    #pu.DrawTarget(match,1-can.GetRightMargin(),0.92,31)
    pu.DrawSource(probe,1-can.GetRightMargin(),0.87,31)
    
    can.SaveAs ("Plots/ProbeCheck_%s_%s_%s_%s.pdf"%(var,probe,region,hist[0]))
    
    pu.DrawTLatex(0.5,0.94,"%s for %s probes in %s %s"%(var,probe,region,hist[0]),0.04,52,22)

    can.SaveAs("Plots/AllProbePlots.pdf")
    
def doProbePlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, log):

    if len(hists_final)>1:
        print 'Error: Something went wrong in TPFinalHistos()!'
        sys.exit(1)
    else:
        h_data = hists_final[0].Histos[PlotKinds.Probes]["Data"]
        h_bkg = hists_final[0].Histos[PlotKinds.Probes]["Bkg"]
        h_mc = hists_final[0].Histos[PlotKinds.Probes]["MC"]
        h_model = hists_final[0].Histos[PlotKinds.Probes]["Model"]
        ratio = hists_final[0].Histos[PlotKinds.Probes]["DataVsModel"]
    
    ######
    # used for styling histo (when TGraphAsymmErrors are used)
    h_styling = HistoDefs.initHisto(var)[0].Clone("stylingHisto")
    # from print in Histos.py:
    h_styling.GetXaxis().SetTitleSize(0.0500000007451)
    h_styling.GetXaxis().SetTitleOffset(1.39999997616)
    h_styling.GetXaxis().SetLabelSize(0.0500000007451)
    h_styling.GetXaxis().SetLabelOffset(0.00499999988824)
    h_styling.GetYaxis().SetTitleSize(0.0500000007451)
    h_styling.GetYaxis().SetTitleOffset(1.39999997616)
    h_styling.GetYaxis().SetLabelSize(0.0500000007451)
    h_styling.GetYaxis().SetLabelOffset(0.00499999988824)

    h_styling_ratio = h_styling.Clone("stylingHistoRatio")
    h_styling_ratio.GetYaxis().SetTitle("Data/SM")
    
    h_styling.GetYaxis().SetTitle("Probes")
    ######
    
    pu = PlotUtils()
    pu.Size = 0.06
    pu.Lumi = conf_Data.Lumi/1000.
    pu.SqrtS = 13
    
    can,p1,p2 = pu.Prepare2PadCanvas("ProbeCheck_%s_%s_%s"%(var,probe,region))
    can.cd()
    p1.Draw()
    histos.append(p1)
    histos.append(p2)
    histos.append(can)
    p1.cd()
    
    
    # plotting style:
    h_data.SetMarkerStyle(ROOT.kFullDotLarge)
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetMarkerColor(h_data.GetLineColor())
    h_data.SetTitle(conf_Data.Label)
    ratio.SetMarkerStyle(ROOT.kFullDotLarge)
    ratio.SetLineColor(h_data.GetLineColor())
    ratio.SetMarkerColor(h_data.GetLineColor())
    h_bkg.SetLineColor(ROOT.kOrange-2)
    h_bkg.SetFillColor(h_bkg.GetLineColor())
    h_bkg.SetTitle("Data-driven Background")
    h_mc.SetLineColor(ROOT.kBlue-9)
    h_mc.SetFillColor(h_mc.GetLineColor())
    h_mc.SetTitle(conf_Zmumu.Label)

    if (log):
        p1.SetLogy()
        
    stk = ROOT.THStack("Stack","Stack")
    stk.Add(h_bkg)
    stk.Add(h_mc)
    stk.Draw("HIST")
    
    ######
    # labels and axis ranges
    pu.AdaptLabelsTopPad([h_styling,stk.GetHistogram(),h_data,h_bkg,h_mc])
    eff_axis_ranges.append(pu.SetFancyAxisRanges([stk.GetHistogram(), h_data, h_bkg, h_mc]))
    pu.SetFancyAxisRanges([h_styling], minmax = eff_axis_ranges[0][0][1], maxmin = eff_axis_ranges[0][0][0], fixRange=True)
    ######

    h_styling.Draw("AXIS")
    
    if(log):
        stk.SetMinimum(5e-1)
        stk.GetHistogram().SetMaximum(1e3*stk.GetMaximum())
        
    stk.Draw("sameHIST")
    h_data.Draw("same")
    
    pu.DrawAtlas(0.19,0.83)
    pu.DrawLumiSqrtS(0.19,0.76)
    pu.DrawLegend([(h_data,"PL"), (h_mc,"F"), (h_bkg,"F")],0.6,0.6,0.89,0.89)

    p2.cd()
    p2.SetGridy()
    
    ######
    # labels and axis ranges
    pu.AdaptLabelsBottomPad([h_styling_ratio,ratio])
    sf_axis_ranges.append(pu.SetFancyAxisRanges_SF_Probes([ratio]))
    pu.SetFancyAxisRanges_SF_Probes([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
    ######
    
    h_styling_ratio.Draw("AXIS")
    ratio.Draw("same")
    
    can.SaveAs ("Plots/ProbeCheck_%s_%s_%s.pdf"%(var,probe,region))
    p1.cd()
    
    pu.DrawTLatex(0.5,0.94,"%s for %s probes in %s"%(var,probe,region),0.04,52,22)

    can.SaveAs("Plots/AllProbePlots.pdf")
    
def doProbePlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, log):
    
    if len(hists_final)>1:
        print 'Error: Something went wrong in TPFinalHistos()!'
        sys.exit(1)
    else:
        h_data = hists_final[0].Histos[PlotKinds.Probes]["Data"]
        h_bkg = hists_final[0].Histos[PlotKinds.Probes]["Bkg"]
        h_mc = hists_final[0].Histos[PlotKinds.Probes]["MC"]
        h_model = hists_final[0].Histos[PlotKinds.Probes]["Model"]
        ratio = hists_final[0].Histos[PlotKinds.Probes]["DataVsModel"]
        
    
    draw2D (["Data",h_data], var, probe, match, region, conf_Data)
    draw2D (["SM",h_model], var, probe, match, region, conf_Data)
    draw2D (["DataSM",ratio], var, probe, match, region, conf_Data)

def doProbePlots (var, probe, region, conf_Data, conf_Zmumu, log=True):
    
    match = Matches.Medium
    if probe == Probes.MStoMu:
        match = Matches.Calo
        
    infiles = [
                   ["MC",conf_Zmumu],
                   ["Data",conf_Data]
                   ]

    hists_final = []
    
    # faster (without systematic uncertainty bands):
    hists_final.append(Histos.TPFinalHistos("test",probe,match,region,var,infiles,useAsymEff=False))

    if hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].InheritsFrom("TH2"):
        doProbePlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, log)
    else:
        doProbePlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, log)

    
if __name__ == "__main__":
    
    # what we want to run on:
    InputData = DSConfig.Data_15_firstWeek
    InputZmumu = DSConfig.Zmumu_mc15

    dummy = ROOT.TCanvas("dummy","dummy",800,800)
    dummy.SaveAs("Plots/AllProbePlots.pdf[")
    
    # variables we want to plot:
    #doProbePlots ("etaphiFine", Probes.ID, DetRegions.All, InputData, InputZmumu, log=False)
    #doProbePlots ("mll", Probes.ID, DetRegions.All, InputData, InputZmumu, log=False)
    #doProbePlots ("pt", Probes.ID, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("eta", Probes.ID, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("d0", Probes.ID, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("z0", Probes.ID, DetRegions.All, InputData, InputZmumu)

    doProbePlots ("etaphiFine", Probes.Calo, DetRegions.All, InputData, InputZmumu, log=False)
    doProbePlots ("mll", Probes.Calo, DetRegions.All, InputData, InputZmumu, log=False)
    doProbePlots ("pt", Probes.Calo, DetRegions.All, InputData, InputZmumu)
    doProbePlots ("eta", Probes.Calo, DetRegions.All, InputData, InputZmumu)
    doProbePlots ("d0", Probes.Calo, DetRegions.All, InputData, InputZmumu)
    doProbePlots ("z0", Probes.Calo, DetRegions.All, InputData, InputZmumu)

    #doProbePlots ("etaphiFine", Probes.MStoMu, DetRegions.All, InputData, InputZmumu, log=False)
    #doProbePlots ("mll", Probes.MStoMu, DetRegions.All, InputData, InputZmumu, log=False)
    #doProbePlots ("pt", Probes.MStoMu, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("eta", Probes.MStoMu, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("d0", Probes.MStoMu, DetRegions.All, InputData, InputZmumu)
    #doProbePlots ("z0", Probes.MStoMu, DetRegions.All, InputData, InputZmumu)
    
    #doProbePlots ("etaphiFine", Probes.ID, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("mll", Probes.ID, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("pt", Probes.ID, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("eta", Probes.ID, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("d0", Probes.ID, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("z0", Probes.ID, DetRegions.noCrack, InputData, InputZmumu)

    #doProbePlots ("etaphiFine", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("mll", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("pt", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("eta", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("d0", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("z0", Probes.Calo, DetRegions.noCrack, InputData, InputZmumu)

    #doProbePlots ("etaphiFine", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("mll", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu, log=False)
    #doProbePlots ("pt", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("eta", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("d0", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu)
    #doProbePlots ("z0", Probes.MStoMu, DetRegions.noCrack, InputData, InputZmumu)

    dummy.SaveAs("Plots/AllProbePlots.pdf]")
