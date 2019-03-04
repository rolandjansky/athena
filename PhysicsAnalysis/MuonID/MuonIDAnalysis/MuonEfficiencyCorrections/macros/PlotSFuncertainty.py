# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT
from array import array
import argparse, sys
from PlotUtils import PlotUtils, DiagnosticHisto
from CheckAppliedSFs import GetTypeAndWP, KnownWPs, getArgParser
import gc
import os, math

class SystDependency(object):
    def __init__(self,
                 var_name = "", axis_title ="",
                 bins = -1, bmin = 0., bmax = 0.,
                 bdir = None,                 
               
                 wp = "",
                 sys = "SYS",
                 test_tree = None,
                 ):
        ### Direct access to the branch which are going to be compared
        self.__muon_var  = test_tree.GetLeaf("Muon_%s"%(var_name))
        
        self.__var_name  = "%s_%s_%s"%(wp,var_name, sys)
        self.__wp = wp
        self.__sys = sys
        self.__nom_sf = test_tree.GetLeaf("%s_SF"%(wp))
        self.__1up_sf = test_tree.GetLeaf("%s_SF__MUON_EFF_%s_%s__1up"%(wp,KnownWPs[wp],sys))
        self.__1dn_sf = test_tree.GetLeaf("%s_SF__MUON_EFF_%s_%s__1down"%(wp,KnownWPs[wp],sys))
        self.__nom_histo = DiagnosticHisto(
                                    name = "%s"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir)
        self.__1up_histo = DiagnosticHisto(
                                    name = "%s_1UP"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir)
        self.__1dn_histo = DiagnosticHisto(
                                    name = "%s_1UP"%(self.__var_name),
                                    axis_title = axis_title,
                                    bins = bins, bmin = bmin, bmax = bmax, 
                                    bdir = bdir)
                
    def fill(self):
        self.__nom_histo.fill(value = self.__muon_var.GetValue(), weight= self.__nom_sf.GetValue())
        self.__1up_histo.fill(value = self.__muon_var.GetValue(), weight= self.__1up_sf.GetValue())
        self.__1dn_histo.fill(value = self.__muon_var.GetValue(), weight= self.__1dn_sf.GetValue())     
        
   
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
    
  
if __name__ == "__main__":
    Options = getArgParser().parse_args()

    if not os.path.exists(Options.InputFile):
        print 'ERROR: File %s does not exist!'%Options.InputFile
        sys.exit(1)
    infile  = ROOT.TFile(Options.InputFile)
    
    tree = infile.Get("MuonEfficiencyTest")
    branchesInFile = [key.GetName() for key in tree.GetListOfBranches()]

    allWPs = []
    for wp in KnownWPs.iterkeys():
        if not wp in allWPs: allWPs.append(wp)
    WPs = set([i[ : i.find("_SF")] for i in branchesInFile if not i.startswith("Muon") and i.find("SF") != -1 ])
    
    print "INFO: Found the following working points: %s"%(",".join(WPs))
   
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
    
    
    for wp in WPs:
        for sys in ["SYS", "STAT"]:
            Histos +=[
            SystDependency(
                 var_name = "pt", 
                 axis_title ="p_{T} #mu(%s) [MeV]"%(wp),
                 bins =500 , bmin = 30.e3, bmax = 10.e6,
                 wp =wp, sys = sys, test_tree = tree),
           SystDependency(
                var_name = "eta", 
                axis_title ="#eta #mu(%s)"%(wp),
                bins =25 , bmin = -2.5, bmax = 2.5,
                wp =wp, sys = sys, test_tree = tree),
           SystDependency(
                var_name = "phi", 
                axis_title ="#phi (%s)"%(wp),
                bins =20 , bmin = -3.1415, bmax = 3.1415,
                wp =wp, sys = sys, test_tree = tree) ]

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        if i % 2500 == 0: print "INFO: %d/%d events processed"%(i, tree.GetEntries())
        if math.fabs(tree.Muon_eta) > 2.5  or tree.Muon_pt < 15.e3: continue
        for H in Histos:  
             if tree.Muon_isHighPt == True or  H.name().find("HighPt") == -1: H.fill()
        
    print "INFO: Histograms filled"
    pu = PlotUtils()
    pu.Size = 18

    dummy = ROOT.TCanvas("dummy", "dummy", 800, 600)
    dummy.SaveAs("%s/AllSystDep%s.pdf[" % (Options.outDir, bonusname))
    for H in Histos:
        can = ROOT.TCanvas("sf_dep_%s"%(H.name()),"SFCheck",1000,600)
       
        nom = H.get_nom_H1().TH1()
        up = H.get_1dn_H1().TH1()
        dn = H.get_1up_H1().TH1()
        
        up.SetLineColor(ROOT.kRed)
        dn.SetLineColor(ROOT.kBlue)
                    
        up.Divide(nom)
        dn.Divide(nom)
        
                    
        up.Draw("HIST")
        dn.Draw("HISTSAME")
        up.SetTitle("1UP")
        dn.SetTitle("1DOWN")
        
        up.SetMinimum(0.01)     
        up.GetXaxis().SetRangeUser(40.e3, 10.e6)
        up.SetMaximum(2)
     
        up.GetYaxis().SetTitle("Ratio to nominal")
       
        pu.DrawLegend([(up,'L'),(dn,'L')], 0.3, 0.75, 0.9, 0.9)          
        pu.DrawTLatex(0.55, 0.55, "WP: %s, %s"%(H.get_wp(), H.get_sys()))
       # pu.DrawTLatex(0.55, 0.6, comp.name().split("_")[1])
        can.SaveAs("%s/SysCheck%s%s.pdf"%(Options.outDir, H.name(),bonusname))
        can.SaveAs("%s/AllSystDep%s.pdf" % (Options.outDir, bonusname))

    dummy.SaveAs("%s/AllSystDep%s.pdf]" % (Options.outDir, bonusname))
        
   
       
