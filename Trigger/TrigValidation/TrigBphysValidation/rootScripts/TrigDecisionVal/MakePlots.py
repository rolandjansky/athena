#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## ==========================================================
## For B-trigger Monte Carlo Physics Validation : Make Plots
## author: Yi Yang <yi.yang@cern.ch>
## 2010 Nov. 15
## ==========================================================
import string, os
## ==========================================================
## Basic settings
## ==========================================================
execute     = True   # execute macro
ATLAS       = False  # To load ATLAS style, need ATLAS style files in the working directory
makePlot    = True   # Make plots 
SaveFiles   = False
savedirPath = "./pic_test_L2_mu4_Upsi"
## ==========================================================
## Signal type (Jpsi or Upsi)
## ==========================================================
Signal     = "Upsi" # for the signal : Jpsi or Upsi or Bmumu
## ==========================================================
## Input Files : Monte Carlo
## ==========================================================
dirPath   = "./"
mcFile  = [ ]        # Don't comment out
Name_mcFile  = [ ]   # Don't comment out

mcFile  += [""+dirPath+"Histograms_Data_test.root"]
Name_mcFile += ["test"]
mcFile  += [""+dirPath+"../../Release16/PhyValidation/Histograms_MC_Upsilon1S_mu2p5mu2p5_r2040.root"]
Name_mcFile += ["ref"]

## ==========================================================
## Input Files : Real Data ( Doing the simplest sideband subtraction)
## ==========================================================
dataFile   = [ ]       # Don't comment out
Name_dataFile  =[ ]    # Don't comment out
#dataFile  += [ ""+dirPath+"Histograms_Data_E_after180644_2011.root" ]
#Name_dataFile +=["2011_E_after180644"]
#dataFile  += [ ""+dirPath+"Histograms_Data_H.root" ]
#Name_dataFile +=["period_H"]


## ==========================================================
## name of the chain that should be analyzed
## ==========================================================
#denominator = "L1_MU0"
denominator = "L1_2MU0"
#denominator = "EF_mu4_Trk_Jpsi"
#denominator = "EF_mu4_Trk_Upsi_FS"
chains = [ ]
## Test
## Topology L2
"""
chains += ["L2_2mu4"]
chains += ["L2_2mu4_DiMu"]
#chains += ["L2_2mu4_DiMu_noVtx"]
chains += ["L2_2mu4_Jpsimumu"]
#chains += ["L2_2mu4_Upsimumu"]
"""

## Topology EF
chains += ["EF_2mu4"]
chains += ["EF_2mu4_DiMu"]
#chains += ["EF_2mu4_DiMu_noVtx"]
#chains += ["EF_2mu4_Jpsimumu"]
chains += ["EF_2mu4_Upsimumu"]

"""
## TrigDiMu L2
chains += [ "L2_mu4_DiMu" ]
chains += [ "L2_mu4_DiMu_FS" ]
chains += [ "L2_mu4_DiMu_MG" ]
chains += [ "L2_mu4_DiMu_MG_FS" ]
#chains += [ "L2_mu4_DiMu_SiTrk" ]
#chains += [ "L2_mu4_DiMu_SiTrk_FS" ]
#chains += [ "L2_MU4_DiMu" ]
#chains += [ "L2_MU4_DiMu_FS" ]
chains += [ "L2_mu4_Jpsimumu"]
#chains += [ "L2_mu4_Upsimumu_FS"]
"""
"""
## TrigDiMu EF
chains += [ "EF_mu4_DiMu" ]
chains += [ "EF_mu4_DiMu_FS" ]
chains += [ "EF_mu4_DiMu_MG" ]
chains += [ "EF_mu4_DiMu_MG_FS" ]
#chains += [ "EF_mu4_DiMu_SiTrk" ]
#chains += [ "EF_mu4_DiMu_SiTrk_FS" ]
#chains += [ "EF_MU4_DiMu" ]
#chains += [ "EF_MU4_DiMu_FS" ]
#chains += [ "EF_mu4_Jpsimumu" ]
#chains += [ "EF_mu6_Jpsimumu" ]
#chains += [ "EF_mu10_Jpsimumu" ]
#chains += [ "EF_mu4_Upsimumu_FS" ]
#chains += [ "EF_mu4_Trk_Jpsi" ]
#chains += [ "EF_mu4_Trk_Upsi_FS" ]
"""
"""
chains += [ "EF_mu6_Jpsimumu" ]
chains += [ "EF_mu6_Jpsimumu_tight" ]
"""
"""
chains += [ "EF_mu4_Upsimumu_FS" ]
chains += [ "EF_mu4_Upsimumu_tight_FS" ]
"""























## ====================================================================================
##
##
## Main script  ( If not necessary, please not edit )
##
##
## ====================================================================================
## generate root macro
macro  = ""
macro += "void makePlots(){ \n"

# Basic setting
if (ATLAS):
    macro += "   gROOT->LoadMacro(\"AtlasUtils.C\"); \n" 
else :
    macro += "   gROOT->SetStyle(\"Plain\"); \n"
    macro += "   gStyle->SetOptFit(1); \n" 
    macro += "   gStyle->SetPadColor(0); \n"
    macro += "   gStyle->SetPadTopMargin(0.05); \n"
    macro += "   gStyle->SetPadRightMargin(0.05); \n"
    macro += "   gStyle->SetPadBottomMargin(0.16); \n"
    macro += "   gStyle->SetPadLeftMargin(0.16); \n"

macro += "   gStyle->SetPalette(1); \n"
macro += "   gStyle->SetOptStat(0); \n"
macro += "   gStyle->SetEndErrorSize(0); \n"
macro += "\n"


macro += "\n"

# Load files and histograms
macro += "   // Reference Samples \n"
count_mc = 0
for mc_file in mcFile :
    count_mc = count_mc + 1
    macro += "   TFile *f_mc"+str(count_mc)+" = new TFile(\""+mc_file+"\"); \n "
    macro += "  TH1F *h_mass_"+denominator+"_mc"+str(count_mc)+"        = (TH1F*)h_"+Signal+"_mass_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_mc"+str(count_mc)+"_sg     = (TH1F*)h_"+Signal+"_mass_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_mc"+str(count_mc)+"_bk1    = (TH1F*)h_"+Signal+"_mass_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_mc"+str(count_mc)+"_bk2    = (TH1F*)h_"+Signal+"_mass_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_mc"+str(count_mc)+"_sg   = (TH1F*)h_"+Signal+"_mu1_pt_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_mc"+str(count_mc)+"_bk1  = (TH1F*)h_"+Signal+"_mu1_pt_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_mc"+str(count_mc)+"_bk2  = (TH1F*)h_"+Signal+"_mu1_pt_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_mc"+str(count_mc)+"_sg  = (TH1F*)h_"+Signal+"_mu1_eta_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_mc"+str(count_mc)+"_bk1 = (TH1F*)h_"+Signal+"_mu1_eta_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_mc"+str(count_mc)+"_bk2 = (TH1F*)h_"+Signal+"_mu1_eta_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_mc"+str(count_mc)+"_sg   = (TH1F*)h_"+Signal+"_mu2_pt_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_mc"+str(count_mc)+"_bk1  = (TH1F*)h_"+Signal+"_mu2_pt_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_mc"+str(count_mc)+"_bk2  = (TH1F*)h_"+Signal+"_mu2_pt_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_mc"+str(count_mc)+"_sg  = (TH1F*)h_"+Signal+"_mu2_eta_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_mc"+str(count_mc)+"_bk1 = (TH1F*)h_"+Signal+"_mu2_eta_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_mc"+str(count_mc)+"_bk2 = (TH1F*)h_"+Signal+"_mu2_eta_bk2_"+denominator+"_"+denominator+ "; \n"
    for chain in chains :
        macro += "   TH1F *h_mass_"+chain+"_mc"+str(count_mc)+"        = (TH1F*)h_"+Signal+"_mass_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_mc"+str(count_mc)+"_sg     = (TH1F*)h_"+Signal+"_mass_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_mc"+str(count_mc)+"_bk1    = (TH1F*)h_"+Signal+"_mass_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_mc"+str(count_mc)+"_bk2    = (TH1F*)h_"+Signal+"_mass_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_mc"+str(count_mc)+"_sg   = (TH1F*)h_"+Signal+"_mu1_pt_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_mc"+str(count_mc)+"_bk1  = (TH1F*)h_"+Signal+"_mu1_pt_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_mc"+str(count_mc)+"_bk2  = (TH1F*)h_"+Signal+"_mu1_pt_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_mc"+str(count_mc)+"_sg  = (TH1F*)h_"+Signal+"_mu1_eta_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_mc"+str(count_mc)+"_bk1 = (TH1F*)h_"+Signal+"_mu1_eta_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_mc"+str(count_mc)+"_bk2 = (TH1F*)h_"+Signal+"_mu1_eta_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_mc"+str(count_mc)+"_sg   = (TH1F*)h_"+Signal+"_mu2_pt_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_mc"+str(count_mc)+"_bk1  = (TH1F*)h_"+Signal+"_mu2_pt_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_mc"+str(count_mc)+"_bk2  = (TH1F*)h_"+Signal+"_mu2_pt_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_mc"+str(count_mc)+"_sg  = (TH1F*)h_"+Signal+"_mu2_eta_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_mc"+str(count_mc)+"_bk1 = (TH1F*)h_"+Signal+"_mu2_eta_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_mc"+str(count_mc)+"_bk2 = (TH1F*)h_"+Signal+"_mu2_eta_bk2_"+denominator+"_"+chain+ "; \n"

macro += "\n"

macro += "   // Real Data Samples \n"
count_data = 0
for data_file in dataFile :
    count_data = count_data + 1
    macro += "   TFile *f_data"+str(count_data)+" = new TFile(\""+data_file+"\"); \n "
    macro += "  TH1F *h_mass_"+denominator+"_data"+str(count_data)+"        = (TH1F*)h_"+Signal+"_mass_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_data"+str(count_data)+"_sg     = (TH1F*)h_"+Signal+"_mass_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_data"+str(count_data)+"_bk1    = (TH1F*)h_"+Signal+"_mass_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mass_"+denominator+"_data"+str(count_data)+"_bk2    = (TH1F*)h_"+Signal+"_mass_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_data"+str(count_data)+"_sg   = (TH1F*)h_"+Signal+"_mu1_pt_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_data"+str(count_data)+"_bk1  = (TH1F*)h_"+Signal+"_mu1_pt_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_pt_"+denominator+"_data"+str(count_data)+"_bk2  = (TH1F*)h_"+Signal+"_mu1_pt_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_data"+str(count_data)+"_sg  = (TH1F*)h_"+Signal+"_mu1_eta_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_data"+str(count_data)+"_bk1 = (TH1F*)h_"+Signal+"_mu1_eta_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu1_eta_"+denominator+"_data"+str(count_data)+"_bk2 = (TH1F*)h_"+Signal+"_mu1_eta_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_data"+str(count_data)+"_sg   = (TH1F*)h_"+Signal+"_mu2_pt_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_data"+str(count_data)+"_bk1  = (TH1F*)h_"+Signal+"_mu2_pt_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_pt_"+denominator+"_data"+str(count_data)+"_bk2  = (TH1F*)h_"+Signal+"_mu2_pt_bk2_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_data"+str(count_data)+"_sg  = (TH1F*)h_"+Signal+"_mu2_eta_sg_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_data"+str(count_data)+"_bk1 = (TH1F*)h_"+Signal+"_mu2_eta_bk1_"+denominator+"_"+denominator+ "; \n"
    macro += "  TH1F *h_mu2_eta_"+denominator+"_data"+str(count_data)+"_bk2 = (TH1F*)h_"+Signal+"_mu2_eta_bk2_"+denominator+"_"+denominator+ "; \n"
    for chain in chains :
        macro += "   TH1F *h_mass_"+chain+"_data"+str(count_data)+"        = (TH1F*)h_"+Signal+"_mass_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_data"+str(count_data)+"_sg     = (TH1F*)h_"+Signal+"_mass_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_data"+str(count_data)+"_bk1    = (TH1F*)h_"+Signal+"_mass_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mass_"+chain+"_data"+str(count_data)+"_bk2    = (TH1F*)h_"+Signal+"_mass_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_data"+str(count_data)+"_sg   = (TH1F*)h_"+Signal+"_mu1_pt_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_data"+str(count_data)+"_bk1  = (TH1F*)h_"+Signal+"_mu1_pt_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_pt_"+chain+"_data"+str(count_data)+"_bk2  = (TH1F*)h_"+Signal+"_mu1_pt_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_data"+str(count_data)+"_sg  = (TH1F*)h_"+Signal+"_mu1_eta_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_data"+str(count_data)+"_bk1 = (TH1F*)h_"+Signal+"_mu1_eta_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu1_eta_"+chain+"_data"+str(count_data)+"_bk2 = (TH1F*)h_"+Signal+"_mu1_eta_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_data"+str(count_data)+"_sg   = (TH1F*)h_"+Signal+"_mu2_pt_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_data"+str(count_data)+"_bk1  = (TH1F*)h_"+Signal+"_mu2_pt_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_pt_"+chain+"_data"+str(count_data)+"_bk2  = (TH1F*)h_"+Signal+"_mu2_pt_bk2_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_data"+str(count_data)+"_sg  = (TH1F*)h_"+Signal+"_mu2_eta_sg_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_data"+str(count_data)+"_bk1 = (TH1F*)h_"+Signal+"_mu2_eta_bk1_"+denominator+"_"+chain+ "; \n"
        macro += "   TH1F *h_mu2_eta_"+chain+"_data"+str(count_data)+"_bk2 = (TH1F*)h_"+Signal+"_mu2_eta_bk2_"+denominator+"_"+chain+ "; \n"

    macro += "\n"

macro += "   // Plots \n"
## =================================================
## plot
## =================================================
numb_c1col = len(mcFile) + len(dataFile)
if len(mcFile) + len(dataFile) > 3 :
    numb_c1col = 3
c1col = numb_c1col * 400
numb_c1row = (len(mcFile) + len(dataFile))//3
if ((len(mcFile) + len(dataFile))//3) < 1 :
    numb_c1row = 1
if ((len(mcFile) + len(dataFile))/3.) > 1 :
    numb_c1row = ( ((len(mcFile) + len(dataFile))//3)  + 1)
c1row = numb_c1row * 400

macro += "   c1 = new TCanvas(\"HF1\", \"Histos1\", 50, 50,"+str(c1col)+", "+str(c1row)+"); \n"

#print total_files
macro += "   c1->Divide("+str(numb_c1col)+","+str(numb_c1row)+" );\n"

# Rebin
rebin = 10
if Signal == "Upsi":
    rebin = 40
plot_tot = 0
Color = [4, 2, 3, 5, 7, 29, 43, 37, 8 ]

## =================================================
##  Reference Samples 
## =================================================
plot_mc = 0
for mc_file in mcFile :
    plot_tot = plot_tot + 1
    macro += "   c1->cd("+str(plot_tot)+"); \n"

    plot_mc = plot_mc + 1
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->Rebin(" +str(rebin)+ "); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->SetTitle(""); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetXaxis()->SetTitle(\"Inv. Mass(#mu#mu) [GeV]\"); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetYaxis()->SetTitle(\"Entries\"); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetXaxis()->SetNdivisions(505); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetYaxis()->SetNdivisions(505); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->SetLineWidth(2); \n"
    macro += "   h_mass_"+denominator+"_mc"+str(plot_mc)+" ->Draw(\"hist\"); \n"

    macro += "   TLine *l1 = new TLine(2.9, 0, 2.9, h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetMaximum()*0.7); \n"
    macro += "   l1->SetLineStyle(7); \n"
    macro += "   l1->SetLineWidth(2); \n"
    macro += "   l1->Draw(\"same\"); \n"
    macro += "   TLine *l2 = new TLine(3.3, 0, 3.3, h_mass_"+denominator+"_mc"+str(plot_mc)+" ->GetMaximum()*0.7); \n"
    macro += "   l2->SetLineStyle(7); \n"
    macro += "   l2->SetLineWidth(2); \n"
    macro += "   l2->Draw(\"same\"); \n"

    if Signal != "Upsi" : 
        macro += "   TLegend *leg = new TLegend(0.6,0.5,0.9,0.9); \n"
    if Signal == "Upsi" : 
        macro += "   TLegend *leg = new TLegend(0.2,0.5,0.5,0.9); \n"
    macro += "   leg->SetHeader(\"MC ["+Name_mcFile[plot_mc-1]+ "] : w.r.t. "+denominator+" \" );\n"
    macro += "   leg->AddEntry(h_mass_"+denominator+"_mc"+str(plot_mc)+", \""+denominator+"\", \"f\"); \n"

    ### Calculate Efficiency
    macro += " cout << \"==============================================\" << endl; \n"
    macro += " cout << \"           MC Sample : "+Name_mcFile[plot_mc-1]+"      \"  << endl; \n"
    macro += " cout << \"==============================================\" << endl; \n"
    macro += " cout << \" "+denominator+"   : \" << h_mass_"+denominator+"_mc"+str(plot_mc)+"_sg ->Integral()  <<  endl; \n"

    color_mc = 0
    for chain in chains :
        macro += "   h_mass_"+chain+"_mc"+str(plot_mc)+" ->Rebin(" +str(rebin)+ "); \n"
        macro += "   h_mass_"+chain+"_mc"+str(plot_mc)+" ->SetLineWidth(2); \n"
        macro += "   h_mass_"+chain+"_mc"+str(plot_mc)+" ->SetFillColor("+str(Color[color_mc])+"); \n"
        macro += "   h_mass_"+chain+"_mc"+str(plot_mc)+" ->Draw(\"histsame\"); \n"

        macro += "   leg->AddEntry(h_mass_"+chain+"_mc"+str(plot_mc)+", \""+chain+"\", \"f\"); \n"

        ### Calculate Efficiency
        macro += " cout << \" "+chain+"   : \" << h_mass_"+chain+"_mc"+str(plot_mc)+"_sg ->Integral() << \"     Eff [%] : \" << ( h_mass_"+chain+"_mc"+str(plot_mc)+"_sg ->Integral() / h_mass_"+denominator+"_mc"+str(plot_mc)+"_sg ->Integral() )*100 << \" +/- \" << 100*sqrt( ( ( h_mass_"+chain+"_mc"+str(plot_mc)+"_sg ->Integral() / h_mass_"+denominator+"_mc"+str(plot_mc)+"_sg ->Integral() )*(1 - ( h_mass_"+chain+"_mc"+str(plot_mc)+"_sg ->Integral() / h_mass_"+denominator+"_mc"+str(plot_mc)+"_sg ->Integral() ) ) ) /  h_mass_"+denominator+"_mc"+str(plot_mc)+"_sg ->Integral() )<< endl; \n"

        color_mc = color_mc + 1

    macro += " leg->SetFillColor(10); \n"
    macro += " leg->SetLineColor(10); \n"
    macro += " leg->SetTextSize(0.025); \n"
    macro += " leg->Draw(\"same\"); \n"

   
    macro += "\n"

## =================================================
##  Real Data Samples 
## =================================================
plot_data = 0
mean_bk1 = 2.75;
mean_bk2 = 4.05;
dm_bk = 0.3; 
dm_sg = 0.4;
m_sg1 = 2.9;
m_sg2 = 3.3;
if Signal == "Upsi" :
    mean_bk1 = 8.75;
    mean_bk2 = 10.25;
    dm_bk = 0.5; 
    dm_sg = 1.;
    m_sg1 = 9.;
    m_sg2 = 10.;

for data_file in dataFile :
    plot_tot = plot_tot + 1
    macro += "   c1->cd("+str(plot_tot)+");\n"

    plot_data = plot_data + 1
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->Rebin(" +str(rebin)+ "); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->SetTitle(""); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->GetXaxis()->SetTitle(\"Inv. Mass(#mu#mu) [GeV]\"); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->GetYaxis()->SetTitle(\"Entries\"); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->GetXaxis()->SetNdivisions(505); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->GetYaxis()->SetNdivisions(505); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+"  ->SetLineWidth(2); \n"
    macro += "   h_mass_"+denominator+"_data"+str(plot_data)+" ->Draw(\"hist\"); \n"

    macro += "   TLine *l1 = new TLine(2.9, 0, 2.9, h_mass_"+denominator+"_data"+str(plot_data)+" ->GetMaximum()*0.7); \n"
    macro += "   l1->SetLineStyle(7); \n"
    macro += "   l1->SetLineWidth(2); \n"
    macro += "   l1->Draw(\"same\"); \n"
    macro += "   TLine *l2 = new TLine(3.3, 0, 3.3, h_mass_"+denominator+"_data"+str(plot_data)+" ->GetMaximum()*0.7); \n"
    macro += "   l2->SetLineStyle(7); \n"
    macro += "   l2->SetLineWidth(2); \n"
    macro += "   l2->Draw(\"same\"); \n"

    if Signal != "Upsi" : 
        macro += "   TLegend *leg = new TLegend(0.6,0.5,0.9,0.9); \n"
    if Signal == "Upsi" : 
        macro += "   TLegend *leg = new TLegend(0.2,0.5,0.5,0.9); \n"
    macro += "   leg->SetHeader(\"Data ["+Name_dataFile[plot_data-1]+ "] : w.r.t. "+denominator+"\" );\n"
    macro += "   leg->AddEntry(h_mass_"+denominator+"_data"+str(plot_data)+", \""+denominator+"\", \"f\"); \n"

    ### Calculate Efficiency
    macro += "   double sig_"+denominator+"_data"+str(plot_data)+" = h_mass_"+denominator+"_data"+str(plot_data)+"_sg -> Integral(); \n"
    macro += "   double bk1_"+denominator+"_data"+str(plot_data)+" = h_mass_"+denominator+"_data"+str(plot_data)+"_bk1 -> Integral(); \n"
    macro += "   double bk2_"+denominator+"_data"+str(plot_data)+" = h_mass_"+denominator+"_data"+str(plot_data)+"_bk2 -> Integral(); \n"
    macro += "   double p0_"+denominator+"_data"+str(plot_data)+" = (bk1_"+denominator+"_data"+str(plot_data)+"*"+str(mean_bk2)+" - bk2_"+denominator+"_data"+str(plot_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+") );          \n"    
    macro += "   double p1_"+denominator+"_data"+str(plot_data)+" = (bk1_"+denominator+"_data"+str(plot_data)+" - bk2_"+denominator+"_data"+str(plot_data)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk1)+" - "+str(mean_bk2)+") );          \n"    
      
    macro += "   double bk_"+denominator+"_data"+str(plot_data)+" = p0_"+denominator+"_data"+str(plot_data)+"*"+str(dm_sg)+" + (p1_"+denominator+"_data"+str(plot_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+"  )                 ; \n"
       
    macro += " cout << \"==============================================\" << endl; \n"
    macro += " cout << \"           Data : "+Name_dataFile[plot_data-1]+"      \"  << endl; \n"
    macro += " cout << \"==============================================\" << endl; \n"
    macro += " cout << \" "+denominator+"   : \" << sig_"+denominator+"_data"+str(plot_data)+"- bk_"+denominator+"_data"+str(plot_data)+"  <<  endl; \n"

    color_data = 0
    for chain in chains :
        macro += "   h_mass_"+chain+"_data"+str(plot_data)+" ->Rebin(" +str(rebin)+ "); \n"
        macro += "   h_mass_"+chain+"_data"+str(plot_data)+" ->SetLineWidth(2); \n"
        macro += "   h_mass_"+chain+"_data"+str(plot_data)+" ->SetFillColor("+str(Color[color_data])+"); \n"
        macro += "   h_mass_"+chain+"_data"+str(plot_data)+" ->Draw(\"histsame\"); \n"
    
        macro += "   leg->AddEntry(h_mass_"+chain+"_data"+str(plot_data)+", \""+chain+"\", \"f\"); \n"

        ### Calculate Efficiency
        macro += "   double sig_"+chain+"_data"+str(plot_data)+" = h_mass_"+chain+"_data"+str(plot_data)+"_sg -> Integral(); \n"
        macro += "   double bk1_"+chain+"_data"+str(plot_data)+" = h_mass_"+chain+"_data"+str(plot_data)+"_bk1 -> Integral(); \n"
        macro += "   double bk2_"+chain+"_data"+str(plot_data)+" = h_mass_"+chain+"_data"+str(plot_data)+"_bk2 -> Integral(); \n"
        macro += "   double p0_"+chain+"_data"+str(plot_data)+" = (bk1_"+chain+"_data"+str(plot_data)+"*"+str(mean_bk2)+" - bk2_"+chain+"_data"+str(plot_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+") );          \n"    
        macro += "   double p1_"+chain+"_data"+str(plot_data)+" = (bk1_"+chain+"_data"+str(plot_data)+" - bk2_"+chain+"_data"+str(plot_data)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk1)+" - "+str(mean_bk2)+") );          \n"    
      
        macro += "   double bk_"+chain+"_data"+str(plot_data)+" = p0_"+chain+"_data"+str(plot_data)+"*"+str(dm_sg)+" + (p1_"+chain+"_data"+str(plot_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+"  )                 ; \n"
        
        ## Cout ....
        macro += " cout << \" "+chain+"   : \" << sig_"+chain+"_data"+str(plot_data)+"- bk_"+chain+"_data"+str(plot_data)+"  << \"     Eff [%]: \" << (  ( sig_"+chain+"_data"+str(plot_data)+"- bk_"+chain+"_data"+str(plot_data)+" )  / ( sig_"+denominator+"_data"+str(plot_data)+"- bk_"+denominator+"_data"+str(plot_data)+" )     ) *100 << \" +/- \" <<  100*sqrt( ( ( ( sig_"+chain+"_data"+str(plot_data)+"- bk_"+chain+"_data"+str(plot_data)+" )  / ( sig_"+denominator+"_data"+str(plot_data)+"- bk_"+denominator+"_data"+str(plot_data)+" )  )*( 1 - ( sig_"+chain+"_data"+str(plot_data)+"- bk_"+chain+"_data"+str(plot_data)+" )  / ( sig_"+denominator+"_data"+str(plot_data)+"- bk_"+denominator+"_data"+str(plot_data)+" ) ) ) / ( sig_"+denominator+"_data"+str(plot_data)+"- bk_"+denominator+"_data"+str(plot_data)+" )   )  << endl; \n"

        color_data = color_data + 1

    macro += " leg->SetFillColor(10); \n"
    macro += " leg->SetLineColor(10); \n"
    macro += " leg->SetTextSize(0.025); \n"
    macro += " leg->Draw(\"same\"); \n"
    
    macro += "\n"


macro += "\n"


## ==============================================
## Efficiency vs pT(mu1) Plots
## ==============================================
numb_c2col = len(chains)
if len(chains) > 4 :
    numb_c2col = 4
c2col = numb_c2col * 300
numb_c2row = len(chains)//4
if (len(chains)//4) < 1 :
    numb_c2row = 1
if (len(chains)/4.) > 1 :
    numb_c2row = ( (len(chains)//4)  + 1)
c2row = numb_c2row * 300

macro += "   c2 = new TCanvas(\"HF2\", \"Histos2\", 70, 70,"+str(c2col)+", "+str(c2row)+"); \n"
macro += "   c2->Divide("+str(numb_c2col)+","+str(numb_c2row)+"); \n"

## =================================================
## plot
## =================================================
# Rebin
rebin_pt = 2
plot_pt_tot = 0

for chain in chains :
    plot_pt_tot = plot_pt_tot + 1
    macro += "   c2->cd("+str(plot_pt_tot)+");\n"
    macro += "   TLegend *leg_"+chain+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+chain+" -> SetHeader(\""+chain+ " w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    # Monte Carlo
    plot_pt_mc = 0
    for mc_file in mcFile :
        plot_pt_mc = plot_pt_mc + 1
        if plot_pt_tot == 1 :
            macro += "   h_mu1_pt_"+denominator+"_mc"+str(plot_pt_mc)+"_sg ->Rebin(" +str(rebin_pt)+ "); \n"
        macro += "   h_mu1_pt_"+chain+"_mc"+str(plot_pt_mc)+"_sg  ->Rebin(" +str(rebin_pt)+ "); \n"
    
        macro += "  TGraphAsymmErrors *g_pt_"+chain+"_mc"+str(plot_pt_mc)+" = new TGraphAsymmErrors( h_mu1_pt_"+chain+"_mc"+str(plot_pt_mc)+"_sg, h_mu1_pt_"+denominator+"_mc"+str(plot_pt_mc)+"_sg ); \n"
        if plot_pt_mc == 1 :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetXaxis()->SetTitle(\"pT(#mu_{1}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_pt_"+chain+"_mc"+str(plot_pt_mc)+", \"MC sample : "+Name_mcFile[plot_pt_mc-1]+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    ## Real Data
    plot_pt_data = 0    
    for data_file in dataFile :
        plot_pt_data = plot_pt_data + 1
        if plot_pt_tot == 1 :
            macro += "   h_mu1_pt_"+denominator+"_data"+str(plot_pt_data)+"_sg ->Rebin(" +str(rebin_pt)+ "); \n"
            macro += "   double N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[20]; \n"
            macro += "   for (int i = 0; i < 20; i++){ \n"
            macro += "       double Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_data)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_data)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_data)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_pt_"+denominator+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_pt_"+denominator+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_pt_"+denominator+"_data"+str(plot_pt_data)+" = p0_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(dm_sg)+" + (p1_pt_"+denominator+"_data"+str(plot_pt_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] = (Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_data)+");  \n"
            macro += "       if ( (Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_data)+") < 0 ){ N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] = Signal_pt_"+denominator+"_data"+str(plot_pt_data)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x[20]; \n"
            macro += "    double xerr[20]; \n"
            macro += "    for (int i = 0; i < 20; i++){ \n"
            macro += "           x[i] = 0.5 + i; \n"
            macro += "           xerr[i] = 0.5; \n"
            macro += "     } \n"


        macro += "   h_mu1_pt_"+chain+"_data"+str(plot_pt_data)+"_sg  ->Rebin(" +str(rebin_pt)+ "); \n"
        macro += "   double N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[20]; \n"
        macro += "   for (int i = 0; i < 20; i++){ \n"
        macro += "       double Signal_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_data)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_data)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_data)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_pt_"+chain+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_pt_"+chain+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_pt_"+chain+"_data"+str(plot_pt_data)+" = p0_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(dm_sg)+" + (p1_pt_"+chain+"_data"+str(plot_pt_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = (Signal_pt_"+chain+"_data"+str(plot_pt_data)+" - NumB_pt_"+chain+"_data"+str(plot_pt_data)+");  \n"
        macro += "       if ( (Signal_pt_"+chain+"_data"+str(plot_pt_data)+" - NumB_pt_"+chain+"_data"+str(plot_pt_data)+") < 0 ){ N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = Signal_pt_"+chain+"_data"+str(plot_pt_data)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_pt_"+chain+"_data"+str(plot_pt_data)+"[20]; \n"
        macro += "     double eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[20];  \n"
        macro += "     for (int i = 0; i < 20; i++){ \n"
        macro += "          if ( N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] == 0 || N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] == 0 ){  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = 0;  \n" 
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i];  \n"
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = ( N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] )*sqrt( 1/N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] + 1/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_pt_"+chain+"_data"+str(plot_pt_data)+" = new TGraphErrors( 20, x, eff_pt_"+chain+"_data"+str(plot_pt_data)+", xerr, eff_err_pt_"+chain+"_data"+str(plot_pt_data)+" ); \n"
    
        if len(mcFile) == 0 :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetXaxis()->SetTitle(\"pT(#mu_{1}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_pt_"+chain+"_data"+str(plot_pt_data)+", \"Data sample : "+Name_dataFile[plot_pt_data-1]+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1


    macro += " leg_"+chain+" ->SetFillColor(10); \n"
    macro += " leg_"+chain+" ->SetLineColor(10); \n"
    macro += " leg_"+chain+" ->SetTextSize(0.025); \n"
    macro += " leg_"+chain+" ->Draw(\"same\"); \n"
   
    macro += "\n"





## ==============================================
## Efficiency vs eta(mu1) Plots
## ==============================================
macro += "   c3 = new TCanvas(\"HF3\", \"Histos3\", 90, 90,"+str(c2col)+", "+str(c2row)+"); \n"
macro += "   c3->Divide("+str(numb_c2col)+","+str(numb_c2row)+"); \n"

## =================================================
## plot
## =================================================
# Rebin
rebin_eta = 2
plot_eta_tot = 0

for chain in chains :
    plot_eta_tot = plot_eta_tot + 1
    macro += "   c3->cd("+str(plot_eta_tot)+");\n"
    macro += "   TLegend *leg_"+chain+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+chain+" -> SetHeader(\""+chain+ " w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    # Monte Carlo
    plot_eta_mc = 0
    for mc_file in mcFile :
        plot_eta_mc = plot_eta_mc + 1
        if plot_eta_tot == 1 :
            macro += "   h_mu1_eta_"+denominator+"_mc"+str(plot_eta_mc)+"_sg ->Rebin(" +str(rebin_eta)+ "); \n"
        macro += "   h_mu1_eta_"+chain+"_mc"+str(plot_eta_mc)+"_sg  ->Rebin(" +str(rebin_eta)+ "); \n"
    
        macro += "  TGraphAsymmErrors *g_eta_"+chain+"_mc"+str(plot_eta_mc)+" = new TGraphAsymmErrors( h_mu1_eta_"+chain+"_mc"+str(plot_eta_mc)+"_sg, h_mu1_eta_"+denominator+"_mc"+str(plot_eta_mc)+"_sg ); \n"
        if plot_eta_mc == 1 :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" ->SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{1}) \");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_eta_"+chain+"_mc"+str(plot_eta_mc)+", \"MC sample : "+Name_mcFile[plot_eta_mc-1]+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    ## Real Data
    plot_eta_data = 0    
    for data_file in dataFile :
        plot_eta_data = plot_eta_data + 1
        if plot_eta_tot == 1 :
            macro += "   h_mu1_eta_"+denominator+"_data"+str(plot_eta_data)+"_sg ->Rebin(" +str(rebin_eta)+ "); \n"
            macro += "   double N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[25]; \n"
            macro += "   for (int i = 0; i < 25; i++){ \n"
            macro += "       double Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_data)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_data)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_data)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_eta_"+denominator+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_eta_"+denominator+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_eta_"+denominator+"_data"+str(plot_eta_data)+" = p0_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(dm_sg)+" + (p1_eta_"+denominator+"_data"+str(plot_eta_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] = (Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_data)+");  \n"
            macro += "       if ( (Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_data)+") < 0 ){ N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] = Signal_eta_"+denominator+"_data"+str(plot_eta_data)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x_eta[25]; \n"
            macro += "    double xerr_eta[25]; \n"
            macro += "    for (int i = 0; i < 25; i++){ \n"
            macro += "           x_eta[i] = -2.4 + 0.2*(i); \n"
            macro += "           xerr_eta[i] = 0.1; \n"
            macro += "     } \n"


        macro += "   h_mu1_eta_"+chain+"_data"+str(plot_eta_data)+"_sg  ->Rebin(" +str(rebin_eta)+ "); \n"
        macro += "   double N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[25]; \n"
        macro += "   for (int i = 0; i < 25; i++){ \n"
        macro += "       double Signal_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_data)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_data)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_data)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_eta_"+chain+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_eta_"+chain+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_eta_"+chain+"_data"+str(plot_eta_data)+" = p0_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(dm_sg)+" + (p1_eta_"+chain+"_data"+str(plot_eta_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = (Signal_eta_"+chain+"_data"+str(plot_eta_data)+" - NumB_eta_"+chain+"_data"+str(plot_eta_data)+");  \n"
        macro += "       if ( (Signal_eta_"+chain+"_data"+str(plot_eta_data)+" - NumB_eta_"+chain+"_data"+str(plot_eta_data)+") < 0 ){ N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = Signal_eta_"+chain+"_data"+str(plot_eta_data)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_eta_"+chain+"_data"+str(plot_eta_data)+"[25]; \n"
        macro += "     double eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[25];  \n"
        macro += "     for (int i = 0; i < 25; i++){ \n"
        macro += "          if ( N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] == 0 || N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] == 0 ){  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = 0;  \n" 
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i];  \n"
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = ( N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] )*sqrt( 1/N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] + 1/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_eta_"+chain+"_data"+str(plot_eta_data)+" = new TGraphErrors( 25, x_eta, eff_eta_"+chain+"_data"+str(plot_eta_data)+", xerr_eta, eff_err_eta_"+chain+"_data"+str(plot_eta_data)+" ); \n"
    
        if len(mcFile) == 0 :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{1})\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_eta_"+chain+"_data"+str(plot_eta_data)+", \"Data sample : "+Name_dataFile[plot_eta_data-1]+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1


    macro += " leg_"+chain+" ->SetFillColor(10); \n"
    macro += " leg_"+chain+" ->SetLineColor(10); \n"
    macro += " leg_"+chain+" ->SetTextSize(0.025); \n"
    macro += " leg_"+chain+" ->Draw(\"same\"); \n"
   
    macro += "\n"
    macro += "\n"
    macro += "\n"


### Plot for each file
## =================================================
## plot : Eff vs pt
## =================================================
plot_pt_tot = 0
for mc_file in mcFile :
    plot_pt_tot = plot_pt_tot + 1

    can_ind_pt = 3 + plot_pt_tot
    pos_can_ind_pt = 90+20*plot_pt_tot
    macro += "   c"+str(can_ind_pt)+" = new TCanvas(\"HF"+str(can_ind_pt)+"\", \"Histos"+str(can_ind_pt)+"\", "+str(pos_can_ind_pt)+", "+str(pos_can_ind_pt)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_mcFile[plot_pt_tot-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_mcFile[plot_pt_tot-1]+" -> SetHeader(\" MC ["+Name_mcFile[plot_pt_tot-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    plot_pt_mc = 0
    # Monte Carlo
    for chain in chains :
    
        plot_pt_mc = plot_pt_mc + 1
        macro += "  TGraphAsymmErrors *g_pt_"+chain+"_mc"+str(plot_pt_tot)+" = new TGraphAsymmErrors( h_mu1_pt_"+chain+"_mc"+str(plot_pt_tot)+"_sg, h_mu1_pt_"+denominator+"_mc"+str(plot_pt_tot)+"_sg ); \n"
        if plot_pt_mc == 1 :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> GetXaxis()->SetTitle(\"pT(#mu_{1}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_mcFile[plot_pt_tot-1]+" ->AddEntry( g_pt_"+chain+"_mc"+str(plot_pt_tot)+", \""+chain+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    macro += " leg_"+Name_mcFile[plot_pt_tot-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot-1]+" ->Draw(\"same\"); \n"


plot_pt_tot = 0
for data_file in dataFile :
    plot_pt_tot = plot_pt_tot + 1

    can_ind_pt_data = 3 + plot_pt_tot
    pos_can_ind_pt_data = 90+20*plot_pt_tot
    if (len(mcFile) != 0):
        can_ind_pt_data = can_ind_pt + plot_pt_tot
        pos_can_ind_pt_data = pos_can_ind_pt+20*plot_pt_tot
    macro += "   c"+str(can_ind_pt_data)+" = new TCanvas(\"HF"+str(can_ind_pt_data)+"\", \"Histos"+str(can_ind_pt_data)+"\", "+str(pos_can_ind_pt_data)+", "+str(pos_can_ind_pt_data)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_dataFile[plot_pt_tot-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_dataFile[plot_pt_tot-1]+" -> SetHeader(\" Data ["+Name_dataFile[plot_pt_tot-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    plot_pt_data = 0
    for chain in chains :
        plot_pt_data = plot_pt_data + 1
        if plot_pt_data == 1 :
            macro += "   double N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[20]; \n"
            macro += "   for (int i = 0; i < 20; i++){ \n"
            macro += "       double Signal_pt_"+denominator+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_tot)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_tot)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+denominator+"_data"+str(plot_pt_tot)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_pt_"+denominator+"_data"+str(plot_pt_tot)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_pt_"+denominator+"_data"+str(plot_pt_tot)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_pt_"+denominator+"_data"+str(plot_pt_tot)+" = p0_pt_"+denominator+"_data"+str(plot_pt_tot)+"*"+str(dm_sg)+" + (p1_pt_"+denominator+"_data"+str(plot_pt_tot)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i] = (Signal_pt_"+denominator+"_data"+str(plot_pt_tot)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_tot)+");  \n"
            macro += "       if ( (Signal_pt_"+denominator+"_data"+str(plot_pt_tot)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_tot)+") < 0 ){ N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i] = Signal_pt_"+denominator+"_data"+str(plot_pt_tot)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x[20]; \n"
            macro += "    double xerr[20]; \n"
            macro += "    for (int i = 0; i < 20; i++){ \n"
            macro += "           x[i] = 0.5 + i; \n"
            macro += "           xerr[i] = 0.5; \n"
            macro += "     } \n"


        macro += "   double N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[20]; \n"
        macro += "   for (int i = 0; i < 20; i++){ \n"
        macro += "       double Signal_pt_"+chain+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_tot)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_tot)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot)+" = h_mu1_pt_"+chain+"_data"+str(plot_pt_tot)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_pt_"+chain+"_data"+str(plot_pt_tot)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_pt_"+chain+"_data"+str(plot_pt_tot)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_pt_"+chain+"_data"+str(plot_pt_tot)+" = p0_pt_"+chain+"_data"+str(plot_pt_tot)+"*"+str(dm_sg)+" + (p1_pt_"+chain+"_data"+str(plot_pt_tot)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = (Signal_pt_"+chain+"_data"+str(plot_pt_tot)+" - NumB_pt_"+chain+"_data"+str(plot_pt_tot)+");  \n"
        macro += "       if ( (Signal_pt_"+chain+"_data"+str(plot_pt_tot)+" - NumB_pt_"+chain+"_data"+str(plot_pt_tot)+") < 0 ){ N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = Signal_pt_"+chain+"_data"+str(plot_pt_tot)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_pt_"+chain+"_data"+str(plot_pt_tot)+"[20]; \n"
        macro += "     double eff_err_pt_"+chain+"_data"+str(plot_pt_tot)+"[20];  \n"
        macro += "     for (int i = 0; i < 20; i++){ \n"
        macro += "          if ( N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] == 0 || N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i] == 0 ){  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = 0;  \n" 
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i];  \n"
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] = ( N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i] )*sqrt( 1/N_sig_pt_"+chain+"_data"+str(plot_pt_tot)+"[i] + 1/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_pt_"+chain+"_data"+str(plot_pt_tot)+" = new TGraphErrors( 20, x, eff_pt_"+chain+"_data"+str(plot_pt_tot)+", xerr, eff_err_pt_"+chain+"_data"+str(plot_pt_tot)+" ); \n"
    
        if plot_pt_data == 1 :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> GetXaxis()->SetTitle(\"pT(#mu_{1}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_dataFile[plot_pt_tot-1]+" ->AddEntry( g_pt_"+chain+"_data"+str(plot_pt_tot)+", \""+chain+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    macro += " leg_"+Name_dataFile[plot_pt_tot-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot-1]+" ->Draw(\"same\"); \n"


   
    macro += "\n"



## =================================================
## plot : Eff vs eta
## =================================================
plot_eta_tot = 0
for mc_file in mcFile :
    plot_eta_tot = plot_eta_tot + 1

    can_ind_eta = can_ind_pt  + plot_eta_tot
    if (len(dataFile) != 0 ) :
        can_ind_eta = can_ind_pt_data  + plot_eta_tot
    pos_can_ind_eta = 90+20*plot_eta_tot
    macro += "   c"+str(can_ind_eta)+" = new TCanvas(\"HF"+str(can_ind_eta)+"\", \"Histos"+str(can_ind_eta)+"\", "+str(pos_can_ind_eta)+", "+str(pos_can_ind_eta)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_mcFile[plot_eta_tot-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_mcFile[plot_eta_tot-1]+" -> SetHeader(\" MC ["+Name_mcFile[plot_eta_tot-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    plot_eta_mc = 0
    # Monte Carlo
    for chain in chains :
    
        plot_eta_mc = plot_eta_mc + 1
        macro += "  TGraphAsymmErrors *g_eta_"+chain+"_mc"+str(plot_eta_tot)+" = new TGraphAsymmErrors( h_mu1_eta_"+chain+"_mc"+str(plot_eta_tot)+"_sg, h_mu1_eta_"+denominator+"_mc"+str(plot_eta_tot)+"_sg ); \n"
        if plot_eta_mc == 1 :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{1})\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_mcFile[plot_eta_tot-1]+" ->AddEntry( g_eta_"+chain+"_mc"+str(plot_eta_tot)+", \""+chain+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    macro += " leg_"+Name_mcFile[plot_eta_tot-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot-1]+" ->Draw(\"same\"); \n"


plot_eta_tot = 0
for data_file in dataFile :
    plot_eta_tot = plot_eta_tot + 1
    
    can_ind_eta_data = can_ind_pt_data + plot_eta_tot
    pos_can_ind_eta_data = pos_can_ind_pt_data+20*plot_eta_tot
    if (len(mcFile) != 0):
        can_ind_eta_data = can_ind_eta + plot_eta_tot
        pos_can_ind_eta_data = pos_can_ind_eta+20*plot_eta_tot
    macro += "   c"+str(can_ind_eta_data)+" = new TCanvas(\"HF"+str(can_ind_eta_data)+"\", \"Histos"+str(can_ind_eta_data)+"\", "+str(pos_can_ind_eta_data)+", "+str(pos_can_ind_eta_data)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_dataFile[plot_eta_tot-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_dataFile[plot_eta_tot-1]+" -> SetHeader(\" Data ["+Name_dataFile[plot_eta_tot-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    plot_eta_data = 0
    for chain in chains :
        plot_eta_data = plot_eta_data + 1
        if plot_eta_data == 1 :
            macro += "   double N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[25]; \n"
            macro += "   for (int i = 0; i < 25; i++){ \n"
            macro += "       double Signal_eta_"+denominator+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_tot)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_tot)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+denominator+"_data"+str(plot_eta_tot)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_eta_"+denominator+"_data"+str(plot_eta_tot)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_eta_"+denominator+"_data"+str(plot_eta_tot)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_eta_"+denominator+"_data"+str(plot_eta_tot)+" = p0_eta_"+denominator+"_data"+str(plot_eta_tot)+"*"+str(dm_sg)+" + (p1_eta_"+denominator+"_data"+str(plot_eta_tot)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i] = (Signal_eta_"+denominator+"_data"+str(plot_eta_tot)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_tot)+");  \n"
            macro += "       if ( (Signal_eta_"+denominator+"_data"+str(plot_eta_tot)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_tot)+") < 0 ){ N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i] = Signal_eta_"+denominator+"_data"+str(plot_eta_tot)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x_eta[25]; \n"
            macro += "    double xerr_eta[25]; \n"
            macro += "    for (int i = 0; i < 25; i++){ \n"
            macro += "           x_eta[i] = -2.4 + 0.2*(i); \n"
            macro += "           xerr_eta[i] = 0.1; \n"
            macro += "     } \n"

        macro += "   double N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[25]; \n"
        macro += "   for (int i = 0; i < 25; i++){ \n"
        macro += "       double Signal_eta_"+chain+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_tot)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_tot)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot)+" = h_mu1_eta_"+chain+"_data"+str(plot_eta_tot)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_eta_"+chain+"_data"+str(plot_eta_tot)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_eta_"+chain+"_data"+str(plot_eta_tot)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_eta_"+chain+"_data"+str(plot_eta_tot)+" = p0_eta_"+chain+"_data"+str(plot_eta_tot)+"*"+str(dm_sg)+" + (p1_eta_"+chain+"_data"+str(plot_eta_tot)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = (Signal_eta_"+chain+"_data"+str(plot_eta_tot)+" - NumB_eta_"+chain+"_data"+str(plot_eta_tot)+");  \n"
        macro += "       if ( (Signal_eta_"+chain+"_data"+str(plot_eta_tot)+" - NumB_eta_"+chain+"_data"+str(plot_eta_tot)+") < 0 ){ N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = Signal_eta_"+chain+"_data"+str(plot_eta_tot)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_eta_"+chain+"_data"+str(plot_eta_tot)+"[25]; \n"
        macro += "     double eff_err_eta_"+chain+"_data"+str(plot_eta_tot)+"[25];  \n"
        macro += "     for (int i = 0; i < 25; i++){ \n"
        macro += "          if ( N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] == 0 || N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i] == 0 ){  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = 0;  \n" 
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i];  \n"
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] = ( N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i] )*sqrt( 1/N_sig_eta_"+chain+"_data"+str(plot_eta_tot)+"[i] + 1/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_eta_"+chain+"_data"+str(plot_eta_tot)+" = new TGraphErrors( 25, x_eta, eff_eta_"+chain+"_data"+str(plot_eta_tot)+", xerr_eta, eff_err_eta_"+chain+"_data"+str(plot_eta_tot)+" ); \n"
    
        if plot_eta_data == 1 :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{1})\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_dataFile[plot_eta_tot-1]+" ->AddEntry( g_eta_"+chain+"_data"+str(plot_eta_tot)+", \""+chain+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    macro += " leg_"+Name_dataFile[plot_eta_tot-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot-1]+" ->Draw(\"same\"); \n"

    macro += "\n"



## ==============================================
## Efficiency vs pT(mu2) Plots
## ==============================================
numb_c2col = len(chains)
if len(chains) > 4 :
    numb_c2col = 4
c2col = numb_c2col * 300
numb_c2row = len(chains)//4
if (len(chains)//4) < 1 :
    numb_c2row = 1
if (len(chains)/4.) > 1 :
    numb_c2row = ( (len(chains)//4)  + 1)
c2row = numb_c2row * 300

macro += "   c12 = new TCanvas(\"HF12\", \"Histos12\", 70, 70,"+str(c2col)+", "+str(c2row)+"); \n"
macro += "   c12->Divide("+str(numb_c2col)+","+str(numb_c2row)+"); \n"

## =================================================
## plot
## =================================================
# Rebin
rebin_pt = 2
plot_pt_tot = 0

for chain in chains :
    plot_pt_tot = plot_pt_tot + 1
    macro += "   c12->cd("+str(plot_pt_tot)+");\n"
    macro += "   TLegend *leg_"+chain+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+chain+" -> SetHeader(\""+chain+ " w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    # Monte Carlo
    plot_pt_mc = 0
    for mc_file in mcFile :
        plot_pt_mc = plot_pt_mc + 1
        if plot_pt_tot == 1 :
            macro += "   h_mu2_pt_"+denominator+"_mc"+str(plot_pt_mc)+"_sg ->Rebin(" +str(rebin_pt)+ "); \n"
        macro += "   h_mu2_pt_"+chain+"_mc"+str(plot_pt_mc)+"_sg  ->Rebin(" +str(rebin_pt)+ "); \n"
    
        macro += "  TGraphAsymmErrors *g_pt_"+chain+"_mc"+str(plot_pt_mc)+" = new TGraphAsymmErrors( h_mu2_pt_"+chain+"_mc"+str(plot_pt_mc)+"_sg, h_mu2_pt_"+denominator+"_mc"+str(plot_pt_mc)+"_sg ); \n"
        if plot_pt_mc == 1 :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetXaxis()->SetTitle(\"pT(#mu_{2}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_mc)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_pt_"+chain+"_mc"+str(plot_pt_mc)+", \"MC sample : "+Name_mcFile[plot_pt_mc-1]+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    ## Real Data
    plot_pt_data = 0    
    for data_file in dataFile :
        plot_pt_data = plot_pt_data + 1
        if plot_pt_tot == 1 :
            macro += "   h_mu2_pt_"+denominator+"_data"+str(plot_pt_data)+"_sg ->Rebin(" +str(rebin_pt)+ "); \n"
            macro += "   double N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[20]; \n"
            macro += "   for (int i = 0; i < 20; i++){ \n"
            macro += "       double Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_data)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_data)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_data)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_pt_"+denominator+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_pt_"+denominator+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_data)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_pt_"+denominator+"_data"+str(plot_pt_data)+" = p0_pt_"+denominator+"_data"+str(plot_pt_data)+"*"+str(dm_sg)+" + (p1_pt_"+denominator+"_data"+str(plot_pt_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] = (Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_data)+");  \n"
            macro += "       if ( (Signal_pt_"+denominator+"_data"+str(plot_pt_data)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_data)+") < 0 ){ N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] = Signal_pt_"+denominator+"_data"+str(plot_pt_data)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x[20]; \n"
            macro += "    double xerr[20]; \n"
            macro += "    for (int i = 0; i < 20; i++){ \n"
            macro += "           x[i] = 0.5 + i; \n"
            macro += "           xerr[i] = 0.5; \n"
            macro += "     } \n"


        macro += "   h_mu2_pt_"+chain+"_data"+str(plot_pt_data)+"_sg  ->Rebin(" +str(rebin_pt)+ "); \n"
        macro += "   double N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[20]; \n"
        macro += "   for (int i = 0; i < 20; i++){ \n"
        macro += "       double Signal_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_data)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_data)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_data)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_pt_"+chain+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_pt_"+chain+"_data"+str(plot_pt_data)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_data)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_pt_"+chain+"_data"+str(plot_pt_data)+" = p0_pt_"+chain+"_data"+str(plot_pt_data)+"*"+str(dm_sg)+" + (p1_pt_"+chain+"_data"+str(plot_pt_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = (Signal_pt_"+chain+"_data"+str(plot_pt_data)+" - NumB_pt_"+chain+"_data"+str(plot_pt_data)+");  \n"
        macro += "       if ( (Signal_pt_"+chain+"_data"+str(plot_pt_data)+" - NumB_pt_"+chain+"_data"+str(plot_pt_data)+") < 0 ){ N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = Signal_pt_"+chain+"_data"+str(plot_pt_data)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_pt_"+chain+"_data"+str(plot_pt_data)+"[20]; \n"
        macro += "     double eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[20];  \n"
        macro += "     for (int i = 0; i < 20; i++){ \n"
        macro += "          if ( N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] == 0 || N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] == 0 ){  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = 0;  \n" 
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i];  \n"
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_data)+"[i] = ( N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] )*sqrt( 1/N_sig_pt_"+chain+"_data"+str(plot_pt_data)+"[i] + 1/N_sig_pt_"+denominator+"_data"+str(plot_pt_data)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_pt_"+chain+"_data"+str(plot_pt_data)+" = new TGraphErrors( 20, x, eff_pt_"+chain+"_data"+str(plot_pt_data)+", xerr, eff_err_pt_"+chain+"_data"+str(plot_pt_data)+" ); \n"
    
        if len(mcFile) == 0 :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetXaxis()->SetTitle(\"pT(#mu_{2}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_data)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_pt_"+chain+"_data"+str(plot_pt_data)+", \"Data sample : "+Name_dataFile[plot_pt_data-1]+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1


    macro += " leg_"+chain+" ->SetFillColor(10); \n"
    macro += " leg_"+chain+" ->SetLineColor(10); \n"
    macro += " leg_"+chain+" ->SetTextSize(0.025); \n"
    macro += " leg_"+chain+" ->Draw(\"same\"); \n"
   
    macro += "\n"





## ==============================================
## Efficiency vs eta(mu2) Plots
## ==============================================
macro += "   c13 = new TCanvas(\"HF13\", \"Histos13\", 90, 90,"+str(c2col)+", "+str(c2row)+"); \n"
macro += "   c13->Divide("+str(numb_c2col)+","+str(numb_c2row)+"); \n"

## =================================================
## plot
## =================================================
# Rebin
rebin_eta = 2
plot_eta_tot = 0

for chain in chains :
    plot_eta_tot = plot_eta_tot + 1
    macro += "   c13->cd("+str(plot_eta_tot)+");\n"
    macro += "   TLegend *leg_"+chain+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+chain+" -> SetHeader(\""+chain+ " w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    # Monte Carlo
    plot_eta_mc = 0
    for mc_file in mcFile :
        plot_eta_mc = plot_eta_mc + 1
        if plot_eta_tot == 1 :
            macro += "   h_mu2_eta_"+denominator+"_mc"+str(plot_eta_mc)+"_sg ->Rebin(" +str(rebin_eta)+ "); \n"
        macro += "   h_mu2_eta_"+chain+"_mc"+str(plot_eta_mc)+"_sg  ->Rebin(" +str(rebin_eta)+ "); \n"
    
        macro += "  TGraphAsymmErrors *g_eta_"+chain+"_mc"+str(plot_eta_mc)+" = new TGraphAsymmErrors( h_mu2_eta_"+chain+"_mc"+str(plot_eta_mc)+"_sg, h_mu2_eta_"+denominator+"_mc"+str(plot_eta_mc)+"_sg ); \n"
        if plot_eta_mc == 1 :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{2}) \");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_mc)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_eta_"+chain+"_mc"+str(plot_eta_mc)+", \"MC sample : "+Name_mcFile[plot_eta_mc-1]+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    ## Real Data
    plot_eta_data = 0    
    for data_file in dataFile :
        plot_eta_data = plot_eta_data + 1
        if plot_eta_tot == 1 :
            macro += "   h_mu2_eta_"+denominator+"_data"+str(plot_eta_data)+"_sg ->Rebin(" +str(rebin_eta)+ "); \n"
            macro += "   double N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[25]; \n"
            macro += "   for (int i = 0; i < 25; i++){ \n"
            macro += "       double Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_data)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_data)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_data)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_eta_"+denominator+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_eta_"+denominator+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_data)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_eta_"+denominator+"_data"+str(plot_eta_data)+" = p0_eta_"+denominator+"_data"+str(plot_eta_data)+"*"+str(dm_sg)+" + (p1_eta_"+denominator+"_data"+str(plot_eta_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] = (Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_data)+");  \n"
            macro += "       if ( (Signal_eta_"+denominator+"_data"+str(plot_eta_data)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_data)+") < 0 ){ N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] = Signal_eta_"+denominator+"_data"+str(plot_eta_data)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x_eta[25]; \n"
            macro += "    double xerr_eta[25]; \n"
            macro += "    for (int i = 0; i < 25; i++){ \n"
            macro += "           x_eta[i] = -2.4 + 0.2*(i); \n"
            macro += "           xerr_eta[i] = 0.1; \n"
            macro += "     } \n"


        macro += "   h_mu2_eta_"+chain+"_data"+str(plot_eta_data)+"_sg  ->Rebin(" +str(rebin_eta)+ "); \n"
        macro += "   double N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[25]; \n"
        macro += "   for (int i = 0; i < 25; i++){ \n"
        macro += "       double Signal_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_data)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_data)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_data)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_eta_"+chain+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_eta_"+chain+"_data"+str(plot_eta_data)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_data)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_data)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_eta_"+chain+"_data"+str(plot_eta_data)+" = p0_eta_"+chain+"_data"+str(plot_eta_data)+"*"+str(dm_sg)+" + (p1_eta_"+chain+"_data"+str(plot_eta_data)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = (Signal_eta_"+chain+"_data"+str(plot_eta_data)+" - NumB_eta_"+chain+"_data"+str(plot_eta_data)+");  \n"
        macro += "       if ( (Signal_eta_"+chain+"_data"+str(plot_eta_data)+" - NumB_eta_"+chain+"_data"+str(plot_eta_data)+") < 0 ){ N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = Signal_eta_"+chain+"_data"+str(plot_eta_data)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_eta_"+chain+"_data"+str(plot_eta_data)+"[25]; \n"
        macro += "     double eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[25];  \n"
        macro += "     for (int i = 0; i < 25; i++){ \n"
        macro += "          if ( N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] == 0 || N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] == 0 ){  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = 0;  \n" 
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i];  \n"
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_data)+"[i] = ( N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] )*sqrt( 1/N_sig_eta_"+chain+"_data"+str(plot_eta_data)+"[i] + 1/N_sig_eta_"+denominator+"_data"+str(plot_eta_data)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_eta_"+chain+"_data"+str(plot_eta_data)+" = new TGraphErrors( 25, x_eta, eff_eta_"+chain+"_data"+str(plot_eta_data)+", xerr_eta, eff_err_eta_"+chain+"_data"+str(plot_eta_data)+" ); \n"
    
        if len(mcFile) == 0 :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{2})\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_data)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+chain+" ->AddEntry( g_eta_"+chain+"_data"+str(plot_eta_data)+", \"Data sample : "+Name_dataFile[plot_eta_data-1]+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1


    macro += " leg_"+chain+" ->SetFillColor(10); \n"
    macro += " leg_"+chain+" ->SetLineColor(10); \n"
    macro += " leg_"+chain+" ->SetTextSize(0.025); \n"
    macro += " leg_"+chain+" ->Draw(\"same\"); \n"
   
    macro += "\n"
    macro += "\n"
    macro += "\n"


### Plot for each file
## =================================================
## plot : Eff vs pt
## =================================================
plot_pt_tot_2 = 0
for mc_file in mcFile :
    plot_pt_tot_2 = plot_pt_tot_2 + 1

    can_ind_pt = 13 + plot_pt_tot_2
    pos_can_ind_pt = 90+20*plot_pt_tot_2
    macro += "   c"+str(can_ind_pt)+" = new TCanvas(\"HF"+str(can_ind_pt)+"\", \"Histos"+str(can_ind_pt)+"\", "+str(pos_can_ind_pt)+", "+str(pos_can_ind_pt)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_mcFile[plot_pt_tot_2-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_mcFile[plot_pt_tot_2-1]+" -> SetHeader(\" MC ["+Name_mcFile[plot_pt_tot_2-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    plot_pt_mc = 0
    # Monte Carlo
    for chain in chains :
    
        plot_pt_mc = plot_pt_mc + 1
        macro += "  TGraphAsymmErrors *g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" = new TGraphAsymmErrors( h_mu2_pt_"+chain+"_mc"+str(plot_pt_tot_2)+"_sg, h_mu2_pt_"+denominator+"_mc"+str(plot_pt_tot_2)+"_sg ); \n"
        if plot_pt_mc == 1 :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> GetXaxis()->SetTitle(\"pT(#mu_{2}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_mcFile[plot_pt_tot_2-1]+" ->AddEntry( g_pt_"+chain+"_mc"+str(plot_pt_tot_2)+", \""+chain+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    macro += " leg_"+Name_mcFile[plot_pt_tot_2-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot_2-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot_2-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_mcFile[plot_pt_tot_2-1]+" ->Draw(\"same\"); \n"


plot_pt_tot_2 = 0
for data_file in dataFile :
    plot_pt_tot_2 = plot_pt_tot_2 + 1

    can_ind_pt_data = 13 + plot_pt_tot_2
    pos_can_ind_pt_data = 90+20*plot_pt_tot_2
    if (len(mcFile) != 0):
        can_ind_pt_data = can_ind_pt + plot_pt_tot_2
        pos_can_ind_pt_data = pos_can_ind_pt+20*plot_pt_tot_2
    macro += "   c"+str(can_ind_pt_data)+" = new TCanvas(\"HF"+str(can_ind_pt_data)+"\", \"Histos"+str(can_ind_pt_data)+"\", "+str(pos_can_ind_pt_data)+", "+str(pos_can_ind_pt_data)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_dataFile[plot_pt_tot_2-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_dataFile[plot_pt_tot_2-1]+" -> SetHeader(\" Data ["+Name_dataFile[plot_pt_tot_2-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_pt = 0
    plot_pt_data = 0
    for chain in chains :
        plot_pt_data = plot_pt_data + 1
        if plot_pt_data == 1 :
            macro += "   double N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[20]; \n"
            macro += "   for (int i = 0; i < 20; i++){ \n"
            macro += "       double Signal_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = (bkgd_1_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" - bkgd_2_pt_"+denominator+"_data"+str(plot_pt_tot_2)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" = p0_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"*"+str(dm_sg)+" + (p1_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i] = (Signal_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_tot_2)+");  \n"
            macro += "       if ( (Signal_pt_"+denominator+"_data"+str(plot_pt_tot_2)+" - NumB_pt_"+denominator+"_data"+str(plot_pt_tot_2)+") < 0 ){ N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i] = Signal_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x[20]; \n"
            macro += "    double xerr[20]; \n"
            macro += "    for (int i = 0; i < 20; i++){ \n"
            macro += "           x[i] = 0.5 + i; \n"
            macro += "           xerr[i] = 0.5; \n"
            macro += "     } \n"


        macro += "   double N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[20]; \n"
        macro += "   for (int i = 0; i < 20; i++){ \n"
        macro += "       double Signal_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_tot_2)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_tot_2)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = h_mu2_pt_"+chain+"_data"+str(plot_pt_tot_2)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot_2)+"*"+str(mean_bk2)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot_2)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = (bkgd_1_pt_"+chain+"_data"+str(plot_pt_tot_2)+" - bkgd_2_pt_"+chain+"_data"+str(plot_pt_tot_2)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = p0_pt_"+chain+"_data"+str(plot_pt_tot_2)+"*"+str(dm_sg)+" + (p1_pt_"+chain+"_data"+str(plot_pt_tot_2)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = (Signal_pt_"+chain+"_data"+str(plot_pt_tot_2)+" - NumB_pt_"+chain+"_data"+str(plot_pt_tot_2)+");  \n"
        macro += "       if ( (Signal_pt_"+chain+"_data"+str(plot_pt_tot_2)+" - NumB_pt_"+chain+"_data"+str(plot_pt_tot_2)+") < 0 ){ N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = Signal_pt_"+chain+"_data"+str(plot_pt_tot_2)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[20]; \n"
        macro += "     double eff_err_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[20];  \n"
        macro += "     for (int i = 0; i < 20; i++){ \n"
        macro += "          if ( N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] == 0 || N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i] == 0 ){  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = 0;  \n" 
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i];  \n"
        macro += "              eff_err_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] = ( N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i]/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i] )*sqrt( 1/N_sig_pt_"+chain+"_data"+str(plot_pt_tot_2)+"[i] + 1/N_sig_pt_"+denominator+"_data"+str(plot_pt_tot_2)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" = new TGraphErrors( 20, x, eff_pt_"+chain+"_data"+str(plot_pt_tot_2)+", xerr, eff_err_pt_"+chain+"_data"+str(plot_pt_tot_2)+" ); \n"
    
        if plot_pt_data == 1 :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMaximum(1.05);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMinimum(0);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetTitle(\"\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> GetXaxis()->SetTitle(\"pT(#mu_{2}) [GeV]\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetLineColor("+str(Color[color_pt])+");\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_pt_"+chain+"_data"+str(plot_pt_tot_2)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_dataFile[plot_pt_tot_2-1]+" ->AddEntry( g_pt_"+chain+"_data"+str(plot_pt_tot_2)+", \""+chain+"\"  , \"lp\"); \n"
        color_pt = color_pt + 1

    macro += " leg_"+Name_dataFile[plot_pt_tot_2-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot_2-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot_2-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_dataFile[plot_pt_tot_2-1]+" ->Draw(\"same\"); \n"


   
    macro += "\n"



## =================================================
## plot : Eff vs eta
## =================================================
plot_eta_tot_2 = 0
for mc_file in mcFile :
    plot_eta_tot_2 = plot_eta_tot_2 + 1

    can_ind_eta = can_ind_pt  + plot_eta_tot_2
    if (len(dataFile) != 0 ) :
        can_ind_eta = can_ind_pt_data  + plot_eta_tot_2
    pos_can_ind_eta = 90+20*plot_eta_tot_2
    macro += "   c"+str(can_ind_eta)+" = new TCanvas(\"HF"+str(can_ind_eta)+"\", \"Histos"+str(can_ind_eta)+"\", "+str(pos_can_ind_eta)+", "+str(pos_can_ind_eta)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_mcFile[plot_eta_tot_2-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_mcFile[plot_eta_tot_2-1]+" -> SetHeader(\" MC ["+Name_mcFile[plot_eta_tot_2-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    plot_eta_mc = 0
    # Monte Carlo
    for chain in chains :
    
        plot_eta_mc = plot_eta_mc + 1
        macro += "  TGraphAsymmErrors *g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" = new TGraphAsymmErrors( h_mu2_eta_"+chain+"_mc"+str(plot_eta_tot_2)+"_sg, h_mu2_eta_"+denominator+"_mc"+str(plot_eta_tot_2)+"_sg ); \n"
        if plot_eta_mc == 1 :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{2})\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_mcFile[plot_eta_tot_2-1]+" ->AddEntry( g_eta_"+chain+"_mc"+str(plot_eta_tot_2)+", \""+chain+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    macro += " leg_"+Name_mcFile[plot_eta_tot_2-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot_2-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot_2-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_mcFile[plot_eta_tot_2-1]+" ->Draw(\"same\"); \n"


plot_eta_tot_2 = 0
for data_file in dataFile :
    plot_eta_tot_2 = plot_eta_tot_2 + 1
    
    can_ind_eta_data = can_ind_pt_data + plot_eta_tot_2
    pos_can_ind_eta_data = pos_can_ind_pt_data+20*plot_eta_tot_2
    if (len(mcFile) != 0):
        can_ind_eta_data = can_ind_eta + plot_eta_tot_2
        pos_can_ind_eta_data = pos_can_ind_eta+20*plot_eta_tot_2
    macro += "   c"+str(can_ind_eta_data)+" = new TCanvas(\"HF"+str(can_ind_eta_data)+"\", \"Histos"+str(can_ind_eta_data)+"\", "+str(pos_can_ind_eta_data)+", "+str(pos_can_ind_eta_data)+", 500, 500); \n"

    macro += "   TLegend *leg_"+Name_dataFile[plot_eta_tot_2-1]+" = new TLegend(0.4,0.2,0.75,0.4); \n"
    macro += "   leg_"+Name_dataFile[plot_eta_tot_2-1]+" -> SetHeader(\" Data ["+Name_dataFile[plot_eta_tot_2-1]+ "] : w.r.t. "+denominator+"\" );\n"
    color_eta = 0
    plot_eta_data = 0
    for chain in chains :
        plot_eta_data = plot_eta_data + 1
        if plot_eta_data == 1 :
            macro += "   double N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[25]; \n"
            macro += "   for (int i = 0; i < 25; i++){ \n"
            macro += "       double Signal_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"_sg->GetBinContent(i+1); \n"
            macro += "       double bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"_bk1->GetBinContent(i+1); \n"
            macro += "       double bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"_bk2->GetBinContent(i+1); \n"
            macro += "       double p0_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
            macro += "       double p1_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = (bkgd_1_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" - bkgd_2_eta_"+denominator+"_data"+str(plot_eta_tot_2)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
            macro += "       double NumB_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" = p0_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"*"+str(dm_sg)+" + (p1_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
            macro += "       N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i] = (Signal_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_tot_2)+");  \n"
            macro += "       if ( (Signal_eta_"+denominator+"_data"+str(plot_eta_tot_2)+" - NumB_eta_"+denominator+"_data"+str(plot_eta_tot_2)+") < 0 ){ N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i] = Signal_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"; } \n"
         
            macro += " } \n"
               
            macro += "    double x_eta[25]; \n"
            macro += "    double xerr_eta[25]; \n"
            macro += "    for (int i = 0; i < 25; i++){ \n"
            macro += "           x_eta[i] = -2.4 + 0.2*(i); \n"
            macro += "           xerr_eta[i] = 0.1; \n"
            macro += "     } \n"

        macro += "   double N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[25]; \n"
        macro += "   for (int i = 0; i < 25; i++){ \n"
        macro += "       double Signal_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_tot_2)+"_sg->GetBinContent(i+1); \n"
        macro += "       double bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_tot_2)+"_bk1->GetBinContent(i+1); \n"
        macro += "       double bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = h_mu2_eta_"+chain+"_data"+str(plot_eta_tot_2)+"_bk2->GetBinContent(i+1); \n"
        macro += "       double p0_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot_2)+"*"+str(mean_bk2)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot_2)+"*"+str(mean_bk1)+") / ( "+str(dm_bk)+" * ( "+str(mean_bk2)+" - "+str(mean_bk1)+" ) ); \n"
        macro += "       double p1_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = (bkgd_1_eta_"+chain+"_data"+str(plot_eta_tot_2)+" - bkgd_2_eta_"+chain+"_data"+str(plot_eta_tot_2)+") / ( "+str(dm_bk)+"*( "+str(mean_bk1)+" - "+str(mean_bk2)+" ) ); \n"
        macro += "       double NumB_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = p0_eta_"+chain+"_data"+str(plot_eta_tot_2)+"*"+str(dm_sg)+" + (p1_eta_"+chain+"_data"+str(plot_eta_tot_2)+"/2)*( "+str(m_sg2)+"*"+str(m_sg2)+" - "+str(m_sg1)+"*"+str(m_sg1)+" ); \n"
        macro += "       N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = (Signal_eta_"+chain+"_data"+str(plot_eta_tot_2)+" - NumB_eta_"+chain+"_data"+str(plot_eta_tot_2)+");  \n"
        macro += "       if ( (Signal_eta_"+chain+"_data"+str(plot_eta_tot_2)+" - NumB_eta_"+chain+"_data"+str(plot_eta_tot_2)+") < 0 ){ N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = Signal_eta_"+chain+"_data"+str(plot_eta_tot_2)+"; } \n"
        macro += " } \n"       
                
        macro += "     double eff_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[25]; \n"
        macro += "     double eff_err_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[25];  \n"
        macro += "     for (int i = 0; i < 25; i++){ \n"
        macro += "          if ( N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] == 0 || N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i] == 0 ){  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = 0;  \n" 
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = 0; \n" 
        macro += "          }else{  \n"
        macro += "              eff_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i];  \n"
        macro += "              eff_err_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] = ( N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i]/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i] )*sqrt( 1/N_sig_eta_"+chain+"_data"+str(plot_eta_tot_2)+"[i] + 1/N_sig_eta_"+denominator+"_data"+str(plot_eta_tot_2)+"[i] ); \n"
        macro += "          } \n"
        macro += "      } \n"

        macro += " TGraphErrors *g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" = new TGraphErrors( 25, x_eta, eff_eta_"+chain+"_data"+str(plot_eta_tot_2)+", xerr_eta, eff_err_eta_"+chain+"_data"+str(plot_eta_tot_2)+" ); \n"
    
        if plot_eta_data == 1 :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMaximum(1.05);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMinimum(0);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> GetYaxis()->SetNdivisions(510);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetTitle(\"\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> GetXaxis()->SetTitle(\"#eta(#mu_{2})\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> GetYaxis()->SetTitle(\"Efficiency\");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> Draw(\"ap\");\n"
        else :
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerSize(0.7);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetMarkerStyle(20);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetLineColor("+str(Color[color_eta])+");\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> SetLineWidth(1.5);\n"
            macro += " g_eta_"+chain+"_data"+str(plot_eta_tot_2)+" -> Draw(\"psame\");\n"
        
        macro += "   leg_"+Name_dataFile[plot_eta_tot_2-1]+" ->AddEntry( g_eta_"+chain+"_data"+str(plot_eta_tot_2)+", \""+chain+"\"  , \"lp\"); \n"
        color_eta = color_eta + 1

    macro += " leg_"+Name_dataFile[plot_eta_tot_2-1]+" ->SetFillColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot_2-1]+" ->SetLineColor(10); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot_2-1]+" ->SetTextSize(0.025); \n"
    macro += " leg_"+Name_dataFile[plot_eta_tot_2-1]+" ->Draw(\"same\"); \n"

    macro += "\n"




if (SaveFiles) :
    macro += "  c1->SaveAs(\""+savedirPath+"/All_mass.png\") ; \n"
    macro += "  c1->SaveAs(\""+savedirPath+"/All_mass.eps\") ; \n"
    macro += "  c2->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt.png\") ; \n"
    macro += "  c2->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt.eps\") ; \n"
    macro += "  c3->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta.png\") ; \n"
    macro += "  c3->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta.eps\") ; \n"
    count_mc_pt = 3
    for mc_file in mcFile :
        count_mc_pt = count_mc_pt + 1 
        macro += "  c"+str(count_mc_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt_"+Name_mcFile[count_mc_pt - 4]+".png\") ; \n"
        macro += "  c"+str(count_mc_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt_"+Name_mcFile[count_mc_pt - 4]+".eps\") ; \n"
    count_data_pt = count_mc_pt
    for data_file in dataFile :
        count_data_pt = count_data_pt + 1 
        macro += "  c"+str(count_data_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt_"+Name_dataFile[count_data_pt - count_mc_pt - 1]+".png\") ; \n"
        macro += "  c"+str(count_data_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_pt_"+Name_dataFile[count_data_pt - count_mc_pt - 1]+".eps\") ; \n"
    count_mc_eta = count_data_pt
    for mc_file in mcFile :
        count_mc_eta = count_mc_eta + 1
        macro += "  c"+str(count_mc_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta_"+Name_mcFile[count_mc_eta - count_data_pt - 1]+".png\") ; \n"
        macro += "  c"+str(count_mc_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta_"+Name_mcFile[count_mc_eta - count_data_pt - 1]+".eps\") ; \n"
    count_data_eta = count_mc_eta
    for data_file in dataFile :
        count_data_eta = count_data_eta + 1 
        macro += "  c"+str(count_data_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta_"+Name_dataFile[count_data_eta - count_mc_eta - 1]+".png\") ; \n"
        macro += "  c"+str(count_data_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu1_eta_"+Name_dataFile[count_data_eta - count_mc_eta - 1]+".eps\") ; \n"


    macro += "  c12->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt.png\") ; \n"
    macro += "  c12->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt.eps\") ; \n"
    macro += "  c13->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta.png\") ; \n"
    macro += "  c13->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta.eps\") ; \n"
    count_mc_pt = 13
    for mc_file in mcFile :
        count_mc_pt = count_mc_pt + 1 
        macro += "  c"+str(count_mc_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt_"+Name_mcFile[count_mc_pt - 14]+".png\") ; \n"
        macro += "  c"+str(count_mc_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt_"+Name_mcFile[count_mc_pt - 14]+".eps\") ; \n"
    count_data_pt = count_mc_pt
    for data_file in dataFile :
        count_data_pt = count_data_pt + 1 
        macro += "  c"+str(count_data_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt_"+Name_dataFile[count_data_pt - count_mc_pt - 1]+".png\") ; \n"
        macro += "  c"+str(count_data_pt)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_pt_"+Name_dataFile[count_data_pt - count_mc_pt - 1]+".eps\") ; \n"
    count_mc_eta = count_data_pt
    for mc_file in mcFile :
        count_mc_eta = count_mc_eta + 1
        macro += "  c"+str(count_mc_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta_"+Name_mcFile[count_mc_eta - count_data_pt - 1]+".png\") ; \n"
        macro += "  c"+str(count_mc_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta_"+Name_mcFile[count_mc_eta - count_data_pt - 1]+".eps\") ; \n"
    count_data_eta = count_mc_eta
    for data_file in dataFile :
        count_data_eta = count_data_eta + 1 
        macro += "  c"+str(count_data_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta_"+Name_dataFile[count_data_eta - count_mc_eta - 1]+".png\") ; \n"
        macro += "  c"+str(count_data_eta)+"->SaveAs(\""+savedirPath+"/Eff_vs_mu2_eta_"+Name_dataFile[count_data_eta - count_mc_eta - 1]+".eps\") ; \n"


macro += " } \n" 

## create macro
f = open( "makePlots.C", "w" )
f.write( macro )
f.close()

## execute root macro
if( execute and makePlot ) :
  os.system( "root -l  makePlots.C" )

if( execute and not makePlot ) :
  os.system( "root -l -b  makePlots.C" )


## end of script



