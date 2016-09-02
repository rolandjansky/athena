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

def draw2D (histo, var, probe, match, region, conf_Data, histoKind, corr, LumiScale, syshisto, analysis):
    
    pu = PlotUtils()
    pu.Size = 0.06*(1-pu.VerticalCanvasSplit)
    pu.Lumi = conf_Data.Lumi

    can = ROOT.TCanvas("Eff_%s_%s_%s_%s_%s"%(match,var,probe,region,histoKind.replace(" ","")),"Efficiencies",800,600)
    can.cd()

    histos.append(can)
    histo.GetZaxis().SetTitle(histoKind)

    if HistoKinds.SF in histoKind:
        ROOT.gStyle.SetPalette(55)
        histo.GetZaxis().SetRangeUser(0.85,1.15)
    else:
        ROOT.gStyle.SetPalette(54)
        histo.GetZaxis().SetRangeUser(0.80,1.01)

    if syshisto != None:
        for i in range(0, Utils.GetNbins(syshisto)):
            #print 'sys: \t%s \tstat: \t%s'%(syshisto.GetBinContent(i),histo.GetBinError(i))
            # WARNING: Calling TH2Poly.SetBinError results in very strange things...
            if not histo.InheritsFrom("TH2Poly"):
                histo.SetBinError(i, math.sqrt(histo.GetBinError(i)**2+syshisto.GetBinContent(i)**2))
        histo.SetMarkerSize(0.4)
        histo.Draw("colz l E TEXT")
    else:       
        histo.Draw("colz l")

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

def doEffPlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, syshisto = None, corr = True, analysis=Analysis.Z_Reco):
    
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
    
    baseVar = var.replace("_posq","").replace("_negq","")
    
                  
        
    ######
    # used for styling histo (when TGraphAsymmErrors are used)
    h_styling = HistoDefs.initHisto(baseVar)[0].Clone("stylingHisto")
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
    if baseVar == "DetRegions":
    # for week-1 data (DetRegions plot)
        h_styling.GetXaxis().SetLabelOffset(1.3 * h_styling.GetXaxis().GetLabelOffset())
        #h_styling.GetXaxis().SetRangeUser(1.5,11.5)
        for i in range(1,Utils.GetNbins(h_styling)-1):
            h_styling.GetXaxis().SetBinLabel(i,"")
        h_styling.GetXaxis().SetTitle("")

        h_styling_ratio.GetXaxis().SetLabelOffset(6.0 * h_styling_ratio.GetXaxis().GetLabelOffset())
        #h_styling_ratio.GetXaxis().SetRangeUser(1.5,12.5)
        h_styling_ratio.GetXaxis().SetTitleOffset(h_styling_ratio.GetXaxis().GetTitleOffset()*1.2)
    ##############

    pu = PlotUtils()
    pu.Size = 0.06
    pu.Lumi = conf_Data.Lumi
    #pu.SqrtS = 8

    # for evolution plots
    nEventsStr = ""
    if LumiScale != -1:
        nEventsStr = ("_%.3fipb"%LumiScale).replace(".","pt")
    if baseVar == "DetRegions":
        can,p1,p2 = pu.Prepare2PadCanvas("Eff_%s_%s_%s_%s%s"%(match,var,probe,region,nEventsStr),width=800, height=600)
    else:
        can,p1,p2 = pu.Prepare2PadCanvas("Eff_%s_%s_%s_%s%s"%(match,var,probe,region,nEventsStr),width=800, height=600)
    
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
    if match == Matches.HighPt:
        h_styling.GetYaxis().SetRangeUser(0.001,1.001)
    #if baseVar == "DetRegions":
        
    h_styling.Draw("AXIS")
    if h_mc.InheritsFrom("TGraphAsymmErrors"):
        h_mc.Draw("sameP")
    else:
        h_mc.Draw("same")

    if h_ref == None:
        #if h_truth.InheritsFrom("TGraphAsymmErrors"):
            #h_truth.Draw("sameP")
        #else:
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

    extraGap = 0.
    if "pt" in var:
        extraGap = 0.1
    pu.DrawAtlas(0.19+extraGap,0.4)
    if LumiScale != -1:
        lumi=LumiScale*InputData.Lumi
        pu.DrawLumiSqrtSEvolution(0.19+extraGap,0.32,lumi=lumi)
    else:
        pu.DrawLumiSqrtS(0.19+extraGap,0.32)
    if h_ref != None:
        # for comparing Matches.ID to Matches.ID_noMCP
        if match in Matches.ID:
            pu.DrawLegend([(h_data,"PL"), (h_ref,"PL"), (h_mc,"PL"), (h_ref_mc,"PL")],0.69,0.08,0.89,0.33)
        else:
            pu.DrawLegend([(h_data,"PL"), (h_ref,"PL"), (h_mc,"PL")],0.69,0.08,0.89,0.33)
    else:
        #pu.DrawLegend([(h_data,"PL"), (h_mc,"PL"), (h_truth,"PL")],0.69,0.08,0.89,0.33)
        pu.DrawLegend([(h_data,"PL"), (h_mc,"PL")],0.69,0.32,1-ROOT.gPad.GetRightMargin(),0.47)
    
    #pu.DrawSource(probe,0.69,0.36)
    pu.DrawTarget(match,0.19+extraGap,0.24)
    #pu.DrawTLatex(0.19+extraGap, 0.18,"Runs 267638, 267639",0.06)
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        pu.DrawTLatex(0.19+extraGap,0.16,"ID eff. #font[62]{not} applied")
    if "_posq" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"q > 0",0.06)
    elif "_negq" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"q < 0",0.06)
    if "_Aside" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"#eta > 0",0.06)
    elif "_Cside" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"#eta < 0",0.06)
    if "_pt30" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"p_{T} > 30 GeV",0.06)
    if "_pt70" in var:
        pu.DrawTLatex(0.19+extraGap,0.08,"p_{T} > 70 GeV",0.06)
    pu.DrawRegionLabel(region,0.19+extraGap,0.12)

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
                ratioWithTotalSys.SetPointEYhigh(i,math.sqrt(ratioWithTotalSys.GetErrorYhigh(i)**2.+(ratioWithTotalSys.GetY()[i]*syshisto.GetY()[i])**2.))
                ratioWithTotalSys.SetPointEYlow(i,math.sqrt(ratioWithTotalSys.GetErrorYlow(i)**2.+(ratioWithTotalSys.GetY()[i]*syshisto.GetY()[i])**2.))
                #Currently not used in SetFancyAxisRanges_SF
                ratioWithTotalSysAxis.SetPoint(i,ratioWithTotalSysAxis.GetX()[i],ratio.GetY()[i]+ratioWithTotalSys.GetErrorYlow(i))
            else:
                ratioWithTotalSys.SetBinError(i,math.sqrt(ratioWithTotalSys.GetBinError(i)*ratioWithTotalSys.GetBinError(i)+ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)*ratioWithTotalSys.GetBinContent(i)*syshisto.GetBinContent(i)))
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

    p2.SetGridy()
    #if baseVar == "DetRegions":
    # for week-1 data (DetRegions plot)
        #h_styling_ratio.GetXaxis().SetLabelOffset(2.4 * h_styling_ratio.GetXaxis().GetLabelOffset())
        #h_styling_ratio.GetXaxis().SetRangeUser(1.5,12.5)
        #h_styling_ratio.GetXaxis().SetTitleOffset(h_styling_ratio.GetXaxis().GetTitleOffset()*0.7)
    ##############
    h_styling_ratio.Draw("AXIS")
    p2.SetGridy()
    if syshisto != None:
        ratioWithTotalSys.SetFillColor(ROOT.kOrange-3)
        ratioWithTotalSys.SetLineColor(ROOT.kOrange-3)
        if h_mc.InheritsFrom("TGraphAsymmErrors"):
            ratioWithTotalSys.Draw("same2")
            ratio.Draw("same2")
        else:
            ratioWithTotalSys.Draw("E2")
            p2.SetGridy()
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
    ratio.SetTitle("Stat only")
    ratioWithTotalSys.SetTitle("Sys #oplus Stat")
    # draw lines in ratio plot
    firstbin = h_styling_ratio.GetXaxis().GetBinLowEdge(1)
    #if "DetR" in var:
        #firstbin = h_styling_ratio.GetXaxis().GetBinLowEdge(2)
    lastbin = h_styling_ratio.GetXaxis().GetBinUpEdge(h_styling_ratio.GetNbinsX())
    l=ROOT.TLine(firstbin,1,lastbin,1)
    l.SetLineWidth(1)
    l.SetLineStyle(ROOT.kDashed)
    l.Draw()
    l2=ROOT.TLine(firstbin,1+2./3. * (h_styling_ratio.GetMaximum()-1),lastbin,1+2./3. * (h_styling_ratio.GetMaximum()-1))
    l2.SetLineWidth(1)
    l2.SetLineStyle(ROOT.kDotted)
    l2.Draw()
    l3=ROOT.TLine(firstbin,1-2./3. * (h_styling_ratio.GetMaximum()-1),lastbin,1-2./3. * (h_styling_ratio.GetMaximum()-1))
    l3.SetLineWidth(1)
    l3.SetLineStyle(ROOT.kDotted)
    l3.Draw()
    
        
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
    
    p1.cd()
    lege = pu.DrawLegend([(ratio,"F"),(ratioWithTotalSys,"F")],0.4,0.00,0.75,0.1)
    lege.SetNColumns(2)
    lege.SetColumnSeparation(0.002)
    
    can.SaveAs ("Plots/Eff_%s_%s_%s_%s%s%s.pdf"%(match,var,probe,region,corrstr,nEventsFileName))
    can.SaveAs ("Plots/Eff_%s_%s_%s_%s%s%s.png"%(match,var,probe,region,corrstr,nEventsFileName))
    can.SaveAs ("Plots/Eff_%s_%s_%s_%s%s%s.eps"%(match,var,probe,region,corrstr,nEventsFileName))


    pu.DrawTLatex(0.5,0.94,"%s Effi%s vs %s for %s probes in %s %s"%(match,corrstr,var,probe,region,nEventsStr),0.04,52,22)

    can.SaveAs("Plots/AllEffPlots.pdf")
    
    
    #h_data = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    #h_mc = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    #h_truth = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency]
    #ratio = hists_final[0].Histos[HistoKinds.SF][HistoKinds.SF]
    #ratiotruth = hists_final[0].Histos[HistoKinds.SF][HistoKinds.TruthSF]
    
      
    
    #direc = outfile.mkdir("Eff_%s_%s_%s_%s"%(probe,match,var,region))
    #direc.cd()
    #h_data.Write("h_data")
    #h_mc.Write("h_mc")
    #h_truth.Write("h_truth")
    #ratio.Write("ratio")
    #ratiotruth.Write("ratiotruth")

    #outfile.cd()

    
def doEffPlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, syshistos, corr = True, analysis=Analysis.Z_Reco):


    h_data = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_truth = hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency]

    ratiodata = hists_final[0].Histos[HistoKinds.SF][HistoKinds.SF]
    ratiotruth = hists_final[0].Histos[HistoKinds.SF][HistoKinds.TruthSF]
    
    if not h_data.InheritsFrom("TGraphAsymmErrors"):
        h_data.SetDirectory(0)
        h_mc.SetDirectory(0)
        h_truth.SetDirectory(0)
        ratiodata.SetDirectory(0)
        ratiotruth.SetDirectory(0)

    if syshistos != None:
        data_sys = syshistos[HistoKinds.DataEfficiency]
        mc_sys = syshistos[HistoKinds.MCEfficiency]
        sf_sys = syshistos[HistoKinds.SF]
        if not h_data.InheritsFrom("TGraphAsymmErrors"):
            data_sys.SetDirectory(0)
            mc_sys.SetDirectory(0)
            sf_sys.SetDirectory(0)
        draw2D (h_data, var, probe, match, region, conf_Data, HistoKinds.DataEfficiency, corr, LumiScale, data_sys, analysis)
        draw2D (h_mc, var, probe, match, region, conf_Data, HistoKinds.MCEfficiency, corr, LumiScale, mc_sys, analysis)
        draw2D (ratiodata, var, probe, match, region, conf_Data, HistoKinds.SF, corr, LumiScale, sf_sys, analysis)
    else:
        draw2D (h_data, var, probe, match, region, conf_Data, HistoKinds.DataEfficiency, corr, LumiScale, None, analysis)
        draw2D (h_mc, var, probe, match, region, conf_Data, HistoKinds.MCEfficiency, corr, LumiScale, None, analysis)
        draw2D (ratiodata, var, probe, match, region, conf_Data, HistoKinds.SF, corr, LumiScale, None, analysis)

    draw2D (h_truth, var, probe, match, region, conf_Data, HistoKinds.TruthEfficiency, corr, LumiScale, None, analysis)
    draw2D (ratiotruth, var, probe, match, region, conf_Data, HistoKinds.TruthSF, corr, LumiScale, None, analysis)

def doEffPlots (var, probe, match, region, conf_Data, conf_Zmumu, inputComparison = None, corr = True, includeSys = True, doClosure = False, LumiScale=-1, doPreRecEtaCopy=False, analysis=Analysis.Z_Reco, backgrounds={}):

    infiles = [
                   ["MC",conf_Zmumu],
                   ["Data",conf_Data]
                   ]
    for k,v in backgrounds.iteritems():
        infiles.append( [k,v])

    hists_final = []

    # for pre-recommendations, copy negative eta bins of transition region to positive ones (use doPreRecEtaCopy=True):
    if includeSys:
        # with systematic uncertainty bands:
        charge = None
        varinput = var
        if "_negq" in var:
            varinput = var.split("_negq")[0]
            charge = "neg"
        if "_posq" in var:
            varinput = var.split("_posq")[0]
            charge = "pos"
        test = Histos.TPFinalSysHistos("test",probe,match,region,varinput,infiles,charge=charge,corr=corr,doClosure=doClosure,LumiScale=LumiScale,doPreRecEtaCopy=doPreRecEtaCopy,analysis=analysis).HistoSets
        syshisto = test[Systematics.All][HistoKinds.SF]
        hists_final.append(test["Nominal"])
    else:
        # faster (without systematic uncertainty bands):
        hists_final.append(Histos.TPFinalHistos("test",probe,match,region,var,infiles,corr=corr,doClosure=doClosure,LumiScale=LumiScale,doPreRecEtaCopy=doPreRecEtaCopy,analysis=analysis))
        syshisto = None


    if inputComparison != None:
        infiles2 = [
                   ["MC",conf_Zmumu],
                   ["Data",inputComparison]
                   ]
        hists_final.append(Histos.TPFinalHistos(inputComparison.Label,probe,match,region,var,infiles2,corr=corr,doClosure=doClosure,analysis=analysis))

    if hists_final[0].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].InheritsFrom("TH2"):
        syshistos = None
        if syshisto != None:
            syshistos = test[Systematics.All]
        doEffPlots2D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, syshistos, corr, analysis=analysis)
    else:
        doEffPlots1D (hists_final, var, probe, match, region, conf_Data, conf_Zmumu, LumiScale, syshisto, corr, analysis=analysis)


    
if __name__ == "__main__":

    # what we want to run on:
    InputData = DSConfig.Data_15_50ns
    InputZmumu = DSConfig.Zmumu_r6630
    
    backgrounds = {
        "ttbar":DSConfig.Ttbar_r6633,
        "Ztautau":DSConfig.Ztautau_r6633,
        #"Wplus":DSConfig.Wplus_r6633,
        #"Wminus":DSConfig.Wminus_r6633,
        }
 
    dummy = ROOT.TCanvas("dummy","dummy",800,600)
    dummy.SaveAs("Plots/AllEffPlots.pdf[")
    
    # variables we want to plot:
    # NOTE: 'fineEtaPhi' is 1D while 'etaphiFine' is 2D
    
    #######
    # efficiency plots:
    #######
    # for pre-recommendations, copy negative eta bins of transition region to positive ones (use doPreRecEtaCopy=True):

    #doEffPlots ("phi", Probes.Calo, Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("eta", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.LooseNoCalo,DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("pt", Probes.Calo, Matches.LooseNoCalo,DetRegions.noCrack, InputData, InputZmumu)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu, includeSys = False, doPreRecEtaCopy=False)
    #doEffPlots ("fineEtaPhi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu, doPreRecEtaCopy=False)
    
    #doEffPlots ("phi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = True, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.Crack, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    ##doEffPlots ("etaphiFine", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = False)
    ##doEffPlots ("CaloTag2D", Probes.MStoMu, Matches.Calo, DetRegions.All, InputData, InputZmumu, includeSys = False)

    #doEffPlots ("phi", Probes.Calo, Matches.Loose, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta", Probes.Calo, Matches.Loose,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Loose,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Loose,DetRegions.noCrack, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.Loose, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.Loose, DetRegions.All, InputData, InputZmumu, includeSys = False, doPreRecEtaCopy=True, analysis=Analysis.Z_Reco,backgrounds=backgrounds)

    #doEffPlots ("phi", Probes.Calo, Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta", Probes.Calo, Matches.Medium,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Medium,DetRegions.All, InputData, InputZmumu,analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Medium,DetRegions.noCrack, InputData, InputZmumu,analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    doEffPlots ("DetRegions", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)


    #doEffPlots ("phi", Probes.Calo, Matches.Tight, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta", Probes.Calo, Matches.Tight,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Tight,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.Calo, Matches.Tight,DetRegions.noCrack, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.Tight, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    

    #doEffPlots ("phi", Probes.MStoID, Matches.ID, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta", Probes.MStoID, Matches.ID,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.MStoID, Matches.ID,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt", Probes.MStoID, Matches.ID,DetRegions.noCrack, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions", Probes.MStoID,Matches.ID, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphiFine", Probes.MStoID,Matches.ID, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)

    #doEffPlots ("phi_pt70", Probes.Calo, Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta_pt70", Probes.Calo, Matches.HighPt,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt_pt70", Probes.Calo, Matches.HighPt,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("pt_pt70", Probes.Calo, Matches.HighPt,DetRegions.noCrack, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions_pt70", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions_pt30", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("phi_pt30", Probes.Calo, Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("eta_pt30", Probes.Calo, Matches.HighPt,DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("DetRegions_pt30", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaTemp", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaTemp_pt30", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphi", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds)
    #doEffPlots ("etaphiFine", Probes.Calo,Matches.HighPt, DetRegions.All, InputData, InputZmumu, analysis=Analysis.Z_Reco,backgrounds=backgrounds, includeSys = False)


    dummy.SaveAs("Plots/AllEffPlots.pdf]")
