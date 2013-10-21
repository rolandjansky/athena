/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TLatex.h"

#include "TROOT.h"
#include "TMath.h"

#include <vector>
#include <iostream>

#define PULL_BOUNDARY "3.5"
#define RES_BOUNDARY "0.09"

int main(int argc, char **argv);

int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);

// "Private members" section ;-)
// the needed arrays for the branch adresses //
  Int_t           nHits;
  Float_t         RecD0;
  Float_t         RecZ0;
  Float_t         RecPhi0;
  Float_t         RecTheta;
  Float_t         RecEta;
  Float_t         RecQoverP;
  Float_t         RecErrD0;
  Float_t         RecErrZ0;
  Float_t         RecErrPhi0;
  Float_t         RecErrTheta;
  Float_t         RecErrQoverP;
  Float_t         Chi2overNdof;
  Int_t           Ndof;

//================ Trying out the style ===================/

  TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
  // Canvas
  plain->SetCanvasBorderMode(0);
  plain->SetCanvasColor(0);
  // Pad
  plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  // Title
  plain->SetTitleColor(0);
  plain->SetTitleBorderSize(0);
  plain->SetTitleX(0.05);
  plain->SetTitleY(0.98);
  // Stat
  plain->SetStatColor(0);
  plain->SetOptStat(1110);
  plain->SetOptFit(111);
  // Labelling
  plain->SetLabelFont(132, "XY");
  plain->SetLabelSize(0.05, "XY");
  
  gROOT->SetStyle("Plain");
  
  const int iCanvWidth = 800;
  const int iCanvHeight= 360;

// =======================================================/

  if (argc > 3) {

    int     bins   = 0;
    int     pcut   = 500;
    int     etacut = 300;
  	
    TString filename = argv[1]; 
    filename        += ".root";
  
    std::cout << "[m] Filename       : " << filename << std::endl;  
    
    TString dirname  = argv[2];
    TString treename = argv[3];

    std::cout << "[m] Directory/Tree : " << dirname << "/"<< treename << std::endl;  

    bins = atoi(argv[4]);

    std::cout << "[m] Histogram bins : " << bins << std::endl;  


    if (argc >4) 
      pcut = atoi(argv[5]);

    double qOverPcut = double(1./pcut);

    TString qOverPLimit = "abs(RecQoverP) <";
            qOverPLimit += qOverPcut; 

    std::cout << "[m] cut q/p        : " << qOverPLimit << std::endl;  

    if (argc >5) 
      etacut = atoi(argv[6]);

    double etaCut = 0.01*double(etacut);

    TString etaLimit  = "abs(RecEta) <";
            etaLimit += etaCut; 

    std::cout << "[m] cut eta        : " << etaLimit << std::endl;  

    TString drawCuts  = qOverPLimit;
            drawCuts += " && ";
            drawCuts += etaLimit; 

    // load the File
    TFile* inputFile = TFile::Open(filename);
    
    TCanvas* helperCanvas = new TCanvas("helperCanvas","Helper Canvas",500,500,100,100);

    if (inputFile){
     
      // construct the full file name 
      TString treefullname = dirname;
      treefullname += "/";
      treefullname += treename;

      TTree* tree = (TTree*)gDirectory->Get(treefullname);

      if (tree){

        std::cout << "[m] Tree " << treefullname << " loaded. " << std::endl;

        std::cout << "[m] Setting Branch Addresses ... " << std::endl;
        // Set the Branch Adresses - not needed yet
        tree->SetBranchAddress("nHits",&nHits);
        tree->SetBranchAddress("RecD0",&RecD0);
        tree->SetBranchAddress("RecZ0",&RecZ0);
        tree->SetBranchAddress("RecPhi0",&RecPhi0);
        tree->SetBranchAddress("RecTheta",&RecTheta);
        tree->SetBranchAddress("RecEta",&RecEta);
        tree->SetBranchAddress("RecQoverP",&RecQoverP);
        tree->SetBranchAddress("RecErrD0",&RecErrD0);
        tree->SetBranchAddress("RecErrZ0",&RecErrZ0);
        tree->SetBranchAddress("RecErrPhi0",&RecErrPhi0);
        tree->SetBranchAddress("RecErrTheta",&RecErrTheta);
        tree->SetBranchAddress("RecErrQoverP",&RecErrQoverP);
        tree->SetBranchAddress("Chi2overNdof",&Chi2overNdof);
        tree->SetBranchAddress("Ndof",&Ndof);
//         tree->SetBranchAddress("nSCTHits",&nSCTHits);
//         tree->SetBranchAddress("nSCTHits",&nSCTHits);
//         tree->SetBranchAddress("nTRTHits",&nTRTHits);


        
        // the stored values
        std::vector<TH1F*>    sctResHistograms;
        std::vector<TString>  sctResHistogramNames;
        std::vector<double>   sctResSigma;
        std::vector<double>   sctResSigmaError;
        std::vector<double>   sctResMean;
        std::vector<double>   sctResMeanError;

        std::vector<TH1F*>    sctPullDistHistograms;
        std::vector<TString>  sctPullDistHistogramNames;
        std::vector<double>   sctPullDistSigma;
        std::vector<double>   sctPullDistSigmaError;
        std::vector<double>   sctPullDistMean;
        std::vector<double>   sctPullDistMeanError;
         
       
// <-----------  SCT SECTION  ------------------------------------------------------------------------->
        // OVERALL SECTION ----------------------------------
        helperCanvas->cd();
         // -------------------------------------------------
        TString sctDraw  = "residualLocX>>sct_all_resX(";
                sctDraw += bins;
                sctDraw += ",-";
		sctDraw += RES_BOUNDARY;
		sctDraw += ",";
		sctDraw += RES_BOUNDARY;
		sctDraw += ")";  
        tree->Draw(sctDraw, "DetectorType==2 && "+drawCuts);
         // -------------------------------------------------
                sctDraw  = "pullLocX>>sct_all_pullX(";
                sctDraw += bins;
                sctDraw += ",-";
		sctDraw += PULL_BOUNDARY;
		sctDraw += ",";
		sctDraw += PULL_BOUNDARY;
		sctDraw += ")";  
        tree->Draw(sctDraw, "DetectorType==2 && "+drawCuts);
        // now draw them -------------------------------------------------------------        
        TCanvas* sctCanv = new TCanvas("sctCanv", "SCT Overall", 100, 100, iCanvWidth, iCanvHeight);
        sctCanv->Divide(2,1);   
        sctCanv->cd(1);
        TH1F* htmp    = (TH1F*)gDirectory->Get("sct_all_resX");
        TString hname = "Residual locX - SCT Overall";
        htmp->SetTitle(hname);
        htmp->Draw();
        htmp->Fit("gaus");
        TF1* gaus    = htmp->GetFunction("gaus");
        double sigma       = gaus->GetParameter(2);
        double sigmaError  = gaus->GetParError(2);
        double mean        = gaus->GetParameter(1);
        double meanError   = gaus->GetParError(1);
        // ------------------------------------------
        sctResHistograms.push_back(htmp);
        sctResHistogramNames.push_back(hname);
        sctResSigma.push_back(sigma);
        sctResSigmaError.push_back(sigmaError);
        sctResMean.push_back(mean);
        sctResMeanError.push_back(meanError);

        sctCanv->cd(2);
        htmp = (TH1F*)gDirectory->Get("sct_all_pullX");
        hname="Pull dist locX - SCT Overall";
        htmp->SetTitle(hname);
        htmp->Draw();          
        htmp->Fit("gaus");
        gaus    = htmp->GetFunction("gaus");
        sigma       = gaus->GetParameter(2);
        sigmaError  = gaus->GetParError(2);
        mean        = gaus->GetParameter(1);
        meanError   = gaus->GetParError(1);
        // ---------------------------------------------
        sctPullDistHistograms.push_back(htmp);
        sctPullDistHistogramNames.push_back(hname);
        sctPullDistSigma.push_back(sigma);
        sctPullDistSigmaError.push_back(sigmaError);
        sctPullDistMean.push_back(mean);
        sctPullDistMeanError.push_back(meanError);
          
        // BARREL SECTION -----------------------------------
        helperCanvas->cd();
         TString sctBarrelDrawCuts = "DetectorType==2 && abs(HitSurfaceZ)<800 && "+drawCuts;
         // -------------------------------------------------
                  sctDraw  = "residualLocX>>sct_barrel_resX(";
                  sctDraw += bins;
                  sctDraw += ",-";
		  sctDraw += RES_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += RES_BOUNDARY;
		  sctDraw += ")";  
          tree->Draw(sctDraw, sctBarrelDrawCuts);
         // -------------------------------------------------
                  sctDraw  = "pullLocX>>sct_barrel_pullX(";
                  sctDraw += bins;
                  sctDraw += ",-";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ")";  
          tree->Draw(sctDraw, sctBarrelDrawCuts);
         // now draw them -------------------------------------------------------------        
         TCanvas* sctBarrelCanv = new TCanvas("sctBarrelCanv", "SCT Barrel", 120, 120, iCanvWidth, iCanvHeight);
         sctBarrelCanv->Divide(2,1);   
         sctBarrelCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("sct_barrel_resX");
          hname="Residual locX - SCT Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          sctResHistograms.push_back(htmp);
          sctResHistogramNames.push_back(hname);
          sctResSigma.push_back(sigma);
          sctResSigmaError.push_back(sigmaError);
          sctResMean.push_back(mean);
          sctResMeanError.push_back(meanError);

         sctBarrelCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("sct_barrel_pullX");
          hname="Pull dist locX - SCT Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          sctPullDistHistograms.push_back(htmp);
          sctPullDistHistogramNames.push_back(hname);
          sctPullDistSigma.push_back(sigma);
          sctPullDistSigmaError.push_back(sigmaError);
          sctPullDistMean.push_back(mean);
          sctPullDistMeanError.push_back(meanError);



        // ENDCAP SECTION A: positive EC -----------------------------------
        helperCanvas->cd();

         TString sctPosEndcapCut = "DetectorType==2 && HitSurfaceZ>800 && "+drawCuts;
         // -------------------------------------------------
                  sctDraw  = "residualLocX>>sct_posendcap_resX(";
                  sctDraw += bins;
                  sctDraw += ",-";
		  sctDraw += RES_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += RES_BOUNDARY;
		  sctDraw += ")";  
          tree->Draw(sctDraw, sctPosEndcapCut);
         // -------------------------------------------------
                  sctDraw  = "pullLocX>>sct_posendcap_pullX(";
                  sctDraw += bins;
                  sctDraw += ",-";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ")";  
          tree->Draw(sctDraw, sctPosEndcapCut);

         // now draw them -------------------------------------------------------------        
         TCanvas* sctPosEndcapCanv = new TCanvas("sctPosEndcapCanv", "SCT Positive Endcap", 140, 140, iCanvWidth, iCanvHeight);
         sctPosEndcapCanv->Divide(2,1);   
         sctPosEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("sct_posendcap_resX");
          hname="Residual locX - Positive Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          sctResHistograms.push_back(htmp);
          sctResHistogramNames.push_back(hname);
          sctResSigma.push_back(sigma);
          sctResSigmaError.push_back(sigmaError);
          sctResMean.push_back(mean);
          sctResMeanError.push_back(meanError);

         sctPosEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("sct_posendcap_pullX");
          hname="Pull dist locX - Positive Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          sctPullDistHistograms.push_back(htmp);
          sctPullDistHistogramNames.push_back(hname);
          sctPullDistSigma.push_back(sigma);
          sctPullDistSigmaError.push_back(sigmaError);
          sctPullDistMean.push_back(mean);
          sctPullDistMeanError.push_back(meanError);

        // ENDCAP SECTION B: negative EC -----------------------------------
        helperCanvas->cd();
        TString sctNegEndcapCut = "DetectorType==2 && HitSurfaceZ<-800 && "+drawCuts;

         // -------------------------------------------------
                  sctDraw  = "residualLocX>>sct_negendcap_resX(";
                  sctDraw += ",-";
	  	  sctDraw += RES_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += RES_BOUNDARY;
		  sctDraw += ")";   
          tree->Draw(sctDraw, sctNegEndcapCut);
         // -------------------------------------------------
                  sctDraw  = "pullLocX>>sct_negendcap_pullX(";
                  sctDraw += bins;
                  sctDraw += ",-";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ",";
		  sctDraw += PULL_BOUNDARY;
		  sctDraw += ")";  
          tree->Draw(sctDraw, sctNegEndcapCut);
         // now draw them -------------------------------------------------------------        
         TCanvas* sctNegEndcapCanv = new TCanvas("sctNegEndcapCanv", "SCT Negative Endcap", 160, 160, iCanvWidth, iCanvHeight);
         sctNegEndcapCanv->Divide(2,1);   
         sctNegEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("sct_negendcap_resX");
          hname="Residual locX - Negative Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          sctResHistograms.push_back(htmp);
          sctResHistogramNames.push_back(hname);
          sctResSigma.push_back(sigma);
          sctResSigmaError.push_back(sigmaError);
          sctResMean.push_back(mean);
          sctResMeanError.push_back(meanError);

         sctNegEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("sct_negendcap_pullX");
          hname="Pull dist locX - Negative Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          sctPullDistHistograms.push_back(htmp);
          sctPullDistHistogramNames.push_back(hname);
          sctPullDistSigma.push_back(sigma);
          sctPullDistSigmaError.push_back(sigmaError);
          sctPullDistMean.push_back(mean);
          sctPullDistMeanError.push_back(meanError);


       // Now per layer --------------------------------------------------------------------
       // --------> Barrel Layers

           std::cout << "[i] Starting dedicated SCT session ... " << std::endl;

           double   radii[5]        = { 290., 360., 430., 500., 550.};

           // SCT Barrel
           for (int ilay = 0; ilay<4; ++ilay){
                 helperCanvas->cd();

                 // The Layer id
                 TString layerId  = "_sct_barrel_layer_";
                         layerId += (ilay+1);

                 // The Layer name
                 TString layerName  = "Layer";
                         layerName += (ilay+1);

                 // the residual in x
                 TString cut  = "DetectorType==2 && HitSurfaceRadius>";
                         cut += radii[ilay];
                         cut += " && HitSurfaceRadius<";
                         cut += radii[ilay+1];
                         cut += " && abs(HitSurfaceZ) < 800 && ";
                         cut += qOverPLimit;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                	 ide += ",-";
			 ide += RES_BOUNDARY;
			 ide += ",";
			 ide += RES_BOUNDARY;
			 ide += ")";  

                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-";
		 	 ide += PULL_BOUNDARY;
			 ide += ",";
			 ide += PULL_BOUNDARY;
			 ide += ")";  
                 tree->Draw(ide, cut);

               // SCT Barrel Canvas
               TCanvas* sctBarrelCanv = new TCanvas("sctBarrelCanv"+layerId, "SCT Barrel - "+layerName, 180+ilay*20, 180+ilay*20, iCanvWidth, iCanvHeight);
               sctBarrelCanv->Divide(2,1);
               sctBarrelCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+layerId);
                 hname="Residual locX - SCT Barrel ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 sctResHistograms.push_back(htmp);
                 sctResHistogramNames.push_back(hname);
                 sctResSigma.push_back(sigma);
                 sctResSigmaError.push_back(sigmaError);
                 sctResMean.push_back(mean);
                 sctResMeanError.push_back(meanError);

               sctBarrelCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+layerId);
                 hname="Pull dist locX - Barrel ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 sctPullDistHistograms.push_back(htmp);
                 sctPullDistHistogramNames.push_back(hname);
                 sctPullDistSigma.push_back(sigma);
                 sctPullDistSigmaError.push_back(sigmaError);
                 sctPullDistMean.push_back(mean);
                 sctPullDistMeanError.push_back(meanError);               
            }
            
           // SCT positive endcap section

           double   zPos[10]       = { 820., 880., 1000., 1270., 1350., 1740., 1800., 2150., 2500., 3000.};
           
           // SCT Positive EC
           for (int ilay = 0; ilay<9; ++ilay){
                 helperCanvas->cd();
                 // layer ID
                 TString layerId  ="_sct_posec_layer";
                         layerId +=(ilay+1);
                 // layer name
                 TString layerName  ="Layer ";
                         layerName +=(ilay+1);                

                 // the residual in x
                 TString cut  = "DetectorType==2 && HitSurfaceZ>";
                         cut += zPos[ilay];
                         cut += " && HitSurfaceZ<";
                         cut += zPos[ilay+1];
                         cut += " && "+qOverPLimit;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                	 ide += ",-";
			 ide += RES_BOUNDARY;
			 ide += ",";
			 ide += RES_BOUNDARY;
			 ide += ")";  
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-";
		 	 ide += PULL_BOUNDARY;
			 ide += ",";
			 ide += PULL_BOUNDARY;
			 ide += ")";  
                 tree->Draw(ide, cut);

                 tree->Draw(ide, cut);
               // SCT Positive Endcap canvas
               TCanvas* sctPosECCanv = new TCanvas("sctPosECCanv"+layerId, "SCT Positive EC - "+layerName, 240+ilay*20, 180+ilay*20, iCanvWidth, iCanvHeight);
               sctPosECCanv->Divide(2,1);
               sctPosECCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+layerId);
                 hname="Residual locX - Positive EC ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 sctResHistograms.push_back(htmp);
                 sctResHistogramNames.push_back(hname);
                 sctResSigma.push_back(sigma);
                 sctResSigmaError.push_back(sigmaError);
                 sctResMean.push_back(mean);
                 sctResMeanError.push_back(meanError);

               sctPosECCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+layerId);
                 hname="Pull dist locX  - Positive EC ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 sctPullDistHistograms.push_back(htmp);
                 sctPullDistHistogramNames.push_back(hname);
                 sctPullDistSigma.push_back(sigma);
                 sctPullDistSigmaError.push_back(sigmaError);
                 sctPullDistMean.push_back(mean);
                 sctPullDistMeanError.push_back(meanError);
           }

           // SCT negative endcap section

           for (int ilay = 0; ilay<9; ++ilay){
                 helperCanvas->cd();
                 // layer ID
                 TString layerId  ="_sct_negec_layer";
                         layerId +=(ilay+1);
                 // layer name
                 TString layerName  ="Layer ";
                         layerName +=(ilay+1);  
                 // the residual in x
                 TString cut  = "DetectorType==2 && HitSurfaceZ<-";
                         cut += zPos[ilay];
                         cut += " && HitSurfaceZ>-";
                         cut += zPos[ilay+1];
                         cut += " && "+qOverPLimit;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                	 ide += ",-";
			 ide += RES_BOUNDARY;
			 ide += ",";
			 ide += RES_BOUNDARY;
			 ide += ")";  
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-";
		 	 ide += PULL_BOUNDARY;
			 ide += ",";
			 ide += PULL_BOUNDARY;
			 ide += ")";  
                 tree->Draw(ide, cut);

               // SCT Negative Endcap canvas
               TCanvas* sctNegECCanv = new TCanvas("sctNegECCanv"+layerId, "SCT Negative EC - "+layerName, 280+ilay*20, 180+ilay*20, iCanvWidth, iCanvHeight);
               sctNegECCanv->Divide(2,1);
               sctNegECCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+layerId);
                 hname="Residual locX - Negative EC ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 sctResHistograms.push_back(htmp);
                 sctResHistogramNames.push_back(hname);
                 sctResSigma.push_back(sigma);
                 sctResSigmaError.push_back(sigmaError);
                 sctResMean.push_back(mean);
                 sctResMeanError.push_back(meanError);

             sctNegECCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+layerId);
                 hname="Pull dist locX  - Negative EC ";
                 hname+=layerName;
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 sctPullDistHistograms.push_back(htmp);
                 sctPullDistHistogramNames.push_back(hname);
                 sctPullDistSigma.push_back(sigma);
                 sctPullDistSigmaError.push_back(sigmaError);
                 sctPullDistMean.push_back(mean);
                 sctPullDistMeanError.push_back(meanError);
                            
            }
        } 


       
       int etaSlices = 10;

       double etaSlice = etaCut/etaSlices;

       Float_t etaValues[etaSlices-1];
       Float_t etaErrors[etaSlices-1];

       // residuals
       Float_t sctResSigmaValues_locX[etaSlices-1];
       Float_t sctResSigmaErrors_locX[etaSlices-1];
       Float_t sctResMeanValues_locX[etaSlices-1];
       Float_t sctResMeanErrors_locX[etaSlices-1];

       // pulls
       Float_t sctPullsSigmaValues_locX[etaSlices-1];
       Float_t sctPullsSigmaErrors_locX[etaSlices-1];
       Float_t sctPullsMeanValues_locX[etaSlices-1];
       Float_t sctPullsMeanErrors_locX[etaSlices-1];



     std::cout << "[m] Starting eta slice loop ... " << std::endl;
     // ------------------------------------------->
     for (int i=0; i<etaSlices-1; ++i){

       // ---------------- Residuals -------------------------
       
       double currentEta = 0.5*(i*etaSlice + (i+1)*etaSlice);
       etaValues[i] = currentEta;
       etaErrors[i] = etaSlice/sqrt(12.);

       // pulls and residuals vs eta do it in the helper canvas
       TString drawCuts_rp  = "abs(RecEta)>";
               drawCuts_rp += i*etaSlice;
               drawCuts_rp += " && ";
               drawCuts_rp += " abs(RecEta)< ";
               drawCuts_rp += (i+1)*etaSlice;
               drawCuts_rp += " && " + qOverPLimit;

       helperCanvas->cd();     

       TString histName     = "sct_resX_etaSlice";
               histName    += i;

       TString sctDraw_r  = "residualLocX>>";
               sctDraw_r += histName;
               sctDraw_r += "(";
               sctDraw_r += bins;
               sctDraw_r += ",-";
	       sctDraw_r += RES_BOUNDARY;
	       sctDraw_r += ",";
	       sctDraw_r += RES_BOUNDARY;
	       sctDraw_r += ")";  
       tree->Draw(sctDraw_r, "DetectorType==2 && "+drawCuts_rp);
       
       TH1F* htmp_r         = (TH1F*)gDirectory->Get(histName);
   
       htmp_r->Fit("gaus");
       TF1* gaus_r          = htmp_r->GetFunction("gaus");
       double sigma_r       = gaus_r->GetParameter(2);
       double sigmaError_r  = gaus_r->GetParError(2);
       double mean_r        = gaus_r->GetParameter(1);
       double meanError_r   = gaus_r->GetParError(1);
       // ----------------------------------------
       sctResSigmaValues_locX[i] = sigma_r;
       sctResSigmaErrors_locX[i] = sigmaError_r;
       sctResMeanValues_locX[i]  = mean_r;
       sctResMeanErrors_locX[i]  = meanError_r;
   
       std::cout << "[m] ResX done for slice : " << i << std::endl;

       // ---------------- Pulls -------------------------

       histName     = "sct_pullX_etaSlice";
       histName    += i;

       TString sctDraw_p  = "pullLocX>>";
               sctDraw_p += histName;
               sctDraw_p += "(";
               sctDraw_p += bins;
               sctDraw_p += ",-";
	       sctDraw_p += PULL_BOUNDARY;
               sctDraw_p += ",";
               sctDraw_p += PULL_BOUNDARY;
               sctDraw_p += ")";  
       tree->Draw(sctDraw_p, "DetectorType==2 && "+drawCuts_rp);
       
       TH1F* htmp_p         = (TH1F*)gDirectory->Get(histName);
       htmp_p->Fit("gaus");
       TF1* gaus_p          = htmp_p->GetFunction("gaus");
       double sigma_p       = gaus_p->GetParameter(2);
       double sigmaError_p  = gaus_p->GetParError(2);
       double mean_p        = gaus_p->GetParameter(1);
       double meanError_p   = gaus_p->GetParError(1);
       // ----------------------------------------
       sctPullsSigmaValues_locX[i] = sigma_p;
       sctPullsSigmaErrors_locX[i] = sigmaError_p;
       sctPullsMeanValues_locX[i]  = mean_p;
       sctPullsMeanErrors_locX[i]  = meanError_p;
      
       std::cout << "[m] PullX done for slice : " << i << std::endl;

      }
 
     // now draw the result !!
     TCanvas* residualSlices = new TCanvas("residualSlices", "Residuals vs. #eta", 400,100,iCanvWidth, iCanvHeight);
     residualSlices->Divide(2,1);
     residualSlices->cd(1);
      TGraphErrors* resXGraph = new TGraphErrors(etaSlices, etaValues, sctResSigmaValues_locX, etaErrors, sctResSigmaErrors_locX);
      resXGraph->SetTitle("#sigma_{locX}");
      resXGraph->SetMarkerStyle(1);
      resXGraph->SetMarkerSize(0.8);
      double xmin, xmax, ymin, ymax;
      resXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* resXGraph_Xaxis = resXGraph->GetXaxis();
      resXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* resXGraph_Yaxis = resXGraph->GetYaxis();
      resXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     resXGraph->Draw("ap");


     residualSlices->cd(2);
      TGraphErrors* meanXGraph = new TGraphErrors(etaSlices, etaValues, sctResMeanValues_locX, etaErrors, sctResMeanErrors_locX);
      meanXGraph->SetTitle("#mu_{locX}");
      meanXGraph->SetMarkerStyle(1);
      meanXGraph->SetMarkerSize(0.8);
      meanXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* meanXGraph_Xaxis = meanXGraph->GetXaxis();
      meanXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* meanXGraph_Yaxis = meanXGraph->GetYaxis();
      meanXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     meanXGraph->Draw("ap");
     //
     TCanvas* pullSlices = new TCanvas("pullSlices", "Pulls vs. #eta", 400,100,iCanvWidth, iCanvHeight);
     pullSlices->Divide(2,1);
     pullSlices->cd(1);
      TGraphErrors* pullXGraph  = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      sctPullsSigmaValues_locX, 
                                                      etaErrors,
                                                      sctPullsSigmaErrors_locX);
      pullXGraph->SetTitle("#sigma_{pulls,locX}");
      pullXGraph->SetMarkerStyle(1);
      pullXGraph->SetMarkerSize(0.8);
      pullXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* pullXGraph_Xaxis = pullXGraph->GetXaxis();
      pullXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullXGraph_Yaxis = pullXGraph->GetYaxis();
      pullXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

   pullXGraph->Draw("ap");
    

     pullSlices->cd(2);
      TGraphErrors* pullMeanXGraph = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      sctPullsMeanValues_locX, 
                                                      etaErrors,
                                                      sctPullsMeanErrors_locX);
      pullMeanXGraph->SetTitle("#mu_{pulls, locX}");
      pullMeanXGraph->SetMarkerStyle(1);
      pullMeanXGraph->SetMarkerSize(0.8);
      pullMeanXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* pullMeanXGraph_Xaxis = pullMeanXGraph->GetXaxis();
      pullMeanXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullMeanXGraph_Yaxis = pullMeanXGraph->GetYaxis();
      pullMeanXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     pullMeanXGraph->Draw("ap");

       // cleanup
       delete helperCanvas;
    }

    } else {
      std::cout << "[e] Not enough arguments given! " << std::endl;
      std::cout << "[i] Arguments required: filename (-.root), Directory, Tree, bins, pcut [MeV] , etacut [eta/100]" << std::endl;
      std::cout << "[i] sample usage for a File 'ID_rec.root' from standard InDetRecExample : " << std::endl;
      std::cout << std::endl;
      std::cout << "   SCT_ResidualsAndPulls ID_rec FitterValidation OutputTrack 65 1000 250" << std::endl;
      std::cout << std::endl;
    }

    theApp.Run();
    return(0);
}
