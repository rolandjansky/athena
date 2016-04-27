/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TApplication.h"
#include "TROOT.h"
#include "TMath.h"
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

#include <vector>
#include <iostream>

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

//================ Setting common plot style ===================/

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
    TString driftTimeCut = "";
    if (argc > 7) {
      TString t7 = argv[7]; // needed for following string comparison
      if (t7 == "Drifttime") {
	driftTimeCut = " && residualLocX/pullLocX < 1.0";
        drawCuts += " && residualLocX/pullLocX < 1.0";
        std::cout << "[m] selecting only TRT hits with drift time meas't" << std::endl;
      } else if (t7 == "NoDrifttime") {
	driftTimeCut = " && residualLocX/pullLocX > 1.0";
        drawCuts += " && residualLocX/pullLocX > 1.0";
        std::cout << "[m] selecting only TRT hits with drift time ignored by tracking" << std::endl;
      } else {
        std::cout << "[m] WARNING drift time selection argument not understood, not doing anything!" << std::endl;
      }
    } 

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
//         tree->SetBranchAddress("nTRT Hits",&nTRT Hits);
//         tree->SetBranchAddress("nTRT Hits",&nTRT Hits);
//         tree->SetBranchAddress("nTRTHits",&nTRTHits);


        
         // the stored values
         std::vector<TH1F*>    trtResHistograms;
         std::vector<TString>  trtResHistogramNames;
         std::vector<double>   trtResSigma;
         std::vector<double>   trtResSigmaError;
         std::vector<double>   trtResMean;
         std::vector<double>   trtResMeanError;

         std::vector<TH1F*>    trtPullDistHistograms;
         std::vector<TString>  trtPullDistHistogramNames;
         std::vector<double>   trtPullDistSigma;
         std::vector<double>   trtPullDistSigmaError;
         std::vector<double>   trtPullDistMean;
         std::vector<double>   trtPullDistMeanError;
         
       
// <-----------  TRT  SECTION  ------------------------------------------------------------------------->
        // OVERALL SECTION ----------------------------------
        helperCanvas->cd();
         // -------------------------------------------------
          TString trtDraw  = "residualLocX>>trt_all_resX(";
                  trtDraw += bins;
                  trtDraw += ",-0.5,0.5)"; 
          tree->Draw(trtDraw, "DetectorType==3 && "+drawCuts);
         // -------------------------------------------------
                  trtDraw  = "pullLocX>>trt_all_pullX(";
                  trtDraw += bins;
                  trtDraw += ",-3.5,3.5)"; 
          tree->Draw(trtDraw, "DetectorType==3 && "+drawCuts);
         // now draw them -------------------------------------------------------------        
         TCanvas* trtOverallCanv = new TCanvas("trtOverallCanv", "TRT Overall", 100, 100, iCanvWidth, iCanvHeight);
         trtOverallCanv->Divide(2,1);   
         trtOverallCanv->cd(1);
          TH1F* htmp    = (TH1F*)gDirectory->Get("trt_all_resX");
          TString hname = "Residual locX - TRT  Overall";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          TF1* gaus    = htmp->GetFunction("gaus");
          double sigma       = gaus->GetParameter(2);
          double sigmaError  = gaus->GetParError(2);
          double mean        = gaus->GetParameter(1);
          double meanError   = gaus->GetParError(1);
          // ------------------------------------------
          trtResHistograms.push_back(htmp);
          trtResHistogramNames.push_back(hname);
          trtResSigma.push_back(sigma);
          trtResSigmaError.push_back(sigmaError);
          trtResMean.push_back(mean);
          trtResMeanError.push_back(meanError);

         trtOverallCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("trt_all_pullX");
          hname="Pull dist locX - TRT  Overall";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          trtPullDistHistograms.push_back(htmp);
          trtPullDistHistogramNames.push_back(hname);
          trtPullDistSigma.push_back(sigma);
          trtPullDistSigmaError.push_back(sigmaError);
          trtPullDistMean.push_back(mean);
          trtPullDistMeanError.push_back(meanError);
          
        // BARREL SECTION -----------------------------------
        helperCanvas->cd();

         TString trtBarrelDrawCuts = "DetectorType==3 && abs(HitSurfaceZ)<800 && "+drawCuts;
         // -------------------------------------------------
                  trtDraw  = "residualLocX>>trt_barrel_resX(";
                  trtDraw += bins;
                  trtDraw += ",-0.5,0.5)"; 
          tree->Draw(trtDraw, trtBarrelDrawCuts);
         // -------------------------------------------------
                  trtDraw  = "pullLocX>>trt_barrel_pullX(";
                  trtDraw += bins;
                  trtDraw += ",-3.5,3.5)"; 
          tree->Draw(trtDraw, trtBarrelDrawCuts);
         // now draw them -------------------------------------------------------------        
         TCanvas* trtBarrelCanv = new TCanvas("trtBarrelCanv", "TRT  Barrel", 120, 120, iCanvWidth, iCanvHeight);
         trtBarrelCanv->Divide(2,1);   
         trtBarrelCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("trt_barrel_resX");
          hname="Residual locX - TRT  Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          trtResHistograms.push_back(htmp);
          trtResHistogramNames.push_back(hname);
          trtResSigma.push_back(sigma);
          trtResSigmaError.push_back(sigmaError);
          trtResMean.push_back(mean);
          trtResMeanError.push_back(meanError);

         trtBarrelCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("trt_barrel_pullX");
          hname="Pull dist locX - TRT  Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          trtPullDistHistograms.push_back(htmp);
          trtPullDistHistogramNames.push_back(hname);
          trtPullDistSigma.push_back(sigma);
          trtPullDistSigmaError.push_back(sigmaError);
          trtPullDistMean.push_back(mean);
          trtPullDistMeanError.push_back(meanError);



        // ENDCAP SECTION A: positive EC -----------------------------------
        helperCanvas->cd();

         TString trtPosEndcapCut = "DetectorType==3 && HitSurfaceZ>800 && "+drawCuts;
         // -------------------------------------------------
                  trtDraw  = "residualLocX>>trt_posendcap_resX(";
                  trtDraw += bins;
                  trtDraw += ",-0.5,0.5)"; 
          tree->Draw(trtDraw, trtPosEndcapCut);
         // -------------------------------------------------
                  trtDraw  = "pullLocX>>trt_posendcap_pullX(";
                  trtDraw += bins;
                  trtDraw += ",-3.5,3.5)"; 
          tree->Draw(trtDraw, trtPosEndcapCut);

         // now draw them -------------------------------------------------------------        
         TCanvas* trtPosEndcapCanv = new TCanvas("trtPosEndcapCanv", "TRT  Positive Endcap", 140, 140, iCanvWidth, iCanvHeight);
         trtPosEndcapCanv->Divide(2,1);   
         trtPosEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("trt_posendcap_resX");
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
          trtResHistograms.push_back(htmp);
          trtResHistogramNames.push_back(hname);
          trtResSigma.push_back(sigma);
          trtResSigmaError.push_back(sigmaError);
          trtResMean.push_back(mean);
          trtResMeanError.push_back(meanError);

         trtPosEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("trt_posendcap_pullX");
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
          trtPullDistHistograms.push_back(htmp);
          trtPullDistHistogramNames.push_back(hname);
          trtPullDistSigma.push_back(sigma);
          trtPullDistSigmaError.push_back(sigmaError);
          trtPullDistMean.push_back(mean);
          trtPullDistMeanError.push_back(meanError);

        // ENDCAP SECTION B: negative EC -----------------------------------
        helperCanvas->cd();
        TString trtNegEndcapCut = "DetectorType==3 && HitSurfaceZ<-800 && "+drawCuts;

         // -------------------------------------------------
                  trtDraw  = "residualLocX>>trt_negendcap_resX(";
                  trtDraw += bins;
                  trtDraw += ",-0.5,0.5)"; 
          tree->Draw(trtDraw, trtNegEndcapCut);
         // -------------------------------------------------
                  trtDraw  = "pullLocX>>trt_negendcap_pullX(";
                  trtDraw += bins;
                  trtDraw += ",-3.5,3.5)"; 
          tree->Draw(trtDraw, trtNegEndcapCut);
         // now draw them -------------------------------------------------------------        
         TCanvas* trtNegEndcapCanv = new TCanvas("trtNegEndcapCanv", "TRT  Negative Endcap", 160, 160, iCanvWidth, iCanvHeight);
         trtNegEndcapCanv->Divide(2,1);   
         trtNegEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("trt_negendcap_resX");
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
          trtResHistograms.push_back(htmp);
          trtResHistogramNames.push_back(hname);
          trtResSigma.push_back(sigma);
          trtResSigmaError.push_back(sigmaError);
          trtResMean.push_back(mean);
          trtResMeanError.push_back(meanError);

         trtNegEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("trt_negendcap_pullX");
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
          trtPullDistHistograms.push_back(htmp);
          trtPullDistHistogramNames.push_back(hname);
          trtPullDistSigma.push_back(sigma);
          trtPullDistSigmaError.push_back(sigmaError);
          trtPullDistMean.push_back(mean);
          trtPullDistMeanError.push_back(meanError);

       // Now per layer --------------------------------------------------------------------
       // --------> Barrel Layers

           std::cout << "[i] Starting dedicated TRT  session ... " << std::endl;

           double   radii[4]        = { 580., 700., 865., 1100.};

           // TRT  Barrel
           for (int iring = 0; iring<3; ++iring){
                 helperCanvas->cd();

                 // The Layer id
                 TString layerId  = "_trt_barrel_ring_";
                         layerId += (iring+1);

                 // The Layer name
                 TString layerName  = "Ring ";
                         layerName += (iring+1);

                 // the residual in x
                 TString cut  = "DetectorType==3 && HitSurfaceRadius>";
                         cut += radii[iring];
                         cut += " && HitSurfaceRadius<";
                         cut += radii[iring+1];
                         cut += " && abs(HitSurfaceZ) < 800 && ";
                         cut += drawCuts;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-0.5, 0.5)";

                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);

               // TRT  Barrel Canvas
               TCanvas* trtBarrelCanv = new TCanvas("trtBarrelCanv"+layerId, "TRT  Barrel - "+layerName, 180+iring*20, 180+iring*20, iCanvWidth, iCanvHeight);
               trtBarrelCanv->Divide(2,1);
               trtBarrelCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+layerId);
                 hname="Residual locX - TRT  Barrel ";
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
                 trtResHistograms.push_back(htmp);
                 trtResHistogramNames.push_back(hname);
                 trtResSigma.push_back(sigma);
                 trtResSigmaError.push_back(sigmaError);
                 trtResMean.push_back(mean);
                 trtResMeanError.push_back(meanError);

               trtBarrelCanv->cd(2);
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
                 trtPullDistHistograms.push_back(htmp);
                 trtPullDistHistogramNames.push_back(hname);
                 trtPullDistSigma.push_back(sigma);
                 trtPullDistSigmaError.push_back(sigmaError);
                 trtPullDistMean.push_back(mean);
                 trtPullDistMeanError.push_back(meanError);               
            }
            
           // TRT  positive endcap section

           double   zPos[5]       = { 800., 1350., 1900., 2450., 3000.};
           
           // TRT  Positive Endcap
           for (int ilay = 0; ilay<4; ++ilay){
                 helperCanvas->cd();
                 // layer ID
                 TString layerId  ="_trt_posec_layer";
                         layerId +=(ilay+1);
                 // layer name
                 TString layerName  ="z Sector ";
                         layerName +=(ilay+1);                

                 // the residual in x
                 TString cut  = "DetectorType==3 && HitSurfaceZ>";
                         cut += zPos[ilay];
                         cut += " && HitSurfaceZ<";
                         cut += zPos[ilay+1];
                         cut += " && ";
                         cut += drawCuts;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-0.5, 0.5)";
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);

                 tree->Draw(ide, cut);
               // TRT PosEndcap canvas
               TCanvas* trtPosECCanv = new TCanvas("trtPosECCanv"+layerId, "TRT  Positive EC - "+layerName, 240+ilay*20, 180+ilay*20, iCanvWidth, iCanvHeight);
               trtPosECCanv->Divide(2,1);
               trtPosECCanv->cd(1);
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
                 trtResHistograms.push_back(htmp);
                 trtResHistogramNames.push_back(hname);
                 trtResSigma.push_back(sigma);
                 trtResSigmaError.push_back(sigmaError);
                 trtResMean.push_back(mean);
                 trtResMeanError.push_back(meanError);

               trtPosECCanv->cd(2);
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
                 trtPullDistHistograms.push_back(htmp);
                 trtPullDistHistogramNames.push_back(hname);
                 trtPullDistSigma.push_back(sigma);
                 trtPullDistSigmaError.push_back(sigmaError);
                 trtPullDistMean.push_back(mean);
                 trtPullDistMeanError.push_back(meanError);
           }

           // TRT  negative endcap section

           for (int ilay = 0; ilay<4; ++ilay){
                 helperCanvas->cd();
                 // layer ID
                 TString layerId  ="_trt_negec_layer";
                         layerId +=(ilay+1);
                 // layer name
                 TString layerName  ="Z Sector ";
                         layerName +=(ilay+1);  
                 // the residual in x
                 TString cut  = "DetectorType==3 && HitSurfaceZ<-";
                         cut += zPos[ilay];
                         cut += " && HitSurfaceZ>-";
                         cut += zPos[ilay+1];
			 cut += " && ";
                         cut += drawCuts;
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-0.5, 0.5)";
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += layerId;
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);

               // TRT  NegEndcap canvas
               TCanvas* trtPosECCanv = new TCanvas("trtNegECCanv"+layerId, "TRT  Negative EC - "+layerName, 280+ilay*20, 180+ilay*20, iCanvWidth, iCanvHeight);
               trtPosECCanv->Divide(2,1);
               trtPosECCanv->cd(1);
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
                 trtResHistograms.push_back(htmp);
                 trtResHistogramNames.push_back(hname);
                 trtResSigma.push_back(sigma);
                 trtResSigmaError.push_back(sigmaError);
                 trtResMean.push_back(mean);
                 trtResMeanError.push_back(meanError);

             trtPosECCanv->cd(2);
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
                 trtPullDistHistograms.push_back(htmp);
                 trtPullDistHistogramNames.push_back(hname);
                 trtPullDistSigma.push_back(sigma);
                 trtPullDistSigmaError.push_back(sigmaError);
                 trtPullDistMean.push_back(mean);
                 trtPullDistMeanError.push_back(meanError);
                            
            }

        } 


       
       int etaSlices = 10;

       double etaSlice = etaCut/etaSlices;

       Float_t etaValues[etaSlices-1];
       Float_t etaErrors[etaSlices-1];

       // residuals
       Float_t trtResSigmaValues_locX[etaSlices-1];
       Float_t trtResSigmaErrors_locX[etaSlices-1];
       Float_t trtResMeanValues_locX[etaSlices-1];
       Float_t trtResMeanErrors_locX[etaSlices-1];

       // pulls
       Float_t trtPullsSigmaValues_locX[etaSlices-1];
       Float_t trtPullsSigmaErrors_locX[etaSlices-1];
       Float_t trtPullsMeanValues_locX[etaSlices-1];
       Float_t trtPullsMeanErrors_locX[etaSlices-1];



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
	       drawCuts_rp += " && ";
	       drawCuts_rp += qOverPLimit;
	       drawCuts_rp += driftTimeCut;

       helperCanvas->cd();     

       TString histName     = "trt_resX_etaSlice";
               histName    += i;

       TString trtDraw_r  = "residualLocX>>";
               trtDraw_r += histName;
               trtDraw_r += "(";
               trtDraw_r += bins;
               trtDraw_r += ",-0.5,0.5)"; 
       tree->Draw(trtDraw_r, "DetectorType==3 && "+drawCuts_rp);
       
       TH1F* htmp_r         = (TH1F*)gDirectory->Get(histName);
   
       htmp_r->Fit("gaus");
       TF1* gaus_r          = htmp_r->GetFunction("gaus");
       double sigma_r       = gaus_r->GetParameter(2);
       double sigmaError_r  = gaus_r->GetParError(2);
       double mean_r        = gaus_r->GetParameter(1);
       double meanError_r   = gaus_r->GetParError(1);
       // ----------------------------------------
       trtResSigmaValues_locX[i] = sigma_r;
       trtResSigmaErrors_locX[i] = sigmaError_r;
       trtResMeanValues_locX[i]  = mean_r;
       trtResMeanErrors_locX[i]  = meanError_r;
   
       std::cout << "[m] ResX done for slice : " << i << std::endl;

       // ---------------- Pulls -------------------------

       histName     = "trt_pullX_etaSlice";
       histName    += i;

       TString trtDraw_p  = "pullLocX>>";
               trtDraw_p += histName;
               trtDraw_p += "(";
               trtDraw_p += bins;
               trtDraw_p += ",-3.5,3.5)"; 
       tree->Draw(trtDraw_p, "DetectorType==3 && "+drawCuts_rp);
       
       TH1F* htmp_p         = (TH1F*)gDirectory->Get(histName);
       htmp_p->Fit("gaus");
       TF1* gaus_p          = htmp_p->GetFunction("gaus");
       double sigma_p       = gaus_p->GetParameter(2);
       double sigmaError_p  = gaus_p->GetParError(2);
       double mean_p        = gaus_p->GetParameter(1);
       double meanError_p   = gaus_p->GetParError(1);
       // ----------------------------------------
       trtPullsSigmaValues_locX[i] = sigma_p;
       trtPullsSigmaErrors_locX[i] = sigmaError_p;
       trtPullsMeanValues_locX[i]  = mean_p;
       trtPullsMeanErrors_locX[i]  = meanError_p;
      
       std::cout << "[m] PullX done for slice : " << i << std::endl;

      }
 
     // now draw the result !!
     TCanvas* residualSlices = new TCanvas("residualSlices", "Residuals vs. #eta", 400,100,iCanvWidth, iCanvHeight);
     residualSlices->Divide(2,1);
     residualSlices->cd(1);
      TGraphErrors* resXGraph = new TGraphErrors(etaSlices, etaValues, trtResSigmaValues_locX, etaErrors, trtResSigmaErrors_locX);
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
      TGraphErrors* meanXGraph = new TGraphErrors(etaSlices, etaValues, trtResMeanValues_locX, etaErrors, trtResMeanErrors_locX);
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
                                                      trtPullsSigmaValues_locX, 
                                                      etaErrors,
                                                      trtPullsSigmaErrors_locX);
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
                                                      trtPullsMeanValues_locX, 
                                                      etaErrors,
                                                      trtPullsMeanErrors_locX);
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
      std::cout << "   TRT_ResidualsAndPulls ID_rec FitterValidation OutputTrack 65 1000 250 (No)Drifttime" << std::endl;
      std::cout << std::endl;
    }

    theApp.Run();
    return(0);
}
