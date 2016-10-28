# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
from array import array
import argparse
import sys
from PlotUtils import PlotUtils
import gc
import os

def GetProbeMatchFromType(type):
    probe = ""
    match = ""
    if type == "Medium":
        probe = "CaloProbes"
        match = "MediumMuons"
    elif type == "Loose":
        probe = "CaloProbes"
        match = "LooseMuons_noCaloTag"
    elif type == "Tight":
        probe = "CaloProbes"
        match = "TightMuons"
    elif type == "HighPt":
        probe = "CaloProbes"
        match = "HighPtMuons"
    elif type == "TTVA":
        probe = "LooseProbes_noProbeIP"
        match = "MediumMuons"
    return probe,match

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(description='This script checks applied scale factors written to a file by MuonEfficiencyCorrections/MuonEfficiencyCorrectionsSFFilesTest. For more help type \"python CheckAppliedSFs.py -h\"', prog='CheckAppliedSFs', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--InputFile', help='Specify an input root file', default="Applied_SFs.root")
    parser.add_argument('-l', '--label', help='Specify the dataset you used with MuonEfficiencyCorrectionsSFFilesTest', default="341505.PowhegPythia8EvtGen_ggH125_ZZ4lep_noTau")
    parser.add_argument('-t', '--SFType', help='Specify a scale factor type', nargs='+', default=["Medium","Loose","Tight","HighPt","TTVA"])
    parser.add_argument('-c', '--SFConstituent', help='Specify if you want to plot nominal value, sys or stat error', nargs='+', default=["SF","Sys","Stat"])
    parser.add_argument('-b', '--bonusname', help='Specify a bonusname if you want a special histogram', default="")
    parser.add_argument('--doComparison', help='plot comparison to old release', action='store_true', default=False)
    Options = parser.parse_args()

    infile  = ROOT.TFile(Options.InputFile)
    
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

    for type in Options.SFType:
        for const in Options.SFConstituent:
            probe,match = GetProbeMatchFromType(type)

            period = "2015+16"

            pu = PlotUtils()
            pu.Size = 18
            
            bonusstrForHist = bonusstr
            if "_NoHighEta" in bonusstr and type == "TTVA":
                bonusstrForHist = bonusstr.replace("_NoHighEta","")
            elif "_HighEta" in bonusstr and type == "TTVA":
                bonusstrForHist = bonusstr.replace("_HighEta","")
            can = ROOT.TCanvas("%s%s_%s%s"%(compString,type,const,bonusstr),"SFCheck",800,600)
            can.SetLogy()
            hist = infile.Get("%s%s_%s%s"%(compString,type,const,bonusstrForHist))
            if not hist:
                print 'ERROR: Histogram "%s%s_%s%s" not found!'%(compString,type,const,bonusstrForHist)
                sys.exit(1)
            if hist.GetBinContent(-1) != 0. or hist.GetBinContent(hist.GetNbinsX()+1)!=hist.GetBinContent(hist.GetNbinsX()+2):
                print 'ERROR: Histogram filled correctly??'
                sys.exit(1)
            if hist.GetBinContent(0) != 0.:
                print 'ERROR: %s %s has underflow bin content: %s'%(type,const,hist.GetBinContent(0))
                sys.exit(1)
            if hist.GetBinContent(hist.GetNbinsX()+1) != 0.:
                print 'WARNING: %s %s has overflow bin content: %s'%(type,const,hist.GetBinContent(hist.GetNbinsX()+1))
            minimum = -1.
            maximum = -1.
            for i in range(1,hist.GetNbinsX()+1):
                if hist.GetBinContent(i)!=0:
                    if minimum == -1.:
                        minimum = hist.GetXaxis().GetBinCenter(i)
                    maximum = hist.GetXaxis().GetBinCenter(i)
                
        
            bla = array('d',[ 0.5,0.9,0.95,0.99])
            q = array('d',[0,0,0,0])

            hist.GetQuantiles(4, q, bla)
            avg = hist.GetMean()
            hist.Scale(1./hist.Integral())
            hist.Draw()
            
            if Options.doComparison:
                hist.GetXaxis().SetRangeUser(0.0,max(0.05,maximum*1.04))
            else:
                if not const == "SF":
                    if type == "TTVA":
                        hist.GetXaxis().SetRangeUser(0,0.03)
                    else:
                        hist.GetXaxis().SetRangeUser(0,0.1)
                else:
                    hist.GetXaxis().SetRangeUser(0.5,1.5)
                
            kinematicslabel = "p_{T}>5GeV, |#eta|<2.7"
            if bonusstr == "_pt15":
                kinematicslabel = "p_{T}>15GeV, |#eta|<2.7"
            elif bonusstr == "_NoHighEta":
                kinematicslabel = "p_{T}>5GeV, |#eta|<2.5"
            elif bonusstr == "_HighEta":
                kinematicslabel = "p_{T}>5GeV, |#eta|>2.5"
                
            pu.DrawTLatex(0.58, 0.7, kinematicslabel)
            pu.DrawTarget(match, 0.58, 0.75)
            typelabel = " uncertainties"
            if Options.doComparison:
                typelabel = " uncertainty comparisons"
            if const == "SF":
                typelabel = "s"  
                if Options.doComparison:
                    typelabel = " comparisons"  
            pu.DrawTLatex(0.26, 0.88, "#it{%s} Scale factor%s for 2015+2016 dataset (DS2)"%(type,typelabel))
            pu.DrawTLatex(0.26, 0.81, "Muons from %s"%Options.label)
            
            pu.DrawTLatex(0.58, 0.33, 'Minimum value: %.6f%%'%(minimum*100))
            pu.DrawTLatex(0.58, 0.29, 'Maximum value: %.6f%%'%(maximum*100))
            
            if Options.doComparison or not const == "SF":
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

            can.SaveAs("Plots/AppliedSFCheck_%s%s_%s_%s%s.pdf"%(compString,type,const,period,bonusstr))
            can.SaveAs("Plots/AllAppliedSFCheckPlots%s%s.pdf" % (compString,bonusstr))
        
    dummy.SaveAs("Plots/AllAppliedSFCheckPlots%s%s.pdf]" % (compString,bonusstr))
