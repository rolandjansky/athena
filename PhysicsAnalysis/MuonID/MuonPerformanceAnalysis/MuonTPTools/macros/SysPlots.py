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

def doSysPlot(probe,match,region,var,infiles,charge=None,corr = True, doPreRecEtaCopy=False, backgrounds=[]):

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
    
    h_truth_sys_sf = testpos[Systematics.truth][HistoKinds.SF]
    h_dR_sys_sf = testpos[Systematics.dR][HistoKinds.SF]
    h_BG_sys_sf = testpos[Systematics.BG][HistoKinds.SF]
    try:
        h_Charge_sys_sf = testpos[Systematics.Charge][HistoKinds.SF]
    except KeyError:
        h_Charge_sys_sf = h_BG_sys_sf.Clone("Chargething")  
        if h_Charge_sys_sf.InheritsFrom("TGraphAsymmErrors"):
            for i in range (0,Utils.GetNbins(h_Charge_sys_sf)):
                h_Charge_sys_sf.SetPoint(i,0,0)
        else:
            for i in range (0,Utils.GetNbins(h_Charge_sys_sf)+2):
                h_Charge_sys_sf.SetBinContent(i,0)
                h_Charge_sys_sf.SetBinError(i,0)
            
    #h_Det_sys_sf = testpos[Systematics.Det][HistoKinds.SF]
    h_total_sys_sf = testpos[Systematics.All][HistoKinds.SF]

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
            #if i == 0:
                
            h_truth_sys_sf.SetPoint(i,h_truth_sys_sf.GetX()[i],h_truth_sys_sf.GetY()[i]*100.)
            h_dR_sys_sf.SetPoint(i,h_dR_sys_sf.GetX()[i],h_dR_sys_sf.GetY()[i]*100.)
            h_BG_sys_sf.SetPoint(i,h_BG_sys_sf.GetX()[i],h_BG_sys_sf.GetY()[i]*100.)
            h_Charge_sys_sf.SetPoint(i,h_Charge_sys_sf.GetX()[i],h_Charge_sys_sf.GetY()[i]*100.)
            #h_Det_sys_sf.SetPoint(i,h_Det_sys_sf.GetX()[i],h_Det_sys_sf.GetY()[i]*100.)
            h_total_sys_sf.SetPoint(i,h_total_sys_sf.GetX()[i],h_total_sys_sf.GetY()[i]*100.)
            h_stat_sf.SetPoint(i,h_stat_sf.GetX()[i],h_stat_sf.GetY()[i]*100.)
    else:
        for i in range (1,Utils.GetNbins(h_truth_sys_sf)):
            h_truth_sys_sf.SetBinContent(i, h_truth_sys_sf.GetBinContent(i)*100.)
            h_dR_sys_sf.SetBinContent(i, h_dR_sys_sf.GetBinContent(i)*100.)
            h_BG_sys_sf.SetBinContent(i, h_BG_sys_sf.GetBinContent(i)*100.)
            h_Charge_sys_sf.SetBinContent(i, h_Charge_sys_sf.GetBinContent(i)*100.)
            #h_Det_sys_sf.SetBinContent(i, h_Det_sys_sf.GetBinContent(i)*100.)
            h_total_sys_sf.SetBinContent(i, h_total_sys_sf.GetBinContent(i)*100.)
            h_stat_sf.SetBinContent(i, h_stat_sf.GetBinContent(i)*100.)
        
    pu = PlotUtils()
    pu.Size = 0.06 * (1 - pu.VerticalCanvasSplit)
    pu.Lumi = infiles[1][1].Lumi
    #pu.SqrtS = 8

    can = ROOT.TCanvas("Sys_%s_%s_%s_%s" % (probe, match, var, charge), "Systematics", 800, 600)
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
        
    # clean up over/underflow bins
    for g in [h_total_sys_sf,h_stat_sf,h_dR_sys_sf,h_truth_sys_sf,h_Charge_sys_sf,h_BG_sys_sf]:
        if g.InheritsFrom("TGraphAsymmErrors"):
            for p in range(Utils.GetNbins(g)):
                if g.GetX()[p] < h_styling.GetXaxis().GetBinCenter(1):
                    g.SetPoint(p, h_styling.GetXaxis().GetBinLowEdge(1),g.GetY()[p+1])
                if g.GetX()[p] > h_styling.GetXaxis().GetBinCenter(h_styling.GetNbinsX()):
                    g.SetPoint(p, h_styling.GetXaxis().GetBinUpEdge(h_styling.GetNbinsX()),g.GetY()[p-1])
                    
                
        
    h_styling.Draw("AXIS")
    h_styling.SetMinimum(1.0e-3)
    h_styling.SetMaximum(ymax*1e3)


    h_total_sys_sf.SetLineColor(ROOT.kBlack)
    h_total_sys_sf.GetYaxis().SetTitle("Relative Systematic uncertainty (%)")
    h_total_sys_sf.SetLineWidth(4)
    
    
    h_stat_sf.SetLineColor(ROOT.kOrange-3)
    h_stat_sf.SetLineWidth(2)
    

    h_truth_sys_sf.SetLineColor(ROOT.kBlue)
    h_truth_sys_sf.SetLineWidth(2)
    h_truth_sys_sf.SetMarkerColor(h_truth_sys_sf.GetLineColor())
    h_truth_sys_sf.SetMarkerStyle(24)
    


    h_dR_sys_sf.SetMarkerStyle(25)
    h_dR_sys_sf.SetLineWidth(2)
    h_dR_sys_sf.SetLineColor(ROOT.kGreen+1)
    h_dR_sys_sf.SetMarkerColor(h_dR_sys_sf.GetLineColor())
    

    h_Charge_sys_sf.SetLineWidth(2)
    h_Charge_sys_sf.SetMarkerStyle(26)
    h_Charge_sys_sf.SetLineColor(ROOT.kViolet-5)
    h_Charge_sys_sf.SetMarkerColor(h_Charge_sys_sf.GetLineColor())

    h_BG_sys_sf.SetLineWidth(2)
    h_BG_sys_sf.SetMarkerStyle(26)
    h_BG_sys_sf.SetLineColor(ROOT.kRed+1)
    h_BG_sys_sf.SetMarkerColor(h_BG_sys_sf.GetLineColor())
    

    #h_Det_sys_sf.SetLineWidth(2)
    #h_Det_sys_sf.SetMarkerStyle(26)
    #h_Det_sys_sf.SetLineColor(ROOT.kOrange-3)
    #h_Det_sys_sf.SetMarkerColor(h_Det_sys_sf.GetLineColor())
    
    
    if h_stat_sf.InheritsFrom("TGraphAsymmErrors"):
        h_total_sys_sf.Draw("LX same")
        h_stat_sf.Draw("LX same")
        h_truth_sys_sf.Draw("LX same")
        h_dR_sys_sf.Draw("LX same")
        h_Charge_sys_sf.Draw("LX same")
        h_BG_sys_sf.Draw("LX same")
        #h_Det_sys_sf.Draw("LX same")
    else:
        h_total_sys_sf.Draw("HIST l same")
        h_stat_sf.Draw("HIST l same")
        h_truth_sys_sf.Draw("HIST l same")
        h_Charge_sys_sf.Draw("HIST l same")
        h_dR_sys_sf.Draw("HIST l same")
        h_BG_sys_sf.Draw("HIST l same")
        #h_Det_sys_sf.Draw("HIST l same")

    leg = ROOT.TLegend(.42, .65, .92, .82)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextFont(42)
    leg.SetNColumns(2)
    leg.AddEntry(h_truth_sys_sf, "Truth Closure", "l")
    leg.AddEntry(h_dR_sys_sf, "Match #DeltaR", "l")
    leg.AddEntry(h_BG_sys_sf, "Background", "l")
    leg.AddEntry(h_Charge_sys_sf, "Probe Charge", "l")
    #leg.AddEntry(h_Det_sys_sf, "MC14 vs MC15", "l")
    leg.AddEntry(h_stat_sf, "Statistics", "l")
    leg.AddEntry(h_total_sys_sf, "Total", "l")
    leg.Draw()


    pu.DrawAtlas(can.GetLeftMargin()+0.03, 0.90)
    pu.DrawLumiSqrtS(can.GetLeftMargin()+0.03, 0.85)
    pu.DrawTarget(match, 1 - can.GetRightMargin()-0.03, 0.90, 31)
    pu.DrawSource(probe, 1 - can.GetRightMargin()-0.03, 0.85, 31)
    if charge != None:
        qlabel = 'charge: %s' % (charge)
        pu.DrawTLatex(1 - can.GetRightMargin()-0.03, 0.80, qlabel, size=pu.Size, align=31)
    histlabel = HistoKinds.SF
    pu.DrawTLatex(can.GetLeftMargin()+0.03, 0.80, histlabel, size=pu.Size)

    if charge != None:
        can.SaveAs ("Plots/Sys_%s_%s_%s_%s_%s.pdf" % (probe, match, region, var, charge))
    else:
        can.SaveAs ("Plots/Sys_%s_%s_%s_%s.pdf" % (probe, match, region, var))

    pu.DrawTLatex(0.5,0.97,"Systematics %s vs %s for %s probes in %s"%(match,var,probe,region),0.028,52,22)
    can.SaveAs("Plots/AllSysPlots.pdf")
    
    
if __name__ == "__main__":

    # what we want to run on:
    InputData = DSConfig.Data_15_50ns
    InputZmumu = DSConfig.Zmumu_r6630

    dummy = ROOT.TCanvas("dummy","dummy",800,600)
    dummy.SaveAs("Plots/AllSysPlots.pdf[")
    

    
    backgrounds = {
        "ttbar":DSConfig.Ttbar_r6633,
        "Ztautau":DSConfig.Ztautau_r6633,
        #"Wplus":DSConfig.Wplus_r6633,
        #"Wminus":DSConfig.Wminus_r6633,
        }
    infiles=[["MC",InputZmumu],["Data",InputData]]
    for k,v in backgrounds.iteritems():
        infiles.append( [k,v])
 
    #######
    # systematics plots:
    ########
    doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "DetRegions", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "eta", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "phi", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.All, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Medium, DetRegions.noCrack, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    
    #doSysPlot(Probes.Calo,Matches.Loose, DetRegions.All, "DetRegions", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Loose, DetRegions.All, "eta", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Loose, DetRegions.All, "phi", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Loose, DetRegions.All, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Loose, DetRegions.noCrack, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "DetRegions", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "eta", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "phi", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.All, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.Tight, DetRegions.noCrack, "pt", infiles=infiles,charge=None,corr = True)

    #doSysPlot(Probes.MStoMu,Matches.Calo, DetRegions.All, "DetRegions", infiles=infiles,charge=None,corr = True)
    #doSysPlot(Probes.MStoMu,Matches.Calo, DetRegions.All, "eta", infiles=infiles,charge=None,corr = True)
    #doSysPlot(Probes.MStoMu,Matches.Calo, DetRegions.All, "phi", infiles=infiles,charge=None,corr = True)
    #doSysPlot(Probes.MStoMu,Matches.Calo, DetRegions.All, "pt", infiles=infiles,charge=None,corr = True)
    #doSysPlot(Probes.MStoMu,Matches.Calo, DetRegions.Crack, "pt", infiles=infiles,charge=None,corr = True)

    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "DetRegions", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "eta", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "phi", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.All, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    #doSysPlot(Probes.Calo,Matches.LooseNoCalo, DetRegions.noCrack, "pt", infiles=infiles,charge=None,corr = True,backgrounds=backgrounds)
    
    
    dummy.SaveAs("Plots/AllSysPlots.pdf]")
    
    