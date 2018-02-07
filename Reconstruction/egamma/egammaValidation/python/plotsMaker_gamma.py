#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

print "\n IMPORTING LIBRARIES "
import sys
import os
import math

#Import the ROOT libraries
import ROOT
from ROOT import *

file_name1 = sys.argv[1]
file_name2 = sys.argv[2]

print "\n ****************************************************************************************** "
print " File 1 (Baseline) is \t", file_name1
print " File 2 (ART) is \t", file_name2
print " ****************************************************************************************** \n"

evtNumb_Base    = TH1F("evtNumb_Base"   , ("Event Number Basline")        , 250, 33894000, 33896000)
ph_pt_All_Base  = TH1F("ph_pt_All_Base" , ("Photon Pt Track All Baseline"), 200,        0,      200)
ph_eta_Base     = TH1F("ph_phi_Base"    , ("Photon Eta Coordinate")       , 200,       -3,        3)
ph_phi_Base     = TH1F("ph_phi_Base"    , ("Photon Phi Coordinate")       , 100, -math.pi,  math.pi)


evtNumb_ART    = TH1F("evtNumb_ART"  , ("Event Number ART")       , 250, 33894000, 33896000)
ph_pt_All_ART  = TH1F("ph_pt_All_ART", ("Photon Pt Track All ART"), 200,        0,      200)
ph_eta_ART     = TH1F("ph_phi_ART"    , ("Photon Eta Coordinate") , 200,       -3,        3)
ph_phi_ART     = TH1F("ph_phi_ART"    , ("Photon Phi Coordinate") , 100, -math.pi,  math.pi)


BaseFile = ROOT.TFile(file_name1)
ARTFile  = ROOT.TFile(file_name2)

evtNumb_Base   = BaseFile.Get("evtNmb"      )
ph_pt_All_Base = BaseFile.Get("pT_Phot_All" )
ph_eta_Base    = BaseFile.Get("eta_Phot_All")
ph_phi_Base    = BaseFile.Get("phi_Phot_All")

evtNumb_ART   = ARTFile.Get("evtNmb"      )
ph_pt_All_ART = ARTFile.Get("pT_Phot_All" )
ph_eta_ART    = ARTFile.Get("eta_Phot_All")
ph_phi_ART    = ARTFile.Get("phi_Phot_All")

print " BEGINNING "

c1 = ROOT.TCanvas("c1", "c1", 1200, 900)
c1.Divide(2, 2)

style=ROOT.gStyle
style.SetOptStat(0)


c1.cd(1)
evtNumb_Base.SetMarkerStyle(3)
evtNumb_ART.SetLineColor(ROOT.kBlue)
evtNumb_Base.Draw("hist p")
evtNumb_ART.Draw("same")

leg1 = TLegend(0.2, 0.3, 0.45, 0.45)
leg1.AddEntry(evtNumb_Base, "Baseline",      "p")
leg1.AddEntry(evtNumb_ART , "ART"     ,      "l")
leg1.Draw()

c1.cd(2)
ph_pt_All_Base.SetMarkerStyle(3);
ph_pt_All_ART.SetLineColor(ROOT.kBlue);
ph_pt_All_Base.Draw("hist p");
ph_pt_All_ART.Draw("same");

leg2 = TLegend(0.2, 0.3, 0.45, 0.45)
leg2.AddEntry(ph_pt_All_Base, "Baseline",      "p")
leg2.AddEntry(ph_pt_All_ART , "ART"     ,      "l")
leg2.Draw()

c1.cd(3)
ph_eta_Base.SetMarkerStyle(3);
ph_eta_ART.SetLineColor(ROOT.kBlue);
ph_eta_Base.Draw("hist p");
ph_eta_ART.Draw("same");

leg3 = TLegend(0.2, 0.3, 0.45, 0.45)
leg3.AddEntry(ph_eta_Base, "Baseline",      "p")
leg3.AddEntry(ph_eta_ART , "ART"     ,      "l")
leg3.Draw()

c1.cd(4)
ph_phi_Base.SetMarkerStyle(3);
ph_phi_ART.SetLineColor(ROOT.kBlue);
ph_phi_Base.Draw("hist p");
ph_phi_ART.Draw("same");

leg4 = TLegend(0.2, 0.3, 0.45, 0.45)
leg4.AddEntry(ph_phi_Base, "Baseline",      "p")
leg4.AddEntry(ph_phi_ART , "ART"     ,      "l")
leg4.Draw()

print " SAVING "

c1.Update()
c1.SaveAs("Comparison_photon.pdf")
c1.SaveAs("Comparison_photon.root")

print " FINISHED \n"
