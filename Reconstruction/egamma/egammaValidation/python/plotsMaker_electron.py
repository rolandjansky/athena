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

evtNumb_Base    = TH1F("evtNumb_Base"   , ("Event Number Basline")          , 2000,    85000,   87000)
el_pt_All_Base  = TH1F("el_pt_All_Base" , ("Electron Pt Track All Baseline"),  200,        0,     200)
el_pt_LLH_Base  = TH1F("el_pt_LLH_Base" , ("Electron Pt Track LLH Baseline"),  200,        0,     200)
el_LLH_Eff_Base = TH1F("el_LLH_Eff_Base", ("LLH Electron Efficiency")       ,  200,        0,     200)
el_pt_MLH_Base  = TH1F("el_pt_MLH_Base" , ("Electron Pt Track MLH Baseline"),  200,        0,     200)
el_MLH_Eff_Base = TH1F("el_MLH_Eff_Base", ("MLH Electron Efficiency")       ,  200,        0,     200)
el_pt_TLH_Base  = TH1F("el_pt_TLH_Base" , ("Electron Pt Track TLH Baseline"),  200,        0,     200)
el_TLH_Eff_Base = TH1F("el_TLH_Eff_Base", ("TLH Electron Efficiency")       ,  200,        0,     200)
el_eta_Base     = TH1F("el_phi_Base"    , ("Electron Eta Coordinate")       ,  200,       -3,       3)
el_phi_Base     = TH1F("el_phi_Base"    , ("Electron Phi Coordinate")       ,  100, -math.pi, math.pi)



evtNumb_ART    = TH1F("evtNumb_ART"  , ("Event Number ART")         , 2000,    85000,   87000)
el_pt_All_ART  = TH1F("el_pt_All_ART", ("Electron Pt Track All ART"),  200,        0,     200)
el_pt_LLH_ART  = TH1F("el_pt_LLH_ART", ("Electron Pt Track LLH ART"),  200,        0,     200)
el_LLH_Eff_ART = TH1F("el_LLH_Eff_ART", ("LLH Electron Efficiency") ,  200,        0,     200)
el_pt_MLH_ART  = TH1F("el_pt_MLH_ART", ("Electron Pt Track MLH ART"),  200,        0,     200)
el_MLH_Eff_ART = TH1F("el_MLH_Eff_ART", ("MLH Electron Efficiency") ,  200,        0,     200)
el_pt_TLH_ART  = TH1F("el_pt_TLH_ART", ("Electron Pt Track TLH ART"),  200,        0,     200)
el_TLH_Eff_ART = TH1F("el_TLH_Eff_ART", ("TLH Electron Efficiency") ,  200,        0,     200)
el_eta_ART     = TH1F("el_phi_ART"    , ("Electron Eta Coordinate") ,  200,       -3,       3)
el_phi_ART     = TH1F("el_phi_ART"    , ("Electron Phi Coordinate") ,  100, -math.pi, math.pi)


BaseFile = ROOT.TFile(file_name1)
ARTFile  = ROOT.TFile(file_name2)

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

c1 = ROOT.TCanvas("c1", "c1", 1200, 900)
c1.Divide(2, 3)

style=ROOT.gStyle
style.SetOptStat(0)

el_LLH_Eff_Base.Divide(el_pt_LLH_Base, el_pt_All_Base)
el_LLH_Eff_ART.Divide(el_pt_LLH_ART, el_pt_All_ART)

el_MLH_Eff_Base.Divide(el_pt_MLH_Base, el_pt_All_Base)
el_MLH_Eff_ART.Divide(el_pt_MLH_ART, el_pt_All_ART)

el_TLH_Eff_Base.Divide(el_pt_TLH_Base, el_pt_All_Base)
el_TLH_Eff_ART.Divide(el_pt_TLH_ART, el_pt_All_ART)

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
el_LLH_Eff_Base.SetMarkerStyle(3);
el_LLH_Eff_ART.SetLineColor(ROOT.kBlue);
el_LLH_Eff_Base.Draw("hist p");
el_LLH_Eff_ART.Draw("same");

leg2 = TLegend(0.2, 0.3, 0.45, 0.45)
leg2.AddEntry(el_LLH_Eff_Base, "Baseline",      "p")
leg2.AddEntry(el_LLH_Eff_ART , "ART"     ,      "l")
leg2.Draw()

c1.cd(3)
el_MLH_Eff_Base.SetMarkerStyle(3);
el_MLH_Eff_ART.SetLineColor(ROOT.kBlue);
el_MLH_Eff_Base.Draw("hist p");
el_MLH_Eff_ART.Draw("same");

leg3 = TLegend(0.2, 0.3, 0.45, 0.45)
leg3.AddEntry(el_MLH_Eff_Base, "Baseline",      "p")
leg3.AddEntry(el_MLH_Eff_ART , "ART"     ,      "l")
leg3.Draw()

c1.cd(4)
el_TLH_Eff_Base.SetMarkerStyle(3);
el_TLH_Eff_ART.SetLineColor(ROOT.kBlue);
el_TLH_Eff_Base.Draw("hist p");
el_TLH_Eff_ART.Draw("same");

leg4 = TLegend(0.2, 0.3, 0.45, 0.45)
leg4.AddEntry(el_TLH_Eff_Base, "Baseline",      "p")
leg4.AddEntry(el_TLH_Eff_ART , "ART"     ,      "l")
leg4.Draw()

c1.cd(5)
el_eta_Base.SetMarkerStyle(3);
el_eta_ART.SetLineColor(ROOT.kBlue);
el_eta_Base.Draw("hist p");
el_eta_ART.Draw("same");

leg5 = TLegend(0.2, 0.3, 0.45, 0.45)
leg5.AddEntry(el_eta_Base, "Baseline",      "p")
leg5.AddEntry(el_eta_ART , "ART"     ,      "l")
leg5.Draw()

c1.cd(6)
el_phi_Base.SetMarkerStyle(3);
el_phi_ART.SetLineColor(ROOT.kBlue);
el_phi_Base.Draw("hist p");
el_phi_ART.Draw("same");

leg6 = TLegend(0.2, 0.3, 0.45, 0.45)
leg6.AddEntry(el_phi_Base, "Baseline",      "p")
leg6.AddEntry(el_phi_ART , "ART"     ,      "l")
leg6.Draw()

print " SAVING "

c1.Update()
c1.SaveAs("Comparison_electron.pdf")
c1.SaveAs("Comparison_electron.root")

print " FINISHED \n"
