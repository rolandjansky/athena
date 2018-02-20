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

evtNumb_Base    = TH1F("evtNumb_Base"   , ("Event Number Basline")          , 2000,    85000,   87000)
el_pt_All_Base  = TH1F("el_pt_All_Base" , ("Electron Pt Track All Baseline"),  200,        0,     200)
el_pt_LLH_Base  = TH1F("el_pt_LLH_Base" , ("Electron Pt Track LLH Baseline"),  200,        0,     200)
el_LLH_Eff_Base = TH1F("el_LLH_Eff_Base", ("LLH Electron Efficiency")       ,  200,        0,     200)
el_pt_MLH_Base  = TH1F("el_pt_MLH_Base" , ("Electron Pt Track MLH Baseline"),  200,        0,     200)
el_MLH_Eff_Base = TH1F("el_MLH_Eff_Base", ("MLH Electron Efficiency")       ,  200,        0,     200)
el_pt_TLH_Base  = TH1F("el_pt_TLH_Base" , ("Electron Pt Track TLH Baseline"),  200,        0,     200)
el_TLH_Eff_Base = TH1F("el_TLH_Eff_Base", ("TLH Electron Efficiency")       ,  200,        0,     200)
el_eta_Base     = TH1F("el_eta_Base"    , ("Electron Eta Coordinate")       ,  200,       -3,       3)
el_phi_Base     = TH1F("el_phi_Base"    , ("Electron Phi Coordinate")       ,  100, -math.pi, math.pi)


evtNumb_ART    = TH1F("evtNumb_ART"  , ("Event Number ART")         , 2000,    85000,   87000)
el_pt_All_ART  = TH1F("el_pt_All_ART", ("Electron Pt Track All ART"),  200,        0,     200)
el_pt_LLH_ART  = TH1F("el_pt_LLH_ART", ("Electron Pt Track LLH ART"),  200,        0,     200)
el_LLH_Eff_ART = TH1F("el_LLH_Eff_ART", ("LLH Electron Efficiency") ,  200,        0,     200)
el_pt_MLH_ART  = TH1F("el_pt_MLH_ART", ("Electron Pt Track MLH ART"),  200,        0,     200)
el_MLH_Eff_ART = TH1F("el_MLH_Eff_ART", ("MLH Electron Efficiency") ,  200,        0,     200)
el_pt_TLH_ART  = TH1F("el_pt_TLH_ART", ("Electron Pt Track TLH ART"),  200,        0,     200)
el_TLH_Eff_ART = TH1F("el_TLH_Eff_ART", ("TLH Electron Efficiency") ,  200,        0,     200)
el_eta_ART     = TH1F("el_eta_ART"    , ("Electron Eta Coordinate") ,  200,       -3,       3)
el_phi_ART     = TH1F("el_phi_ART"    , ("Electron Phi Coordinate") ,  100, -math.pi, math.pi)


BaseFile = TFile(file_name1)
ARTFile  = TFile(file_name2)

evtNumb_Base   = BaseFile.Get("evtNmb"       )
el_pt_All_Base = BaseFile.Get("pT_ElTrk_All" )
el_pt_LLH_Base = BaseFile.Get("pT_ElTrk_LLH" )
el_pt_MLH_Base = BaseFile.Get("pT_ElTrk_MLH" )
el_pt_TLH_Base = BaseFile.Get("pT_ElTrk_TLH" )
el_eta_Base    = BaseFile.Get("eta_ElTrk_All")
el_phi_Base    = BaseFile.Get("phi_ElTrk_All")

evtNumb_ART   = ARTFile.Get("evtNmb"       )
el_pt_All_ART = ARTFile.Get("pT_ElTrk_All" )
el_pt_LLH_ART = ARTFile.Get("pT_ElTrk_LLH" )
el_pt_MLH_ART = ARTFile.Get("pT_ElTrk_MLH" )
el_pt_TLH_ART = ARTFile.Get("pT_ElTrk_TLH" )
el_eta_ART    = ARTFile.Get("eta_ElTrk_All")
el_phi_ART    = ARTFile.Get("phi_ElTrk_All")

print " BEGINNING "

c1 = TCanvas("c1", "c1", 1200, 900)
c1.Divide(2, 3)

style=gStyle
style.SetOptStat(0)

myleg = TLegend(0.2, 0.3, 0.45, 0.45)

el_LLH_Eff_Base.Divide(el_pt_LLH_Base, el_pt_All_Base)
el_LLH_Eff_ART.Divide(el_pt_LLH_ART, el_pt_All_ART)

el_MLH_Eff_Base.Divide(el_pt_MLH_Base, el_pt_All_Base)
el_MLH_Eff_ART.Divide(el_pt_MLH_ART, el_pt_All_ART)

el_TLH_Eff_Base.Divide(el_pt_TLH_Base, el_pt_All_Base)
el_TLH_Eff_ART.Divide(el_pt_TLH_ART, el_pt_All_ART)

plotComp(evtNumb_Base   , evtNumb_ART   , 1, c1, myleg)
plotComp(el_LLH_Eff_Base, el_LLH_Eff_ART, 2, c1)
plotComp(el_MLH_Eff_Base, el_MLH_Eff_ART, 3, c1)
plotComp(el_TLH_Eff_Base, el_TLH_Eff_ART, 4, c1)
plotComp(el_eta_Base    , el_eta_ART    , 5, c1)
plotComp(el_phi_Base    , el_phi_ART    , 6, c1)

print " SAVING "

c1.Update()
c1.SaveAs("Comparison_electron.pdf")
c1.SaveAs("Comparison_electron.root")

print " FINISHED \n"
