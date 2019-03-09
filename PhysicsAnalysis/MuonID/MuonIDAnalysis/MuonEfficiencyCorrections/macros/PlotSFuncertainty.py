# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT
from array import array
import argparse, sys
from PlotUtils import PlotUtils, DiagnosticHisto
from CheckAppliedSFs import  KnownWPs, getArgParser, getCalibReleasesAndWP
import gc
import os, math

class SystDependency(object):
    def __init__(self,
                 var_name = "", axis_title ="",
                 bins = -1, bmin = 0., bmax = 0.,
                 bdir = None,                 
                 calib = "",
                 wp = "",
                 sys = "SYS",
                 test_tree = None,
                 log_binning = False,
                 ):
        ### Direct access to the branch which are going to be compared
        self.__muon_var  = test_tree.GetLeaf("Muon_%s"%(var_name))
        self.__norm = 1. if var_name.find("pt") == -1 else 1./1.e3
        
        self.__var_name  = "%s%s_%s_%s"%("" if len(calib) == 0 else calib+"_", wp,var_name, sys)
        self.__wp = wp
        self.__sys = sys
        self.__calib = calib
        self.__nom_sf = test_tree.GetLeaf("%s%s_SF"%("" if len(calib) == 0 else "c%s_"%(calib), wp))
        self.__1up_sf = test_tree.GetLeaf("%s%s_SF__MUON_EFF_%s_%s__1up"%("" if len(calib) == 0 else "c%s_"%(calib), wp,KnownWPs[wp],sys))
        self.__1dn_sf = test_tree.GetLeaf("%s%s_SF__MUON_EFF_%s_%s__1down"%("" if len(calib) == 0 else "c%s_"%(calib), wp,KnownWPs[wp],sys))
        self.__nom_histo = DiagnosticHisto(
                                    name = "%s"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir,
                                    log_binning = log_binning)
        self.__1up_histo = DiagnosticHisto(
                                    name = "%s_1UP"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir,
                                    log_binning = log_binning )
        self.__1dn_histo = DiagnosticHisto(
                                    name = "%s_1UP"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir,
                                    log_binning = log_binning )
                
    def fill(self):
        self.__nom_histo.fill(value = self.__muon_var.GetValue()*self.__norm, weight= self.__nom_sf.GetValue())
        self.__1up_histo.fill(value = self.__muon_var.GetValue()*self.__norm, weight= self.__1up_sf.GetValue())
        self.__1dn_histo.fill(value = self.__muon_var.GetValue()*self.__norm, weight= self.__1dn_sf.GetValue())     
        
   
    def finalize(self):
         self.__nom_histo.write()
         self.__1dn_histo.write()
         self.__1up_histo.write()
         
    def name(self):
        return self.__var_name
   
    def get_nom_H1(self): return  self.__nom_histo 
    def get_1up_H1(self): return  self.__1up_histo 
    def get_1dn_H1(self): return  self.__1dn_histo 
    
    def get_wp(self): return self.__wp
    def get_sys(self): return self.__sys
    def get_calib(self): return self.__calib
  
if __name__ == "__main__":
    Options = getArgParser().parse_args()

    if not os.path.exists(Options.InputFile):
        print 'ERROR: File %s does not exist!'%Options.InputFile
        sys.exit(1)
    infile  = ROOT.TFile(Options.InputFile)
    
    tree = infile.Get("MuonEfficiencyTest")
    calibReleases, WPs = getCalibReleasesAndWP(tree)
   
    if len(Options.WP)>0:
        WPs = [wp for wp in   Options.WP if wp in WPs]
        print 'INFO: WPs given by user, only plot: %s'%(",".join(WPs))

        
    ROOT.gROOT.Macro("rootlogon.C")
    ROOT.gROOT.SetStyle("ATLAS")
    ROOT.gROOT.SetBatch(1)
    gc.disable()
    
    if os.path.isdir(Options.outDir) == False:
        os.system("mkdir -p %s"%(Options.outDir))
    
    bonusname=Options.bonusname

    Histos = []
    
    for calib in calibReleases:
        for wp in WPs:
            for sys in ["SYS", "STAT"]:
                Histos +=[
                SystDependency(
                    var_name = "pt", 
                    axis_title ="p_{T} #mu(%s) [GeV]"%(wp),
                    calib = calib, log_binning = True,
                    bins = 100, bmin = 15, bmax = 3000,
                    wp =wp, sys = sys, test_tree = tree),
            SystDependency(
                    var_name = "eta", 
                    axis_title ="#eta #mu(%s)"%(wp),
                    calib = calib,
                    bins =25 , bmin = -2.5, bmax = 2.5,
                    wp =wp, sys = sys, test_tree = tree),
            SystDependency(
                    var_name = "phi", 
                    axis_title ="#phi (%s)"%(wp),
                    calib = calib,
                    bins =20 , bmin = -3.15, bmax = 3.15,
                    wp =wp, sys = sys, test_tree = tree) ]

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        if i > 0 and i % 5000 == 0: 
            print "INFO: %d/%d events processed"%(i, tree.GetEntries())          
        if math.fabs(tree.Muon_eta) > 2.5  or tree.Muon_pt < 15.e3: continue
        for H in Histos:  
             if tree.Muon_isHighPt == True or  H.name().find("HighPt") == -1: H.fill()
        
    print "INFO: Histograms filled"  
    dummy = ROOT.TCanvas("dummy", "dummy", 800, 600)
    dummy.SaveAs("%s/AllSystDep%s.pdf[" % (Options.outDir, bonusname))
    for H in Histos:
        pu = PlotUtils(status = Options.label)
        pu.Prepare1PadCanvas(H.name())
       
        nom = H.get_nom_H1().TH1()
        up = H.get_1dn_H1().TH1()
        dn = H.get_1up_H1().TH1()
        
        up.SetLineColor(ROOT.kRed)
        dn.SetLineColor(ROOT.kBlue)
                            
        up.Divide(nom)
        dn.Divide(nom)
        
                    
        up.SetTitle("+1#sigma %s"%("systematic" if H.get_sys() == "SYS" else "statistics"))
        dn.SetTitle("-1#sigma %s"%("systematic" if H.get_sys() == "SYS" else "statistics"))
       
        nom.GetYaxis().SetTitle("Ratio to nominal")
        pu.drawStyling(nom, min([up.GetMinimum(0.8), dn.GetMinimum(0.8)])*0.8, 
                            max([up.GetMaximum(), dn.GetMaximum()])*1.1, TopPad = False)
        
        up.Draw("SAMEHIST")
        dn.Draw("HISTSAME")
       
        pu.CreateLegend(0.7, 0.8, 0.9, 0.9)
        pu.AddToLegend([up, dn])
        
        pu.DrawAtlas(0.2, 0.9)
        pu.DrawSqrtS(0.2, 0.85)
        pu.DrawTLatex(0.2,0.8, "%s, %s"%(H.get_wp(), H.get_calib()))
        
        pu.DrawLegend()
     
        pu.saveHisto("%s/SysCheck%s%s"%(Options.outDir, H.name(),bonusname) ,["pdf"])
        pu.saveHisto("%s/AllSystDep%s" % (Options.outDir, bonusname),["pdf"])

    dummy.SaveAs("%s/AllSystDep%s.pdf]" % (Options.outDir, bonusname))
        
   
       
