# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT
from array import array
import argparse
import sys
from PlotUtils import PlotUtils
import gc
import os

def GetProbeMatchFromType(Type,WP):
    probe = ""
    match = ""
    if Type == "Reco":
        probe = "CaloProbes"
        if WP == "Medium": match = "MediumMuons"
        elif WP == "Loose": match = "LooseMuons_noCaloTag"
        elif WP == "Tight": match = "TightMuons"
        elif WP == "HighPt": match = "HighPtMuons"
    elif Type == "TTVA":
        probe = "LooseProbes_noProbeIP"
        match = "MediumMuons"
    elif Type == "BadMuonVeto":
        if WP == "HighPt":
            probe = "HighPtMuons"
            match = "HighPtMuons_PassVeto"
    return probe,match

def GetTypeAndWP(options,histo):
    Type = 'UNKNOWN'
    WP = 'UNKNOWN'
    for itype in options.SFType:
        if '_%s'%(itype) in histo:
            WP = itype.replace('Reco','').replace('Iso','',1).replace('TTVA','').replace('BadMuonVeto','')
            Type = itype.replace(WP,'')
    return Type,WP

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(description='This script checks applied scale factors written to a file by MuonEfficiencyCorrections/MuonEfficiencyCorrectionsSFFilesTest. For more help type \"python CheckAppliedSFs.py -h\"', prog='CheckAppliedSFs', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--InputFile', help='Specify an input root file', default="Applied_SFs.root")
    parser.add_argument('-l', '--label', help='Specify the dataset you used with MuonEfficiencyCorrectionsSFFilesTest', default="361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu")
    parser.add_argument('-t', '--SFType', help='Specify a scale factor type', nargs='+', default=["RecoMedium","RecoLoose","RecoTight","RecoHighPt","TTVALoose","BadMuonVetoHighPt","IsoGradientIso"])
    parser.add_argument('-c', '--SFConstituent', help='Specify if you want to plot nominal value, sys or stat error', nargs='+', default=["SF","DataEff","MCEff"])
    parser.add_argument('-b', '--bonusname', help='Specify a bonusname if you want a special histogram', default="")
    parser.add_argument('--doComparison', help='plot comparison to old release', action='store_true', default=False)
    Options = parser.parse_args()

    if not os.path.exists(Options.InputFile):
        print 'ERROR: File %s does not exist!'%Options.InputFile
        sys.exit(1)
    infile  = ROOT.TFile(Options.InputFile)

    Histograms = []
    for key in infile.GetListOfKeys():
        Histograms.append(key.GetName())
    
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetStyle("ATLAS")
    ROOT.gROOT.SetBatch(1)
    gc.disable()
    
    if os.path.isdir("Plots") == False:
        os.system("mkdir -p Plots")
    
    bonusstr=Options.bonusname
    
    compString = ""
    if Options.doComparison:
        compString = "Comparison_"
        bonusstr = "_NoHighEta"
    
    dummy = ROOT.TCanvas("dummy", "dummy", 800, 600)
    dummy.SaveAs("Plots/AllAppliedSFCheckPlots%s%s.pdf[" % (compString,bonusstr))

    for histo in sorted(Histograms):
        found = False
        for itype in Options.SFType:
            if itype in histo: found = True
        if not found: continue
        found = False
        myConst = ''
        for const in Options.SFConstituent:
            if const in histo: 
                found = True
                myConst = const
        if not found: continue
    
        myType,myWP = GetTypeAndWP(Options,histo)
        probe,match = GetProbeMatchFromType(myType,myWP)
        
        # do not look at systematic variations directly, instead use the relative comparison to nominal value
        if not 'rel' in histo and histo != '%s_%s%s'%(myConst,myType,myWP): continue

        pu = PlotUtils()
        pu.Size = 18
            
        bonusstrForHist = bonusstr
        if "_NoHighEta" in bonusstr and myType == "TTVA":
            bonusstrForHist = bonusstr.replace("_NoHighEta","")
        elif "_HighEta" in bonusstr and myType == "TTVA":
            bonusstrForHist = bonusstr.replace("_HighEta","")
        can = ROOT.TCanvas("%s%s"%(histo,bonusstr),"SFCheck",800,600)
        can.SetLogy()
        hist = infile.Get(histo)
        hist.GetYaxis().SetTitle('Fraction of muons')
        if not hist:
            print 'ERROR: Histogram "%s" not found!'%(histo)
            sys.exit(1)
        if hist.GetBinContent(-1) != 0. or hist.GetBinContent(hist.GetNbinsX()+1)!=hist.GetBinContent(hist.GetNbinsX()+2):
            print 'ERROR: Histogram filled correctly??'
            # sys.exit(1)
        if hist.GetBinContent(0) != 0.:
            print 'ERROR: %s has underflow bin content: %s'%(histo,hist.GetBinContent(0))
            # sys.exit(1)
        if hist.GetBinContent(hist.GetNbinsX()+1) != 0.:
            print 'WARNING: %s has overflow bin content: %s'%(histo,hist.GetBinContent(hist.GetNbinsX()+1))
        minimum = -1.
        maximum = -1.
        for i in range(1,hist.GetNbinsX()+1):
            if hist.GetBinContent(i)!=0:
                if minimum == -1.:
                    minimum = hist.GetXaxis().GetBinCenter(i)
                maximum = hist.GetXaxis().GetBinCenter(i)
                
        
        bla = array('d',[ 0.5,0.9,0.95,0.99])
        q = array('d',[0,0,0,0])
        
        if hist.Integral() == 0.:
            print 'ERROR: Histogram %s is empty, skipping...'%histo
            continue
        hist.GetQuantiles(4, q, bla)
        avg = hist.GetMean()
        hist.Scale(1./hist.Integral())
        hist.Draw()
            
        if Options.doComparison:
            hist.GetXaxis().SetRangeUser(0.0,max(0.05,maximum*1.04))
        else:
            if "rel" in histo:
                suffix = ' |up-nom|'
                if '__1down' in histo: suffix = ' |down-nom|'
                hist.GetXaxis().SetTitle(hist.GetXaxis().GetTitle()+suffix)
                if myType == "TTVA":
                    hist.GetXaxis().SetRangeUser(0,0.1)
                else:
                    hist.GetXaxis().SetRangeUser(0,0.1)
            else:
                hist.GetXaxis().SetRangeUser(0.5,1.5)
                
        kinematicslabel = "p_{T}>4 GeV, |#eta|<2.7"
        if myType == 'TTVA': kinematicslabel = "p_{T}>4 GeV, |#eta|<2.7"
        if bonusstr == "_pt15":
            kinematicslabel = "p_{T}>15 GeV, |#eta|<2.7"
        elif bonusstr == "_NoHighEta":
                kinematicslabel = "p_{T}>4 GeV, |#eta|<2.5"
        elif bonusstr == "_HighEta":
            kinematicslabel = "p_{T}>4 GeV, |#eta|>2.5"
                
        pu.DrawTLatex(0.58, 0.7, kinematicslabel)
        pu.DrawTarget(match, 0.58, 0.75)
        typelabel = " uncertainties"
        if Options.doComparison:
            typelabel = " uncertainty comparisons"
        if myConst == "SF":
            typelabel = "s"  
            if Options.doComparison:
                typelabel = " comparisons"  
        pu.DrawTLatex(0.26, 0.88, "%s - #it{%s} Scale factor%s for 2015+2016 dataset"%(myType,myWP,typelabel))
        pu.DrawTLatex(0.26, 0.81, "Muons from %s"%Options.label)
            
        pu.DrawTLatex(0.58, 0.33, 'Minimum value: %.6f%%'%(minimum*100))
        pu.DrawTLatex(0.58, 0.29, 'Maximum value: %.6f%%'%(maximum*100))
            
        if Options.doComparison or not myConst == "SF" or "rel" in histo:
            cols = [ROOT.kRed+1, ROOT.kOrange+7, ROOT.kBlue+1, ROOT.kGreen+1]
            moo = []
            tlx = ROOT.TLatex()
            tlx.SetNDC()
            tlx.DrawLatex(0.58, 0.55+0.06, 'Mean: %.1f%%'%(avg*100.))
            for i,val in enumerate(q):
                tlx.SetTextColor(cols[i]) 
                line = ROOT.TLine(val, hist.GetMinimum(), val, hist.GetBinContent(hist.GetXaxis().FindBin(val)))
                line.SetLineWidth(2)
                line.SetLineColor(cols[i])
                line.Draw()
                moo += [line]
                if (i == 0):
                    tlx.DrawLatex(0.58, 0.55-0.06*i, 'Median: %.1f%%'%(val*100.))
                else: 
                    tlx.DrawLatex(0.58, 0.55-0.06*i, '%.0f %% quantile: %.1f%%'%(100.*bla[i],val*100.))

        can.SaveAs("Plots/AppliedSFCheck_%s%s%s.pdf"%(compString,histo,bonusstr))
        can.SaveAs("Plots/AllAppliedSFCheckPlots%s%s.pdf" % (compString,bonusstr))
        
    dummy.SaveAs("Plots/AllAppliedSFCheckPlots%s%s.pdf]" % (compString,bonusstr))
