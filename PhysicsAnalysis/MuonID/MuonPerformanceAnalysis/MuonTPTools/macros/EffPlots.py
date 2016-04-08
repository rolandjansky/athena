# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import Histos
import ROOT
import DSConfig
import Utils
from Defs import *
from PlotUtils import PlotUtils
ROOT.gROOT.Macro("rootlogon.C")
ROOT.gROOT.SetStyle("ATLAS")
ROOT.gROOT.SetBatch(1)
import sys

histos = []
eff_axis_ranges = []
sf_axis_ranges = []

def draw2D (histo, var, probe, match, region, conf_Data, histoKind, corr, LumiScale, analysis=Analysis.Z_Reco):

    pu = PlotUtils()
    pu.Size = 0.06*(1-pu.VerticalCanvasSplit)
    pu.Lumi = conf_Data.Lumi/1000.


    can = ROOT.TCanvas("Eff_%s_%s_%s_%s_%s"%(match,var,probe,region,histoKind.replace(" ","")),"Efficiencies",600,600)
    can.cd()

    histos.append(can)
    histo.GetZaxis().SetTitle(histoKind)

    if HistoKinds.SF in histoKind:
        ROOT.gStyle.SetPalette(55)
        histo.GetZaxis().SetRangeUser(0.85,1.15)
    else:
        ROOT.gStyle.SetPalette(54)
        histo.GetZaxis().SetRangeUser(0.80,1.01)

    histo.Draw("colz l")
    #histo.Draw("same TEXT")

    can.SetTopMargin(0.15)
    can.SetRightMargin(0.2)
    can.SetLeftMargin(0.12)
    histo.GetYaxis().SetTitleOffset(0.7*histo.GetYaxis().GetTitleOffset())
    histo.GetZaxis().SetTitleOffset(1.5*histo.GetZaxis().GetTitleOffset())
    # need to find a better way to do this:
    histo.GetZaxis().SetLabelSize(histo.GetYaxis().GetLabelSize())
    histo.GetZaxis().SetTitleSize(histo.GetYaxis().GetTitleSize())

    pu.DrawAtlas(can.GetLeftMargin(),0.92)
    pu.DrawLumiSqrtS(can.GetLeftMargin(),0.87)
    pu.DrawTarget(match,1-can.GetRightMargin(),0.92,31)
    pu.DrawSource(probe,1-can.GetRightMargin(),0.87,31)

    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        pu.DrawTLatex(can.GetLeftMargin()+0.02,can.GetBottomMargin()+0.02,"ID eff. #font[62]{not} applied")

    corrstr = ""
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        corrstr = "_noIDCorr"
    nEventsFileName = ""
    nEventsStr = ""
    if LumiScale != -1:
        nEventsFileName = "_%03d"%nEventsFileNameInt
        nEventsStr = ("_%.4fipb"%LumiScale).replace(".","pt")
        
    can.SaveAs ("Plots/Eff_%s_%s_%s_%s_%s%s%s.pdf"%(match,var,probe,region,histoKind.replace(" ",""),corrstr,nEventsFileName))
    
    if HistoKinds.SF in histoKind:
        pu.DrawTLatex(0.5,0.96,"%s SF%s vs %s for %s probes in %s %s"%(match,corrstr,var,probe,region,nEventsStr),pu.Size*(1-pu.VerticalCanvasSplit),52,21)
    else:
        pu.DrawTLatex(0.5,0.96,"%s Effi%s vs %s for %s probes in %s %s"%(match,corrstr,var,probe,region,nEventsStr),pu.Size*(1-pu.VerticalCanvasSplit),52,21)

    can.SaveAs("Plots/AllEffPlots.pdf")

def doEffPlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, corr = True, syshisto = None, analysis=Analysis.Z_Reco):

    # check if 2 sets of histos are available -> compare them
    if len(hists_final)>1:
        h_ref = hists_final[1].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
        ratio2 = hists_final[1].Histos[HistoKinds.SF][HistoKinds.SF]
        if match in Matches.ID:
            h_ref_mc = hists_final[1].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    else:
        h_ref = None

    h_data = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_truth = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency]
    ratio = hists_final[0].Histos[HistoKinds.SF][HistoKinds.SF]
    ratiotruth = hists_final[0].Histos[HistoKinds.SF][HistoKinds.TruthSF]
        
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
    h_styling_ratio.GetYaxis().SetTitle("Data/MC")
    
    h_styling.GetYaxis().SetTitle("Efficiency")
    ######

    ##############
    if var == "DetRegions":
    # for week-1 data (DetRegions plot)
        h_styling.SetLabelOffset(0.025, "X")
        h_styling.GetXaxis().SetRangeUser(1,11)
        for i in range(1,Utils.GetNbins(h_styling)-1):
            h_styling.GetXaxis().SetBinLabel(i,"")
        h_styling.GetXaxis().SetTitle("")

        h_styling_ratio.SetLabelOffset(0.025, "X")
        h_styling_ratio.GetXaxis().SetRangeUser(1,11)
        h_styling_ratio.GetXaxis().SetTitleOffset(h_styling_ratio.GetXaxis().GetTitleOffset()*1.1)
    ##############

    pu = PlotUtils()
    pu.Size = 0.06
    pu.Lumi = conf_Data.Lumi/1000.
    pu.SqrtS = 13

    # for evolution plots
    nEventsStr = ""
    if LumiScale != -1:
        nEventsStr = ("_%.3fipb"%LumiScale).replace(".","pt")
    if var == "DetRegions":
        can,p1,p2 = pu.Prepare2PadCanvas("Eff_%s_%s_%s_%s%s"%(match,var,probe,region,nEventsStr),width=1000, height=800)
    else:
        can,p1,p2 = pu.Prepare2PadCanvas("Eff_%s_%s_%s_%s%s"%(match,var,probe,region,nEventsStr))
    
    can.cd()
    p1.Draw()
    histos.append(p1)
    histos.append(p2)
    histos.append(can)
    p1.cd()


    # plotting style:
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetMarkerStyle(ROOT.kFullDotLarge)
    if h_ref != None:
        h_ref.SetLineColor(ROOT.kOrange-2)
        h_ref.SetMarkerColor(ROOT.kOrange-2)
        h_ref.SetMarkerStyle(ROOT.kFullSquare)
        h_ref.SetTitle(hists_final[1].Name)
        # for comparing Matches.ID to Matches.ID_noMCP
        if match in Matches.ID:
            h_ref.SetTitle(Matches.ID_noMCP)
            h_ref_mc.SetTitle("%s MC"%(Matches.ID_noMCP))

    h_mc.SetLineColor(ROOT.kRed-7)
    h_mc.SetMarkerColor(ROOT.kRed-7)
    h_mc.SetMarkerStyle(ROOT.kOpenCircle)
    h_mc.SetTitle(conf_Zmumu.Label)

    h_truth.SetLineColor(ROOT.kBlue-7)
    h_truth.SetMarkerColor(ROOT.kBlue-7)
    h_truth.SetMarkerStyle(ROOT.kOpenSquare)
    h_truth.SetTitle("Truth MC")
    h_data.SetTitle(conf_Data.Label)

    ratio.SetLineColor(ROOT.kGreen-8)
    ratio.SetFillColor(ratio.GetLineColor())
    ratio.SetMarkerColor(ROOT.kBlack)
    ratio.SetMarkerStyle(ROOT.kFullDotLarge)

    if h_ref != None:
        ratio2.SetLineColor(h_ref.GetLineColor())
        ratio2.SetMarkerColor(h_ref.GetMarkerColor())
        ratio2.SetMarkerStyle(ROOT.kOpenSquare)
    else:
        ratio2 = None

    ratiotruth.SetLineColor(h_truth.GetLineColor())
    ratiotruth.SetMarkerColor(h_truth.GetMarkerColor())
    ratiotruth.SetMarkerStyle(ROOT.kOpenSquare)
    ratiotruth.SetTitle("Truth MC")

    ######
    # labels and axis ranges
    pu.AdaptLabelsTopPad([h_styling, h_truth, h_data, h_mc])
    # for creating evolution plots, SetFancyAxisRanges_Eff has to be fixed
    #if False:
    if LumiScale != -1:
        if len(eff_axis_ranges)<1:
            eff_axis_ranges.append(pu.SetFancyAxisRanges_Eff([h_data,h_mc], maxmin = 1.05))
            pu.SetFancyAxisRanges_Eff([h_styling], maxmin = eff_axis_ranges[0][0][0], fixRange=True)
            #print eff_axis_ranges
        else:
            eff_axis_ranges.append(pu.SetFancyAxisRanges_Eff([h_data,h_mc], maxmin = eff_axis_ranges[0][0][0], fixRange=True))
            pu.SetFancyAxisRanges_Eff([h_styling], maxmin = eff_axis_ranges[0][0][0], fixRange=True)
            #print eff_axis_ranges
    else:
        eff_axis_ranges.append(pu.SetFancyAxisRanges_Eff([h_data,h_mc], maxmin = 1.05))
        pu.SetFancyAxisRanges_Eff([h_styling], maxmin = eff_axis_ranges[0][0][0], fixRange=True)
        del eff_axis_ranges[:]
    ######
        
    h_styling.Draw("AXIS")
    if h_mc.InheritsFrom("TGraphAsymmErrors"):
        h_mc.Draw("sameP")
    else:
        h_mc.Draw("same")

    if h_ref == None:
        #h_truth.Draw("same")
        h_truth.GetYaxis().SetTitle("Efficiency")
    if h_ref != None:
        if h_mc.InheritsFrom("TGraphAsymmErrors"):
            h_ref.Draw("sameP")
        else:
            h_ref.Draw("same")
        # for comparing Matches.ID to Matches.ID_noMCP
        if match in Matches.ID:
            h_ref_mc.Draw("same")
    if h_mc.InheritsFrom("TGraphAsymmErrors"):
        h_data.Draw("sameP")
    else:
        h_data.Draw("same")
        
    pu.DrawAtlas(0.19,0.4)
    
    if LumiScale != -1:
        lumi=LumiScale*InputData.Lumi
        pu.DrawLumiSqrtSEvolution(0.19,0.32,lumi=lumi)
    else:
        pu.DrawLumiSqrtS(0.19,0.32)
    if h_ref != None:
        # for comparing Matches.ID to Matches.ID_noMCP
        if match in Matches.ID:
            pu.DrawLegend([(h_data,"PL"), (h_ref,"PL"), (h_mc,"PL"), (h_ref_mc,"PL")],0.69,0.08,0.89,0.33)
        else:
            pu.DrawLegend([(h_data,"PL"), (h_ref,"PL"), (h_mc,"PL")],0.69,0.08,0.89,0.33)
    else:
        #pu.DrawLegend([(h_data,"PL"), (h_mc,"PL"), (h_truth,"PL")],0.69,0.08,0.89,0.33)
        pu.DrawLegend([(h_data,"PL"), (h_mc,"PL")],0.55,0.32,1-ROOT.gPad.GetRightMargin(),0.47)
    
    #pu.DrawSource(probe,0.69,0.36)
    pu.DrawTarget(match,0.19,0.24)
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        pu.DrawTLatex(0.19,0.16,"ID eff. #font[62]{not} applied")
    
    
    p2.cd()
    p2.SetGridy()
    
    ######
    # labels and axis ranges
    pu.AdaptLabelsBottomPad([h_styling_ratio,ratiotruth,ratio])
    if syshisto != None:
        ratioWithTotalSys = ratio.Clone("%s_TotalSys"%(ratio.GetName()))
        # put the following into SetFancyAxisRanges_SF in order to consider the systematic bands
        ratioWithTotalSysAxis = ratio.Clone("%s_TotalSysForFancyAxisRanges"%(ratio.GetName()))
        if not ratio.InheritsFrom("TGraphAsymmErrors"):
            ratioWithTotalSys.SetDirectory(0)
            ratioWithTotalSysAxis.SetDirectory(0)
        nbins = Utils.GetNbins(ratioWithTotalSys)
        for i in range (0, nbins):
            if ratio.InheritsFrom("TGraphAsymmErrors"):
                ratioWithTotalSys.SetPointEYhigh(i,ROOT.sqrt(ratioWithTotalSys.GetErrorYhigh(i)**2.+(ratioWithTotalSys.GetY()[i]*syshisto.GetY()[i])**2.))
                ratioWithTotalSys.SetPointEYlow(i,ROOT.sqrt(ratioWithTotalSys.GetErrorYlow(i)**2.+(ratioWithTotalSys.GetY()[i]*syshisto.GetY()[i])**2.))
                #Currently not used in SetFancyAxisRanges_SF
                ratioWithTotalSysAxis.SetPoint(i,ratioWithTotalSysAxis.GetX()[i],ratio.GetY()[i]+ratioWithTotalSys.GetErrorYlow(i))
            else:
                ratioWithTotalSys.SetBinError(i,ROOT.sqrt(ratioWithTotalSys.GetBinError(i)*ratioWithTotalSys.GetBinError(i)+ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)*ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)))
                #Currently not used in SetFancyAxisRanges_SF
                ratioWithTotalSysAxis.SetBinContent(i,ratio.GetBinContent(i)+ratioWithTotalSys.GetBinError(i))
    # for creating evolution plots, SetFancyAxisRanges_SF has to be fixed
    #if False:
    if LumiScale != -1:
        if len(sf_axis_ranges)<1:
            if syshisto != None:
                sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio,ratioWithTotalSys,ratioWithTotalSysAxis],minmax = 0.8, maxmin = 1.2))
                pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
            else:
                sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio],minmax = 0.8, maxmin = 1.2))
                pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
            #print sf_axis_ranges
        else:
            if syshisto != None:
                sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio,ratioWithTotalSys,ratioWithTotalSysAxis],minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True))
                pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
            else:
                sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio],minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True))
                pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
            #print sf_axis_ranges
    else:
        if syshisto != None:
            sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio,ratioWithTotalSys],minmax = 0.8, maxmin = 1.2))
            pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
        else:
            sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio],minmax = 0.8, maxmin = 1.2))
            pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
        #print sf_axis_ranges
        del sf_axis_ranges[:]
    ######

    h_styling_ratio.Draw("AXIS")
    if syshisto != None:
        ratioWithTotalSys.SetFillColor(ROOT.kOrange-3)
        ratioWithTotalSys.SetLineColor(ROOT.kOrange-3)
        if h_mc.InheritsFrom("TGraphAsymmErrors"):
            ratioWithTotalSys.Draw("same2")
            ratio.Draw("same2")
        else:
            ratioWithTotalSys.Draw("E2")
            ratio.Draw("E2 same")
    else:
        if h_mc.InheritsFrom("TGraphAsymmErrors"):
            ratio.Draw("same2")
        else:
            ratio.Draw("E2 same")

    if h_ref == None:
        #ratiotruth.Draw("same")
        pass
    if ratio2 != None:
        ratio2.Draw("same")
    if h_mc.InheritsFrom("TGraphAsymmErrors"):
        ratio.Draw("sameP")
    else:
        ratio.Draw("same")

        
    ROOT.gPad.RedrawAxis()
    corrstr = ""
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        corrstr = "_noIDCorr"
    nEventsFileName = ""
    nEventsStr = ""
    if LumiScale != -1:
        nEventsFileName = "_%04d"%nEventsFileNameInt
        nEventsStr = ("_%.3fipb"%LumiScale).replace(".","pt")
        p1.cd()
        #pu.DrawTLatex(1-can.GetRightMargin(),0.94,"%.0f Zs"%(nMax),0.04,52,31)
        can.cd()
        can.Print("Plots/test.gif+10")

    can.SaveAs ("Plots/Eff_%s_%s_%s_%s%s%s.pdf"%(match,var,probe,region,corrstr,nEventsFileName))
    p1.cd()

    pu.DrawTLatex(0.5,0.94,"%s Effi%s vs %s for %s probes in %s %s"%(match,corrstr,var,probe,region,nEventsStr),0.04,52,22)

    can.SaveAs("Plots/AllEffPlots.pdf")


def doSFValidation1D (hists_final, var, probe, match, region, conf_Data, corr = True, syshisto = None, analysis=Analysis.Z_Reco):


    # check if 2 sets of histos are available -> compare them
    if len(hists_final)<2:
        return
    
    h_mc_SF = hists_final[1].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    ratio_SF = hists_final[1].Histos[HistoKinds.SF][HistoKinds.SF]

    pu = PlotUtils()
    pu.Size = 0.06
    pu.Lumi = conf_Data.Lumi/1000.

    can,p1,p2 = pu.Prepare2PadCanvas("SF_Valid_%s_%s_%s_%s"%(match,var,probe,region))
    can.cd()

    p1.Draw()

    histos.append(p1)
    histos.append(p2)
    histos.append(can)
    p1.cd()
    
    
    # used for styling histo when TGraphAsymmErrors are used
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
    
    h_styling_ratio = h_styling.Clone("stylingHisto")
    
    h_styling.GetYaxis().SetTitle("Efficiency")

    h_data = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    ratio = hists_final[0].Histos[HistoKinds.SF][HistoKinds.SF]

    #h_mc.SetFillColor(ROOT.kRed-7)
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetMarkerStyle(ROOT.kFullDotLarge)

    h_mc_SF.SetFillColor(ROOT.kBlue-7)
    h_mc_SF.SetMarkerColor(ROOT.kBlue-7)
    h_mc_SF.SetLineColor(ROOT.kBlue-7)
    h_mc_SF.SetMarkerSize(1.25 * h_mc_SF.GetMarkerSize())
    h_mc_SF.SetMarkerStyle(ROOT.kOpenSquare)
    h_mc_SF.SetTitle("MC after SF")

    #h_mc.SetFillColor(ROOT.kRed-7)
    h_mc.SetLineColor(ROOT.kRed-7)
    h_mc.SetMarkerColor(ROOT.kRed-7)
    h_mc.SetMarkerStyle(ROOT.kOpenCircle)
    h_mc.SetTitle("MC before SF")

    h_data.SetTitle(conf_Data.Label)

    ratio.SetLineColor(h_mc.GetLineColor())
    ratio.SetMarkerColor(h_mc.GetMarkerColor())
    ratio.SetMarkerStyle(h_mc.GetMarkerStyle())
    ratio.SetTitle("MC before SF")

    ratio_SF.SetLineColor(h_mc_SF.GetLineColor())
    ratio_SF.SetMarkerSize(1.25 * ratio_SF.GetMarkerSize())
    ratio_SF.SetMarkerColor(h_mc_SF.GetMarkerColor())
    ratio_SF.SetMarkerStyle(h_mc_SF.GetMarkerStyle())
    ratio_SF.SetTitle("MC after SF")

    pu.AdaptLabelsTopPad([h_styling,h_data,h_mc,h_mc_SF])
    eff_axis_ranges.append(pu.SetFancyAxisRanges_Eff([h_data,h_mc,h_mc_SF], maxmin = 1.05))
    pu.SetFancyAxisRanges_Eff([h_styling], maxmin = eff_axis_ranges[0][0][0], fixRange=True)
    del eff_axis_ranges[:]
    
    h_styling.Draw("AXIS")
    if h_data.InheritsFrom("TGraphAsymmErrors"):
        h_mc.Draw("sameP")
        h_mc_SF.Draw("sameP")
        h_data.Draw("sameP")
    else:
        h_mc.Draw("same")
        h_mc_SF.Draw("same")
        h_data.Draw("same")

    pu.DrawAtlas(0.19,0.4)
    pu.DrawLumiSqrtS(0.19,0.32)
    pu.DrawLegend([(h_data,"PL"), (h_mc,"PL"), (h_mc_SF,"PL")],0.69,0.08,0.89,0.33)
    
    pu.DrawSource(probe,0.69,0.36)
    pu.DrawTarget(match,0.19,0.24)
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        pu.DrawTLatex(0.19,0.16,"ID eff. #font[62]{not} applied")
    p2.cd()

    p2.SetGridy()

    pu.AdaptLabelsBottomPad([h_styling_ratio,ratio_SF,ratio])
    h_styling_ratio.GetYaxis().SetTitle("Data/MC")
    
    sf_axis_ranges.append(pu.SetFancyAxisRanges_SF([ratio_SF,ratio],minmax = 0.9, maxmin = 1.1))
    pu.SetFancyAxisRanges_SF([h_styling_ratio], minmax = sf_axis_ranges[0][0][1], maxmin = sf_axis_ranges[0][0][0], fixRange=True)
    del sf_axis_ranges[:]


    if syshisto != None:
        ratioWithTotalSys = ratio.Clone("%s_TotalSys"%(ratio.GetName()))
        if not ratio.InheritsFrom("TGraphAsymmErrors"):
            ratioWithTotalSys.SetDirectory(0)
            nbins = ratioWithTotalSys.GetNbinsX()+1
            for i in range (1, nbins):
                ratioWithTotalSys.SetBinError(i,ROOT.sqrt(ratioWithTotalSys.GetBinError(i)*ratioWithTotalSys.GetBinError(i)+ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)*ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)))
        ratioWithTotalSys.SetFillColor(ROOT.kOrange-3)
        ratioWithTotalSys.SetLineColor(ROOT.kOrange-3)
        ratio.SetFillColor(ROOT.kGreen-8)
        ratio.SetLineColor(ROOT.kGreen-8)
        ratio.SetMarkerColor(ROOT.kBlack)
        ratio.SetMarkerStyle(ROOT.kFullDotLarge)
        h_styling_ratio.Draw("AXIS")
        if ratio.InheritsFrom("TGraphAsymmErrors"):
            ratioWithTotalSys.Draw("same2")
            ratio.Draw("same2")
        else:
            ratioWithTotalSys.Draw("E2 same")
            ratio.Draw("E2 same")
    else:
        h_styling_ratio.Draw("AXIS")
        if ratio.InheritsFrom("TGraphAsymmErrors"):
            ratio.Draw("sameP")
        else:
            ratio.Draw("same")

    if ratio.InheritsFrom("TGraphAsymmErrors"):
        ratio_SF.Draw("sameP")
    else:
        ratio_SF.Draw("same")
    ROOT.gPad.RedrawAxis()
    chi2_prev = 0.
    chi2_now = 0.
    for a in range (1,Utils.GetNbins(ratio)):
        if ratio.InheritsFrom("TGraphAsymmErrors"):
            chi2_prev = chi2_prev + (ratio.GetY()[a] - 1.)**2 / ratio.GetN()
            chi2_now = chi2_now + (ratio_SF.GetY()[a] - 1.)**2 / ratio_SF.GetN()
        else:
            chi2_prev = chi2_prev + (ratio.GetBinContent(a) - 1.)**2 / ratio.GetNbinsX()
            chi2_now = chi2_now + (ratio_SF.GetBinContent(a) - 1.)**2 / ratio_SF.GetNbinsX()
    chi2_prev = math.sqrt(chi2_prev)
    chi2_now = math.sqrt(chi2_now)
    pu.DrawTLatex(0.19, 0.5, " #frac{<(SF -1)>}{#sqrt{N_{bins}}}: %.4f before SF, %.4f after"%(chi2_prev, chi2_now))
    corrstr = ""
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        corrstr = "_noIDCorr"
    can.SaveAs ("Plots/SFValid_%s_%s_%s_%s%s.pdf"%(match,var,probe,region,corrstr))
    p1.cd()
    ROOT.gPad.RedrawAxis()

    pu.DrawTLatex(0.5,0.94,"%s Effi%s vs %s for %s probes in %s"%(match,corrstr,var,probe,region),0.04,52,22)

    can.SaveAs("Plots/AllEffPlots.pdf")
    
def doEffPlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, corr = True, analysis=Analysis.Z_Reco):

    # check if 2 sets of histos are available -> how to plot a comparison in 2D?
    if len(hists_final)>1:
        print "Error: Currently no 2D comparison between 2 different datasets possible!"
        sys.exit(1)


    h_data = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_truth = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency]

    ratiodata = hists_final[0].Histos[HistoKinds.SF][HistoKinds.SF]
    ratiotruth = hists_final[0].Histos[HistoKinds.SF][HistoKinds.TruthSF]

    draw2D (h_data, var, probe, match, region, conf_Data, HistoKinds.DataEfficiency, corr, LumiScale)
    draw2D (h_mc, var, probe, match, region, conf_Data, HistoKinds.MCEfficiency, corr, LumiScale)
    draw2D (h_truth, var, probe, match, region, conf_Data, HistoKinds.TruthEfficiency, corr, LumiScale)

    draw2D (ratiodata, var, probe, match, region, conf_Data, HistoKinds.SF, corr, LumiScale)
    draw2D (ratiotruth, var, probe, match, region, conf_Data, HistoKinds.TruthSF, corr, LumiScale)

def doEffPlots (var, probe, match, region, conf_Data, conf_Zmumu, inputComparison = None, corr = True, includeSys = True, doClosure = False, LumiScale=-1, doPreRecEtaCopy=False, analysis=Analysis.Z_Reco):

    infiles = [
                   ["MC",conf_Zmumu],
                   ["Data",conf_Data]
                   ]

    hists_final = []

    # for pre-recommendations, copy negative eta bins of transition region to positive ones (use doPreRecEtaCopy=True):
    if includeSys:
        # with systematic uncertainty bands:
        test = Histos.TPFinalSysHistos("test",probe,match,region,var,infiles,charge=None,corr=corr,doClosure=doClosure,LumiScale=LumiScale,doPreRecEtaCopy=doPreRecEtaCopy, analysis=analysis).HistoSets
        syshisto = test[Systematics.All][0]
        hists_final.append(test["Nominal"])
    else:
        # faster (without systematic uncertainty bands):
        hists_final.append(Histos.TPFinalHistos("test",probe,match,region,var,infiles,corr=corr,doClosure=doClosure,LumiScale=LumiScale,doPreRecEtaCopy=doPreRecEtaCopy, analysis=analysis))
        syshisto = None


    if inputComparison != None:
        infiles2 = [
                   ["MC",conf_Zmumu],
                   ["Data",inputComparison]
                   ]
        hists_final.append(Histos.TPFinalHistos(inputComparison.Label,probe,match,region,var,infiles2,corr=corr,doClosure=doClosure, analysis=analysis))

    if hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].InheritsFrom("TH2"):
        doEffPlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, corr, analysis=analysis)
    else:
        doEffPlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, corr, syshisto, analysis=analysis)


def doSysPlot(probe,match,region,var,infiles,charge=None,corr = True, doPreRecEtaCopy=False):

    testpos = Histos.TPFinalSysHistos("testpos",probe,match,region,var,infiles,charge,corr,doPreRecEtaCopy=doPreRecEtaCopy).HistoSets
    
    # used for styling histo when TGraphAsymmErrors are used
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
    h_styling.GetYaxis().SetTitle("Relative Systematic uncertainty (%)")
    
    h_truth_sys_sf = testpos[Systematics.truth][2]
    h_dR_sys_sf = testpos[Systematics.dR][2]
    h_BG_sys_sf = testpos[Systematics.BG][2]
    h_Det_sys_sf = testpos[Systematics.Det][2]
    h_total_sys_sf = testpos[Systematics.All][2]

    h_stat_sf = testpos["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF]
    
    if h_stat_sf.InheritsFrom("TGraphAsymmErrors"):
        for i in range (0,Utils.GetNbins(h_stat_sf)):
            maxError = max(h_stat_sf.GetErrorYhigh(i),h_stat_sf.GetErrorYlow(i))
            try:
                h_stat_sf.SetPoint(i,h_stat_sf.GetX()[i],maxError/h_stat_sf.GetY()[i])
            except ZeroDivisionError:
                h_stat_sf.SetPoint(i,h_stat_sf.GetX()[i],0.)
            h_total_sys_sf.SetPoint(i,h_total_sys_sf.GetX()[i],math.sqrt(h_total_sys_sf.GetY()[i]**2. + h_stat_sf.GetY()[i]**2.))
    else:
        for i in range (1,Utils.GetNbins(h_stat_sf)):
            try:
                h_stat_sf.SetBinContent(i,h_stat_sf.GetBinError(i)/h_stat_sf.GetBinContent(i))
            except ZeroDivisionError:
                h_stat_sf.SetBinContent(i,0.)
            h_total_sys_sf.SetBinContent(i,math.sqrt(h_total_sys_sf.GetBinContent(i)*h_total_sys_sf.GetBinContent(i) + h_stat_sf.GetBinContent(i)*h_stat_sf.GetBinContent(i)))
    
    if h_stat_sf.InheritsFrom("TGraphAsymmErrors"):
        for i in range (0,Utils.GetNbins(h_stat_sf)):
            h_truth_sys_sf.SetPoint(i,h_truth_sys_sf.GetX()[i],h_truth_sys_sf.GetY()[i]*100.)
            h_dR_sys_sf.SetPoint(i,h_dR_sys_sf.GetX()[i],h_dR_sys_sf.GetY()[i]*100.)
            h_BG_sys_sf.SetPoint(i,h_BG_sys_sf.GetX()[i],h_BG_sys_sf.GetY()[i]*100.)
            h_Det_sys_sf.SetPoint(i,h_Det_sys_sf.GetX()[i],h_Det_sys_sf.GetY()[i]*100.)
            h_total_sys_sf.SetPoint(i,h_total_sys_sf.GetX()[i],h_total_sys_sf.GetY()[i]*100.)
            h_stat_sf.SetPoint(i,h_stat_sf.GetX()[i],h_stat_sf.GetY()[i]*100.)
    else:
        for i in range (1,Utils.GetNbins(h_truth_sys_sf)):
            h_truth_sys_sf.SetBinContent(i, h_truth_sys_sf.GetBinContent(i)*100.)
            h_dR_sys_sf.SetBinContent(i, h_dR_sys_sf.GetBinContent(i)*100.)
            h_BG_sys_sf.SetBinContent(i, h_BG_sys_sf.GetBinContent(i)*100.)
            h_Det_sys_sf.SetBinContent(i, h_Det_sys_sf.GetBinContent(i)*100.)
            h_total_sys_sf.SetBinContent(i, h_total_sys_sf.GetBinContent(i)*100.)
            h_stat_sf.SetBinContent(i, h_stat_sf.GetBinContent(i)*100.)
        
    pu = PlotUtils()
    pu.Size = 0.06 * (1 - pu.VerticalCanvasSplit)
    pu.Lumi = infiles[1][1].Lumi / 1000.

    can = ROOT.TCanvas("Sys_%s_%s_%s_%s" % (probe, match, var, charge), "Systematics", 800, 800)
    can.cd()
    can.SetLeftMargin(0.15)

    can.SetLogy(1)

 

    valuesForYRange = []
    if h_total_sys_sf.InheritsFrom("TGraphAsymmErrors"):
        for i in range (0,Utils.GetNbins(h_total_sys_sf)):
            valuesForYRange.append(h_total_sys_sf.GetY()[i])
        ymin = min(valuesForYRange)
        ymax = max(valuesForYRange)
    else:
        ymin = h_total_sys_sf.GetMinimum()
        ymax = h_total_sys_sf.GetMaximum()
        
    h_styling.Draw("AXIS")
    h_styling.SetMinimum(1.0e-3)
    h_styling.SetMaximum(ymax*1e3)


    h_total_sys_sf.SetLineColor(ROOT.kBlack)
    h_total_sys_sf.GetYaxis().SetTitle("Relative Systematic uncertainty (%)")
    h_total_sys_sf.SetLineWidth(4)
    
    
    h_stat_sf.SetLineColor(ROOT.kMagenta+1)
    h_stat_sf.SetLineWidth(2)
    

    h_truth_sys_sf.SetLineColor(ROOT.kBlue)
    h_truth_sys_sf.SetLineWidth(2)
    h_truth_sys_sf.SetMarkerColor(h_truth_sys_sf.GetLineColor())
    h_truth_sys_sf.SetMarkerStyle(24)
    


    h_dR_sys_sf.SetMarkerStyle(25)
    h_dR_sys_sf.SetLineWidth(2)
    h_dR_sys_sf.SetLineColor(ROOT.kGreen+1)
    h_dR_sys_sf.SetMarkerColor(h_dR_sys_sf.GetLineColor())
    

    h_BG_sys_sf.SetLineWidth(2)
    h_BG_sys_sf.SetMarkerStyle(26)
    h_BG_sys_sf.SetLineColor(ROOT.kRed)
    h_BG_sys_sf.SetMarkerColor(h_BG_sys_sf.GetLineColor())
    

    h_Det_sys_sf.SetLineWidth(2)
    h_Det_sys_sf.SetMarkerStyle(26)
    h_Det_sys_sf.SetLineColor(ROOT.kOrange-3)
    h_Det_sys_sf.SetMarkerColor(h_Det_sys_sf.GetLineColor())
    
    
    if h_stat_sf.InheritsFrom("TGraphAsymmErrors"):
        h_total_sys_sf.Draw("LX same")
        h_stat_sf.Draw("LX same")
        h_truth_sys_sf.Draw("LX same")
        h_dR_sys_sf.Draw("LX same")
        h_BG_sys_sf.Draw("LX same")
        h_Det_sys_sf.Draw("LX same")
    else:
        h_total_sys_sf.Draw("HIST l same")
        h_stat_sf.Draw("HIST l same")
        h_truth_sys_sf.Draw("HIST l same")
        h_dR_sys_sf.Draw("HIST l same")
        h_BG_sys_sf.Draw("HIST l same")
        h_Det_sys_sf.Draw("HIST l same")

    leg = ROOT.TLegend(.45, .68, .95, .85)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextFont(42)
    leg.SetNColumns(2)
    leg.AddEntry(h_truth_sys_sf, "Truth Closure", "l")
    leg.AddEntry(h_dR_sys_sf, "Match #DeltaR", "l")
    leg.AddEntry(h_BG_sys_sf, "Background", "l")
    leg.AddEntry(h_Det_sys_sf, "MC14 vs MC15", "l")
    leg.AddEntry(h_stat_sf, "Statistics", "l")
    leg.AddEntry(h_total_sys_sf, "Total", "l")
    leg.Draw()


    pu.DrawAtlas(can.GetLeftMargin(), 0.91)
    pu.DrawLumiSqrtS(can.GetLeftMargin(), 0.87)
    pu.DrawTarget(match, 1 - can.GetRightMargin(), 0.92, 31)
    pu.DrawSource(probe, 1 - can.GetRightMargin(), 0.87, 31)
    if charge != None:
        qlabel = 'charge: %s' % (charge)
        pu.DrawTLatex(1 - can.GetRightMargin(), 0.82, qlabel, size=pu.Size, align=31)
    histlabel = HistoKinds.SF
    pu.DrawTLatex(can.GetLeftMargin(), 0.82, histlabel, size=pu.Size)

    if charge != None:
        can.SaveAs ("Plots/Sys_%s_%s_%s_%s_%s.pdf" % (probe, match, region, var, charge))
    else:
        can.SaveAs ("Plots/Sys_%s_%s_%s_%s.pdf" % (probe, match, region, var))

    pu.DrawTLatex(0.5,0.94,"Systematics %s vs %s for %s probes in %s"%(match,var,probe,region),0.04,52,22)
    can.SaveAs("Plots/AllEffPlots.pdf")
    
if __name__ == "__main__":

    # what we want to run on:
    InputData = DSConfig.Zmumu_mc15
    InputZmumu = DSConfig.Zmumu_mc15

    dummy = ROOT.TCanvas("dummy","dummy",800,800)
    dummy.SaveAs("Plots/AllEffPlots.pdf[")
    
    # variables we want to plot:
    # NOTE: 'fineEtaPhi' is 1D while 'etaphiFine' is 2D

    
    #######
    # systematics plots:
    #######
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "eta", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "fineEtaPhi", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True, doPreRecEtaCopy=True)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.noCrack, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "eta", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "fineEtaPhi", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True, doPreRecEtaCopy=True)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.noCrack, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)

    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "eta", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "fineEtaPhi", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True, doPreRecEtaCopy=True)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.noCrack, "pt", infiles=[["MC",InputZmumu],["Data",InputData]],charge=None,corr = True)

    
    
    #######
    # efficiency plots:
    #######
    #doEffPlots ("phi", Probes.Calo, Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("eta", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.LooseNoCalo,DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.LooseNoCalo,DetRegions.noCrack, InputData, InputZmumu)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaTemp", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu, includeSys = False, doPreRecEtaCopy=False)
    #doEffPlots ("fineEtaPhi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu, doPreRecEtaCopy=False)
    
    #doEffPlots ("phi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("eta", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = False)
    #doEffPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.Crack, InputData, InputZmumu)
    #doEffPlots ("DetRegions", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaphiFine", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = False)
    #doEffPlots ("CaloTag2D", Probes.MStoMu, Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = False)

    doEffPlots ("phi", Probes.Calo, Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("eta", Probes.Calo, Matches.Medium,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("pt", Probes.Calo, Matches.Medium,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("pt", Probes.Calo, Matches.Medium,DetRegions.noCrack, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("DetRegions", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("etaTemp", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco)
    doEffPlots ("etaphiFine", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, includeSys = False, doPreRecEtaCopy=False, analysis=Analysis.Z_Reco)
    doEffPlots ("fineEtaPhi", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, doPreRecEtaCopy=False, analysis=Analysis.Z_Reco)
    
    #doEffPlots ("phi", Probes.Calo, Matches.Tight, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("eta", Probes.Calo, Matches.Tight,DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.Tight,DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.Tight,DetRegions.noCrack, InputData, InputZmumu)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaTemp", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu, includeSys = False, doPreRecEtaCopy=False)
    #doEffPlots ("fineEtaPhi", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu, doPreRecEtaCopy=False)


    dummy.SaveAs("Plots/AllEffPlots.pdf]")
