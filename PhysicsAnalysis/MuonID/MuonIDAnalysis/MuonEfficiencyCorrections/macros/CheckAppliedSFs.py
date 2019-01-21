# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT
from array import array
import argparse
import sys
from PlotUtils import PlotUtils
import gc
import os, math

class DiagnosticHisto(object):
    def __init__(self, name = "",
                       axis_title = "",
                       bins = -1,
                       min = 0.,
                       max = 0.,
                       bin_width = -1,
                       dir = None):
        self.__name = name
        self.__xTitle = axis_title
        self.__min = min
        self.__width = bin_width
        self.__entries = 0
        self.__content = {}
        self.__error = {}
        self.__TH1 = None
        self.__TDir = dir
        if bins > 0:
            self.__width = (max-min)/ bins 
            self.__TH1 = ROOT.TH1D(name, "Diagnostic histogram", bins, min, max)
            self.__TH1.SetDirectory(dir) 
                
    def name(self): return self.__name
    def TH1(self): return self.__TH1
    def fill(self, value, weight=1.):
        self.__entries +=1
        if self.__TH1: self.__TH1.Fill(value, weight)  
        else:
            rng = self.__getInterval(value)
            try: self.__content[rng] += weight
            except: self.__content[rng] = weight            
            try: self.__error[rng] += weight**2
            except: self.__error[rng] = weight**2
    def __getInterval(self,value):
        i =  math.ceil( (value - self.__min) / self.__width)
        return (self.__min +self.__width*(i-1), self.__min+ self.__width*i)
    def write(self):
        self.fixHisto()
        if not self.__TH1: return            
        self.__TH1.GetXaxis().SetTitle(self.__xTitle)
        self.__TH1.Scale(1./ (self.__TH1.Integral()if self.__entries > 0 else 1.))
        self.__TH1.GetYaxis().SetTitle("Normalized unit area (%.2f)"%(self.__width))
        self.__TH1.SetEntries(self.__entries)
        if self.__TDir: self.__TDir.WriteObject(self.__TH1, self.__name)
    def max(self):
        try: return sorted(self.__content.iterkeys(), key=lambda Rng: Rng[1])[-1][1]
        except: return float('nan')
    def min(self):
        try: return sorted(self.__content.iterkeys(), key=lambda Rng: Rng[0])[0][1]
        except: return float('nan')
    def setMinimum(self, minimum): self.__min = minimum
    def setMaximum(self, maximum): self.__max = maximum
    def fixHisto(self):
        if self.__TH1: return
        bins = int((self.__max - self.__min) / self.__width)
        self.__TH1 = ROOT.TH1D(self.name(), "Diagnostic histogram", bins, self.__min, self.__max)
        self.__TH1.SetDirectory(self.__TDir) 
        for rng in self.__content.iterkeys():
            bin = self.__TH1.FindBin((rng[1] + rng[0])/2.)
            self.__TH1.SetBinContent(bin, self.__content[rng])
            self.__TH1.SetBinError(bin, math.sqrt(self.__error[rng]))

KnownWPs = {
    "Loose" : "RECO",
    "Medium" : "RECO",
    "Tight" : "RECO",
    "HighPt" : "RECO",
    "LowPt" : "RECO",
    "TTVA" : "TTVA",
    "FixedCutLooseIso" : "ISO",
    "LooseTrackOnlyIso" : "ISO",
    "LooseIso" : "ISO",
    "GradientIso" : "ISO",
    "GradientLooseIso" : "ISO",
    "FixedCutTightTrackOnlyIso" : "ISO",
    "FixedCutHighPtTrackOnlyIso" : "ISO",
    "FixedCutTightIso" : "ISO",
    "BadMuonVeto_HighPt" : "BADMUON",
    }

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
    parser.add_argument('-o', '--outDir', help='Specify a destination directory', default="Plots")
    parser.add_argument('-l', '--label', help='Specify the dataset you used with MuonEfficiencyCorrectionsSFFilesTest', default="361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu")
    parser.add_argument('-w', '--WP', help='Specify a WP to plot', nargs='+', default=[])
    parser.add_argument('--varType', help='Specify a variation type', nargs='+', default=["", "MUON_EFF_RECO_SYS__1down", "MUON_EFF_RECO_STAT__1down", "MUON_EFF_RECO_SYS__1up" ])
    parser.add_argument('-c', '--SFConstituent', help='Specify if you want to plot nominal value, sys or stat error', nargs='+', default=["SF","DataEff","MCEff"])
    parser.add_argument('--bonusname', help='Specify a bonus name for the filename', default="")
    parser.add_argument('--bonuslabel', help='Specify a bonus label printed in the histogram', default="")
    parser.add_argument('--noComparison', help='do not plot comparison to old release', action='store_true', default=False)
    parser.add_argument('-n', '--nBins', help='specify number of bins for histograms', type=int, default=15)
    Options = parser.parse_args()

    if not os.path.exists(Options.InputFile):
        print 'ERROR: File %s does not exist!'%Options.InputFile
        sys.exit(1)
    infile  = ROOT.TFile(Options.InputFile)
    
    tree = infile.Get("MuonEfficiencyTest")

    branchesInFile = []
    for key in tree.GetListOfBranches():
        branchesInFile.append(key.GetName())
    calibReleases = []
    allWPs = []
    for wp in KnownWPs.iterkeys():
        if not wp in allWPs: allWPs.append(wp)
    WPs = []
    for i in branchesInFile:
        if not i.startswith("c"): continue
        if not i.endswith("SF"): continue
        calibCand = i[1:-3]
        beststr = ""
        for wp in allWPs:
            wpstr = "_"+wp
            if not wpstr in calibCand: continue
            if calibCand.rfind(wpstr) < calibCand.rfind(beststr): beststr = wpstr
        if len(beststr) > 0:
            if not beststr[1:] in WPs: WPs.append(beststr[1:])
            if not calibCand.replace(beststr,"") in calibReleases: calibReleases.append(calibCand.replace(beststr,""))
    
    print "INFO: Found the following working points: %s"%(",".join(WPs))
    if len(calibReleases)==2: print "INFO: Found the following calibration releases to compare: %s"%(",".join(calibReleases))
    
    if len(Options.WP)>0:
        userWPs = []
        for wp in Options.WP:
            if wp in WPs: userWPs.append(wp)
        WPs = userWPs
        print 'INFO: WPs given by user, only plot: %s'%(",".join(WPs))

        
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetStyle("ATLAS")
    ROOT.gROOT.SetBatch(1)
    gc.disable()
    
    if os.path.isdir(Options.outDir) == False:
        os.system("mkdir -p %s"%(Options.outDir))
    
    bonusname=Options.bonusname

    Histos = {}
    for CR in calibReleases:
        Histos[CR] = {}
        for wp in WPs:
            Histos[CR][wp] = {}
            for t in Options.SFConstituent:
                Histos[CR][wp][t] = {}
                for var in Options.varType:
                    histoname = "%s_%s_%s_%s"%(CR,wp,t,var)
                    if var != "":
                        Histos[CR][wp][t][var] = ROOT.TH1D(histoname,histoname,Options.nBins,0,0.15)
                    else:
                        Histos[CR][wp][t][var] = DiagnosticHisto(name = histoname,
                                                                axis_title = t,
                                                                min = 1.,
                                                                bin_width = 0.0001)

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        if math.fabs(tree.Muon_eta) > 2.5 or tree.Muon_pt < 15.e3: continue
        
        for CR in calibReleases:
            for wp in WPs:
                for t in Options.SFConstituent:
                    for var in Options.varType:
                        myvar = ""
                        if var != "":
                            try:
                                contentVar = getattr(tree, "c%s_%s_%s__%s"%(CR,wp,t,var.replace("RECO",KnownWPs[wp])))
                                contentNom = getattr(tree, "c%s_%s_%s"%(CR,wp,t))
                            except:
                                continue
                            if contentNom==0.: continue
                            Histos[CR][wp][t][var].Fill( math.fabs(contentVar-contentNom)/contentNom )
                        else:
                            Histos[CR][wp][t][var].fill(getattr(tree, "c%s_%s_%s"%(CR,wp,t)))
        
    pu = PlotUtils()
    pu.Size = 18
    
    if len(calibReleases)==2:

        dummy = ROOT.TCanvas("dummy", "dummy", 800, 600)
        dummy.SaveAs("%s/AllAppliedSFCheckPlots%s.pdf[" % (Options.outDir, bonusname))
        
        can = ROOT.TCanvas("calibcomparison%s"%(bonusname),"SFCheck",1000,600)
        can.SetLogy()
        for wp in WPs:
            for t in Options.SFConstituent:
                for var in Options.varType:
                    corrType = "Scale Factor"
                    if t == "DataEff": corrType = "Data efficiency"
                    elif t == "MCEff": corrType = "MC efficiency"
                    if var=="": 
                        minimum = min(Histos[calibReleases[0]][wp][t][var].min(), Histos[calibReleases[1]][wp][t][var].min()) - 0.01
                        maximum = max(Histos[calibReleases[0]][wp][t][var].max(), Histos[calibReleases[1]][wp][t][var].max()) + 0.01
                        Histos[calibReleases[0]][wp][t][var].setMinimum(minimum)
                        Histos[calibReleases[1]][wp][t][var].setMinimum(minimum)
                        Histos[calibReleases[0]][wp][t][var].setMaximum(maximum)
                        Histos[calibReleases[1]][wp][t][var].setMaximum(maximum)
                        Histos[calibReleases[0]][wp][t][var].fixHisto()
                        Histos[calibReleases[1]][wp][t][var].fixHisto()
                        histoCR1 = Histos[calibReleases[0]][wp][t][var].TH1()
                        histoCR2 = Histos[calibReleases[1]][wp][t][var].TH1()
                        histoCR1.GetXaxis().SetTitle(corrType)
                    else:
                        histoCR1 = Histos[calibReleases[0]][wp][t][var]
                        histoCR2 = Histos[calibReleases[1]][wp][t][var]
                        histoCR1.GetXaxis().SetTitle("Relative systematic uncertainty")
                    # get mean before overflow pull
                    histoCR1.SetTitle("%s, Mean: %.8f"%(calibReleases[0],histoCR1.GetMean()))
                    histoCR2.SetTitle("%s, Mean: %.8f"%(calibReleases[1],histoCR2.GetMean()))
                    if var!="": 
                        # pull overflow
                        histoCR1.SetBinContent(histoCR1.GetNbinsX(),histoCR1.GetBinContent(histoCR1.GetNbinsX()+1))
                        histoCR2.SetBinContent(histoCR2.GetNbinsX(),histoCR2.GetBinContent(histoCR2.GetNbinsX()+1))
                        
                    histoCR1.Draw("HIST")
                    histoCR1.SetMinimum(0.01)
                    histoCR1.SetMaximum(max(histoCR1.GetMaximum(), histoCR2.GetMaximum()) *1.e2)
                    
                    histoCR1.GetYaxis().SetTitle("Fraction of muons")
                    histoCR2.SetLineColor(ROOT.kRed)
                    histoCR2.SetMarkerColor(ROOT.kRed)
                    histoCR2.SetLineStyle(9)
                    histoCR2.Draw("sameHIST")
                    pu.DrawLegend([(histoCR1,'L'),(histoCR2,'L')], 0.3, 0.75, 0.9, 0.9)
                    variationDrawn = "Nominal"
                    if "STAT" in var: variationDrawn = "|Stat-Nominal|/Nominal"
                    elif "SYS" in var: variationDrawn = "|Sys-Nominal|/Nominal"
                    pu.DrawTLatex(0.55, 0.5, Options.bonuslabel)
                    pu.DrawTLatex(0.55, 0.55, "WP: %s, %s"%(wp,variationDrawn))
                    pu.DrawTLatex(0.55, 0.6, corrType)
                    can.SaveAs("%s/AppliedSFCheck_%s_%s_%s%s.pdf"%(Options.outDir, wp,t,var,bonusname))
                    can.SaveAs("%s/AllAppliedSFCheckPlots%s.pdf" % (Options.outDir, bonusname))

        dummy.SaveAs("%s/AllAppliedSFCheckPlots%s.pdf]" % (Options.outDir, bonusname))
        
    else:
        print "INFO: Currently, only release comaparisons are implemented"
       
    sys.exit(1) 
        
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
