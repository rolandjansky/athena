#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

print "\n IMPORTING SOME PYTHON LIBRARIES "
import sys
import os
import math

#Import the ROOT libraries
print "\n IMPORTING SOME ROOT LIBRARIES "
from ROOT import gStyle, TCanvas, TH1, TH1F, kBlack, kBlue, TFile, TLegend

print "\n IMPORTING SOME FUNCTIONS "
from plotsHelper import plotComp

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


BaseFile = TFile(file_name1)
ARTFile  = TFile(file_name2)

evtNumb_Base   = BaseFile.Get("evtNmb"      )
ph_pt_All_Base = BaseFile.Get("pT_Phot_All" )
ph_eta_Base    = BaseFile.Get("eta_Phot_All")
ph_phi_Base    = BaseFile.Get("phi_Phot_All")

evtNumb_ART   = ARTFile.Get("evtNmb"      )
ph_pt_All_ART = ARTFile.Get("pT_Phot_All" )
ph_eta_ART    = ARTFile.Get("eta_Phot_All")
ph_phi_ART    = ARTFile.Get("phi_Phot_All")

print " BEGINNING "

c1 = TCanvas("c1", "c1", 1200, 900)
c1.Divide(2, 2)

style=gStyle
style.SetOptStat(0)

myleg = TLegend(0.2, 0.3, 0.45, 0.45)

plotComp(evtNumb_Base   , evtNumb_ART   , 1, c1, myleg)
plotComp(ph_pt_All_Base , ph_pt_All_ART , 2, c1)
plotComp(ph_eta_Base    , ph_eta_ART    , 3, c1)
plotComp(ph_phi_Base    , ph_phi_ART    , 4, c1)

print " SAVING "

c1.Update()
c1.SaveAs("Comparison_photon.pdf")
c1.SaveAs("Comparison_photon.root")

print " FINISHED \n"
