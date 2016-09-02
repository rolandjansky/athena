# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import Histos
import ROOT
import DSConfig
import EffPlots
import Utils
from Defs import *
from PlotUtils import PlotUtils
import sys

histos = []
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
    varinput = var
    if "_negq" in var:
        varinput = var.split("_negq")[0]
        charge = "neg"
    if "_posq" in var:
        varinput = var.split("_posq")[0]
        charge = "pos"
        
    h_styling = HistoDefs.initHisto(varinput)[0].Clone("stylingHisto")
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
    eff_axis_ranges = []
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
    sf_axis_ranges = []
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
        
        
    if (not corr) and (probe == Probes.ID or probe == Probes.Calo):
        corrstr = "_noIDCorr"
    can.SaveAs ("Plots/SFValid_%s_%s_%s_%s%s.pdf"%(match,var,probe,region,corrstr))
    p1.cd()
    ROOT.gPad.RedrawAxis()

    pu.DrawTLatex(0.5,0.94,"%s Effi%s vs %s for %s probes in %s"%(match,corrstr,var,probe,region),0.04,52,22)

    can.SaveAs("Plots/AllFancyPlots.pdf")

def doEvolutionPlots (var, probe, match, region, conf_Data, conf_Zmumu, LumiScale = [1000], includeSys = True):
    
    # for adjusting the evolution plots' filenames
    global nEventsFileNameInt
    nEventsFileNameInt = 0
    for l in LumiScale:
        nEventsFileNameInt = nEventsFileNameInt+1
        EffPlots.doEffPlots (var, probe, match, region, conf_Data, conf_Zmumu, LumiScale=l, includeSys = includeSys)
    #print sf_axis_ranges, eff_axis_ranges
    del sf_axis_ranges[:]
    del eff_axis_ranges[:]

def doValidPlots (var, probe, match, region, conf_Data, conf_Zmumu, conf_Closure, corr = True, includeSys = True):

    infiles = [
                   ["MC",conf_Zmumu],
                   ["Data",conf_Data]
                   ]
    infiles2 = [
                   ["MC",conf_Closure],
                   ["Data",conf_Data]
                   ]

    hists_final = []

    charge = None
    varinput = var
    if "_negq" in var:
        varinput = var.split("_negq")[0]
        charge = "neg"
    if "_posq" in var:
        varinput = var.split("_posq")[0]
        charge = "pos"
        
    hists_noCL = Histos.TPFinalSysHistos("test",probe,match,region,varinput,infiles,charge=charge,corr=corr,doClosure=False).HistoSets
    hists_CL = Histos.TPFinalSysHistos("test",probe,match,region,varinput,infiles2,charge=charge,corr=corr,doClosure=True).HistoSets
    syshisto = None
    doSFValidation1D ([hists_noCL["Nominal"], hists_CL["Nominal"]], var, probe, match, region, conf_Data, corr, syshisto)
    
    
if __name__ == "__main__":

    #ROOT.gROOT.Macro("rootlogon.C")
    #ROOT.gROOT.SetStyle("ATLAS")
    #ROOT.gROOT.SetBatch(1)
    # what we want to run on:
    InputData = DSConfig.Data_15_firstWeek
    #InputDataOld = DSConfig.Data_DRAW
    InputClosure = DSConfig.Zmumu_r6633_Closure
    InputZmumu = DSConfig.Zmumu_r6633

    dummy = ROOT.TCanvas("dummy","dummy",800,800)
    dummy.SaveAs("Plots/AllFancyPlots.pdf[")
    
    #######
    # evolution plots:
    #######
    # for evolution plots, choose steps of maximal events
    #myMaxEvents=[float(lumi)/ InputData.Lumi for lumi in range(10,101)] 
    myMaxEvents=[10. / InputData.Lumi]
    #doEvolutionPlots ("pt", Probes.Calo,Matches.Medium, DetRegions.noCrack, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("eta", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("phi", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("DetRegions", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents, includeSys = True)
    #doEvolutionPlots ("fineEtaPhi", Probes.Calo,Matches.Medium, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)

    #doEvolutionPlots ("pt", Probes.Calo,Matches.LooseNoCalo, DetRegions.noCrack, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("eta", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("phi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("DetRegions", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("fineEtaPhi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)

    #doEvolutionPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.noCrack, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("eta", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("phi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("DetRegions", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)
    #doEvolutionPlots ("fineEtaPhi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData, InputZmumu,LumiScale=myMaxEvents)


   
    #######
    # validation plots (closure test):
    #######   
    doValidPlots ("DetRegions", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("DetRegions_posq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("DetRegions_negq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_posq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_negq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("eta", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("phi", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.Calo,Matches.LooseNoCalo, DetRegions.noCrack, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    
    
    #doValidPlots ("DetRegions", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_posq", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_negq", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("eta", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("phi", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.MStoMu,Matches.Calo, DetRegions.noCrack, InputData,InputZmumu, InputClosure, includeSys = False)
    
    
    doValidPlots ("DetRegions", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("DetRegions_posq", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("DetRegions_negq", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("phi", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("eta", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    
    
    #doValidPlots ("pt", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.Calo,Matches.Medium, DetRegions.noCrack, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("etaTemp", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    
    #doValidPlots ("DetRegions", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    ##doValidPlots ("DetRegions_posq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    ##doValidPlots ("DetRegions_negq", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("eta", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("etaTemp", Probes.Calo,Matches.LooseNoCalo, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
     
    doValidPlots ("DetRegions", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("DetRegions_posq", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("DetRegions_negq", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("phi", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    doValidPlots ("eta", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("etaTemp", Probes.Calo,Matches.Tight, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    
    
    #doValidPlots ("fineEtaPhi", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_posq", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("fineEtaPhi_negq", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("eta", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("phi", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.Calo,Matches.Medium, DetRegions.noCrack, InputData,InputZmumu, InputClosure, includeSys = False)
    #doValidPlots ("pt", Probes.Calo,Matches.Medium, DetRegions.All, InputData,InputZmumu, InputClosure, includeSys = False)

    # very slow due to complex binning
    #doEffPlots ("etaphiFineEurope", Probes.ID,Matches.Loose, DetRegions.All, InputData, InputZmumu, includeSys = False)
    
    dummy.SaveAs("Plots/AllFancyPlots.pdf]")
    
    