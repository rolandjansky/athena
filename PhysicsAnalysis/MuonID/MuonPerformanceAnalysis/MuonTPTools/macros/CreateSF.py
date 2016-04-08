# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import Histos
import Utils
import ROOT
import DSConfig
import Utils
from Defs import *
ROOT.gROOT.Macro("rootlogon.C")
ROOT.gROOT.SetStyle("ATLAS")
ROOT.gROOT.SetBatch(1)
from array import array
from PlotUtils import PlotUtils

hists_final = []

# this creates a 3d binning, with x = fineetaphi, y = pt (constant), z = charge (based on posq and negq histos)
def build_3d_composite(period, raw_posq, raw_negq, jpsibins_low, jpsibins_up, jpsisys=[], isMC=False):

    bins_for_hist = [abin for abin in jpsibins_low+[jpsibins_up[-1]]]
    nptbins = len(bins_for_hist)-1
    ptbins = array('d',bins_for_hist)

    fineetaphibins = []
    for i in range(1,raw_posq.GetNbinsX()+1):
        fineetaphibins.append(raw_posq.GetXaxis().GetBinLowEdge(i))
    fineetaphibins.append(raw_posq.GetXaxis().GetBinUpEdge(raw_posq.GetNbinsX()))
    fineetaphibins = array('d',fineetaphibins)

    qbins = [-2.,0.,2.]
    nqbins = len(qbins)-1
    qbins = array('d',qbins)

    mcNaming = ""
    if isMC:
        mcNaming = "_MC"
    # then create the histo
    if len(jpsisys)>0:
        histoname = "%s_sys_3dized_%s%s"%(raw_posq.GetName(),period,mcNaming)
    else:
        histoname = "%s_3dized_%s%s"%(raw_posq.GetName(),period,mcNaming)
    h3d = ROOT.TH3F(histoname,raw_posq.GetTitle(),raw_posq.GetNbinsX(),fineetaphibins,nptbins,ptbins,nqbins,qbins)

    # and fill it!
    if len(jpsisys)>0:
        for t in range(1,h3d.GetNbinsX()+1):
            sys_p = raw_posq.GetBinContent(t)
            sys_n = raw_negq.GetBinContent(t)
            for u in range(1,h3d.GetNbinsY()+1):
                h3d.SetBinContent(t,u,h3d.GetZaxis().FindBin(-1.),ROOT.sqrt(sys_n*sys_n+ROOT.pow(jpsisys[u-1],2)))
                h3d.SetBinError(t,u,h3d.GetZaxis().FindBin(-1.),ROOT.sqrt(sys_n*sys_n+ROOT.pow(jpsisys[u-1],2)))
                h3d.SetBinContent(t,u,h3d.GetZaxis().FindBin(1.),ROOT.sqrt(sys_p*sys_p+ROOT.pow(jpsisys[u-1],2)))
                h3d.SetBinError(t,u,h3d.GetZaxis().FindBin(1.),ROOT.sqrt(sys_p*sys_p+ROOT.pow(jpsisys[u-1],2)))

    else:
        for t in range(1,h3d.GetNbinsX()+1):
            val_p = raw_posq.GetBinContent(t)
            err_p = raw_posq.GetBinError(t)
            val_n = raw_negq.GetBinContent(t)
            err_n = raw_negq.GetBinError(t)
            for u in range(1,h3d.GetNbinsY()+1):
                h3d.SetBinContent(t,u,h3d.GetZaxis().FindBin(-1.),val_n)
                h3d.SetBinError(t,u,h3d.GetZaxis().FindBin(-1.),err_n)
                h3d.SetBinContent(t,u,h3d.GetZaxis().FindBin(1.),val_p)
                h3d.SetBinError(t,u,h3d.GetZaxis().FindBin(1.),err_p)

    return h3d



def createPeriod (outfile,period,probe,match,Data,MC):

    ptlow = [0., 4.5, 7., 10., 15.]
    ptup = [4.5, 7., 10., 15., 1e15]
    jpsisys = [0.06, 0.015, 0.005, 0.0025, 0.00]
    
    # for pre-recommendations, copy negative eta bins of transition region to positive ones (use doPreRecEtaCopy=True):
    testpos = Histos.TPFinalSysHistos("testpos",probe,match,DetRegions.All,"fineEtaPhi",infiles=[["MC",MC],["Data",Data]],charge="pos",corr = True,doPreRecEtaCopy=False,useAsymEff=True).HistoSets
    testneg = Histos.TPFinalSysHistos("testneg",probe,match,DetRegions.All,"fineEtaPhi",infiles=[["MC",MC],["Data",Data]],charge="neg",corr = True,doPreRecEtaCopy=False,useAsymEff=True).HistoSets

    # fill data and MC efficiency and SF
    h_data_eff_p = testpos["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc_eff_p = testpos["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_sf_p = testpos["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF]

    data_eff_sys_1D_p = testpos[Systematics.All][0]
    mc_eff_sys_1D_p = testpos[Systematics.All][1]
    sf_sys_1D_p = testpos[Systematics.All][2]

    h_data_eff_n = testneg["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc_eff_n = testneg["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_sf_n = testneg["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF]

    data_eff_sys_1D_n = testneg[Systematics.All][0]
    mc_eff_sys_1D_n = testneg[Systematics.All][1]
    sf_sys_1D_n = testneg[Systematics.All][2]

    # convert TGraphAsymmErrors to TH1 for scale factor files
    if h_data_eff_p.InheritsFrom("TGraphAsymmErrors"):
        h_data_eff_p = Utils.ConvertTGraphAsymmErrors(h_data_eff_p)
        h_mc_eff_p = Utils.ConvertTGraphAsymmErrors(h_mc_eff_p)
        h_sf_p = Utils.ConvertTGraphAsymmErrors(h_sf_p)
        data_eff_sys_1D_p = Utils.ConvertTGraphAsymmErrors(data_eff_sys_1D_p)
        mc_eff_sys_1D_p = Utils.ConvertTGraphAsymmErrors(mc_eff_sys_1D_p)
        sf_sys_1D_p = Utils.ConvertTGraphAsymmErrors(sf_sys_1D_p)
    if h_data_eff_n.InheritsFrom("TGraphAsymmErrors"):
        h_data_eff_n = Utils.ConvertTGraphAsymmErrors(h_data_eff_n)
        h_mc_eff_n = Utils.ConvertTGraphAsymmErrors(h_mc_eff_n)
        h_sf_n = Utils.ConvertTGraphAsymmErrors(h_sf_n)
        data_eff_sys_1D_n = Utils.ConvertTGraphAsymmErrors(data_eff_sys_1D_n)
        mc_eff_sys_1D_n = Utils.ConvertTGraphAsymmErrors(mc_eff_sys_1D_n)
        sf_sys_1D_n = Utils.ConvertTGraphAsymmErrors(sf_sys_1D_n)


    data_eff_3d = build_3d_composite(period, h_data_eff_p, h_data_eff_n, ptlow, ptup)
    mc_eff_3d = build_3d_composite(period, h_mc_eff_p, h_mc_eff_n, ptlow, ptup, isMC=True)
    sf_3d = build_3d_composite(period, h_sf_p, h_sf_n, ptlow, ptup)
    data_eff_sys_3d = build_3d_composite(period, data_eff_sys_1D_p, data_eff_sys_1D_n, ptlow, ptup, jpsisys)
    mc_eff_sys_3d = build_3d_composite(period, mc_eff_sys_1D_p, mc_eff_sys_1D_n, ptlow, ptup, jpsisys, isMC=True)
    sf_sys_3d = build_3d_composite(period, sf_sys_1D_p, sf_sys_1D_n, ptlow, ptup, jpsisys)

    # the systematics tool provides relative errors, so multiply
    data_eff_sys_3d.Multiply(data_eff_3d)
    mc_eff_sys_3d.Multiply(mc_eff_3d)
    sf_sys_3d.Multiply(sf_3d)

    # assign axis names that the SF tool knows how to interpret
    data_eff_3d.GetXaxis().SetTitle("fine etaphi bin")
    data_eff_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    data_eff_3d.GetZaxis().SetTitle("charge")

    mc_eff_3d.GetXaxis().SetTitle("fine etaphi bin")
    mc_eff_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    mc_eff_3d.GetZaxis().SetTitle("charge")

    sf_3d.GetXaxis().SetTitle("fine etaphi bin")
    sf_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    sf_3d.GetZaxis().SetTitle("charge")

    data_eff_sys_3d.GetXaxis().SetTitle("fine etaphi bin")
    data_eff_sys_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    data_eff_sys_3d.GetZaxis().SetTitle("charge")

    mc_eff_sys_3d.GetXaxis().SetTitle("fine etaphi bin")
    mc_eff_sys_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    mc_eff_sys_3d.GetZaxis().SetTitle("charge")

    sf_sys_3d.GetXaxis().SetTitle("fine etaphi bin")
    sf_sys_3d.GetYaxis().SetTitle("p_{T} [GeV]")
    sf_sys_3d.GetZaxis().SetTitle("charge")

    outfile.WriteTObject(data_eff_3d,"Eff_%s"%(period))
    outfile.WriteTObject(mc_eff_3d,"MC_Eff_%s"%(period))
    outfile.WriteTObject(sf_3d,"SF_%s"%(period))
    outfile.WriteTObject(data_eff_sys_3d,"Eff_sys_%s"%(period))
    outfile.WriteTObject(mc_eff_sys_3d,"MC_Eff_sys_%s"%(period))
    outfile.WriteTObject(sf_sys_3d,"SF_sys_%s"%(period))

def createCTPeriod (outfile,period,probe,Data,MC):

    test = Histos.TPFinalSysHistos("test",probe,Matches.Calo,DetRegions.All,"CaloTag2D",infiles=[["MC",MC],["Data",Data]],charge=None,corr = True).HistoSets
    
    # fill data and MC efficiency and SF
    h_data_eff = test["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
    h_mc_eff = test["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency]
    h_sf = test["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF]

    data_eff_sys_2D = test[Systematics.All][0]
    mc_eff_sys_2D = test[Systematics.All][1]
    sf_sys_2D = test[Systematics.All][2]

    # do not need to convert TGraphAsymmErrors to TH1 for scale factor files since CaloTag2D is 2D and thus not using TGraphAsymmErrors

    # the systematics tool provides relative errors, so multiply
    data_eff_sys_2D.Multiply(h_data_eff)
    mc_eff_sys_2D.Multiply(h_mc_eff)
    sf_sys_2D.Multiply(h_sf)

    # assign axis names that the SF tool knows how to interpret
    h_data_eff.GetXaxis().SetTitle("#eta")
    h_data_eff.GetYaxis().SetTitle("p_{T} [GeV]")

    h_mc_eff.GetXaxis().SetTitle("#eta")
    h_mc_eff.GetYaxis().SetTitle("p_{T} [GeV]")

    h_sf.GetXaxis().SetTitle("#eta")
    h_sf.GetYaxis().SetTitle("p_{T} [GeV]")
    
    data_eff_sys_2D.GetXaxis().SetTitle("#eta")
    data_eff_sys_2D.GetYaxis().SetTitle("p_{T} [GeV]")
    mc_eff_sys_2D.GetXaxis().SetTitle("#eta")
    mc_eff_sys_2D.GetYaxis().SetTitle("p_{T} [GeV]")
    sf_sys_2D.GetXaxis().SetTitle("#eta")
    sf_sys_2D.GetYaxis().SetTitle("p_{T} [GeV]")
    
    outfile.WriteTObject(h_data_eff,"Eff_%s"%(period))
    outfile.WriteTObject(h_mc_eff,"MC_Eff_%s"%(period))
    outfile.WriteTObject(h_sf,"SF_%s"%(period))
    outfile.WriteTObject(data_eff_sys_2D,"Eff_sys_%s"%(period))
    outfile.WriteTObject(mc_eff_sys_2D,"MC_Eff_sys_%s"%(period))
    outfile.WriteTObject(sf_sys_2D,"SF_sys_%s"%(period))

    

if __name__ == "__main__":

    # --------------------------
    # In principle all you have to setup is defined here
    # --------------------------
    # MC input file
    InputZmumu = DSConfig.Zmumu_mc15

    TypesToRun = [
                ["Medium",Probes.Calo, Matches.Medium],
                ["Tight",Probes.Calo, Matches.Tight],
                ["Loose",Probes.Calo, Matches.LooseNoCalo],
                ["Calo",Probes.MStoMu, Matches.Calo],
                 ]

    periods = [
                ["All",20322.,DSConfig.Data_periodB]
              ]

    # name of the overall period the SF file is about
    filePeriod = "2015"
    ## --------------------------


    for Type in TypesToRun:

        muonTypeName = Type[0]
        Probe = Type[1]
        Match = Type[2]
        if Match == Matches.Calo:
            ofile = ROOT.TFile("CaloTag_%s_SF.root"%(filePeriod),"RECREATE")
        else: 
            ofile = ROOT.TFile("MuonsChain_%s_%s_SF.root"%(muonTypeName,filePeriod),"RECREATE")
        
        ofile.cd()

        # Create luminosity tree
        LumiDataTree = ROOT.TTree('LumiData','Lumi Data')

        # Create branches in the tree
        period = ROOT.std.string("init")
        LumiDataTree.Branch("Period",period)
        lumi = array( 'f', [0] * len(periods) )
        LumiDataTree.Branch( 'IntLumi', lumi, 'IntLumi/F' )

        for p in periods:
            period = ROOT.std.string(p[0])
            lumi[0] = p[1]
            LumiDataTree.SetBranchAddress("Period",period)
            LumiDataTree.Fill()

            DataInput = p[2]
            # get the actual histograms per perdiod/runNumber
            if Match == Matches.Calo:
                createCTPeriod(outfile=ofile,period=period,probe=Probe,Data=DataInput,MC=InputZmumu)
            else:
                createPeriod(outfile=ofile,period=period,probe=Probe,match=Match,Data=DataInput,MC=InputZmumu)
            

        ofile.WriteTObject(LumiDataTree)
        ofile.Close()

