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
//   Int_t           nPixelHits;
//   Int_t           nSCTHits;
//   Int_t           nTRTHits;

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
   plain->SetTitleX(0.25);
   plain->SetTitleY(0.98);
   // Stat
   plain->SetStatColor(0);
   plain->SetOptStat(11111);
   plain->SetOptFit(111);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");

   gROOT->SetStyle("Plain");

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
//         tree->SetBranchAddress("nPixelHits",&nPixelHits);
//         tree->SetBranchAddress("nSCTHits",&nSCTHits);
//         tree->SetBranchAddress("nTRTHits",&nTRTHits);


        
         // the stored values
         std::vector<TH1F*>    pixelResHistograms;
         std::vector<TString>  pixelResHistogramNames;
         std::vector<double>   pixelResSigma;
         std::vector<double>   pixelResSigmaError;
         std::vector<double>   pixelResMean;
         std::vector<double>   pixelResMeanError;

         std::vector<TH1F*>    pixelPullDistHistograms;
         std::vector<TString>  pixelPullDistHistogramNames;
         std::vector<double>   pixelPullDistSigma;
         std::vector<double>   pixelPullDistSigmaError;
         std::vector<double>   pixelPullDistMean;
         std::vector<double>   pixelPullDistMeanError;
         
       
// <-----------  Pixel SECTION  ------------------------------------------------------------------------->
        // OVERALL SECTION ----------------------------------
        helperCanvas->cd();
         // -------------------------------------------------
          TString pixelDraw  = "residualLocX>>pix_all_resX(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.06,0.06)"; 
          tree->Draw(pixelDraw, "DetectorType==1 && "+drawCuts);
         // -------------------------------------------------
                  pixelDraw  = "pullLocX>>pix_all_pullX(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, "DetectorType==1 && "+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "residualLocY>>pix_all_resY(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.6,0.6)"; 
          tree->Draw(pixelDraw, "DetectorType==1 && "+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "pullLocY>>pix_all_pullY(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, "DetectorType==1 && "+drawCuts);    
         // now draw them -------------------------------------------------------------        
         TCanvas* pixCanv = new TCanvas("pixCanv", "Pixel Overall", 100, 100, 600, 620);
         pixCanv->Divide(2,2);    
         pixCanv->cd(1);
          TH1F* htmp    = (TH1F*)gDirectory->Get("pix_all_resX");
          TString hname = "Residual locX - Pixel Overall";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          TF1* gaus    = htmp->GetFunction("gaus");
          double sigma       = gaus->GetParameter(2);
          double sigmaError  = gaus->GetParError(2);
          double mean        = gaus->GetParameter(1);
          double meanError   = gaus->GetParError(1);
          // ------------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("pix_all_pullX");
          hname="Pull dist locX - Pixel Overall";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

         pixCanv->cd(3);
          htmp = (TH1F*)gDirectory->Get("pix_all_resY");
          hname="Residual locY - Pixel Overall";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixCanv->cd(4);
          htmp = (TH1F*)gDirectory->Get("pix_all_pullY");
          hname="Pull dist locY - Pixel Overall";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);
          
        // BARREL SECTION -----------------------------------
        helperCanvas->cd();

        TString pixBarrelCut = "DetectorType==1 && abs(HitSurfaceZ)<400 && ";

         // -------------------------------------------------
                  pixelDraw  = "residualLocX>>pix_barrel_resX(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.06,0.06)"; 
          tree->Draw(pixelDraw, pixBarrelCut+drawCuts);
         // -------------------------------------------------
                  pixelDraw  = "pullLocX>>pix_barrel_pullX(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixBarrelCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "residualLocY>>pix_barrel_resY(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.6,0.6)"; 
          tree->Draw(pixelDraw, pixBarrelCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "pullLocY>>pix_barrel_pullY(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixBarrelCut+drawCuts); 
         // now draw them -------------------------------------------------------------        
         TCanvas* pixBarrelCanv = new TCanvas("pixBarrelCanv", "Pixel Barrel", 120, 120, 600, 620);
         pixBarrelCanv->Divide(2,2);    
         pixBarrelCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("pix_barrel_resX");
          hname="Residual locX - Pixel Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixBarrelCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("pix_barrel_pullX");
          hname="Pull dist locX - Pixel Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

         pixBarrelCanv->cd(3);
          htmp = (TH1F*)gDirectory->Get("pix_barrel_resY");
          hname="Residual locY - Pixel Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixBarrelCanv->cd(4);
          htmp = (TH1F*)gDirectory->Get("pix_barrel_pullY");
          hname="Pull dist locY - Pixel Barrel";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

        // ENDCAP SECTION A: positive EC -----------------------------------
        helperCanvas->cd();

        TString pixelPosEcCut = "DetectorType==1 && HitSurfaceZ>400 && ";

         // -------------------------------------------------
                  pixelDraw  = "residualLocX>>pix_posendcap_resX(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.06,0.06)"; 
          tree->Draw(pixelDraw, pixelPosEcCut+drawCuts);
         // -------------------------------------------------
                  pixelDraw  = "pullLocX>>pix_posendcap_pullX(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixelPosEcCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "residualLocY>>pix_posendcap_resY(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.6,0.6)"; 
          tree->Draw(pixelDraw, pixelPosEcCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "pullLocY>>pix_posendcap_pullY(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixelPosEcCut+drawCuts); 
         // now draw them -------------------------------------------------------------        
         TCanvas* pixPosEndcapCanv = new TCanvas("pixPosEndcapCanv", "Pixel Positive Endcap", 140, 140, 600, 620);
         pixPosEndcapCanv->Divide(2,2);    
         pixPosEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("pix_posendcap_resX");
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
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixPosEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("pix_posendcap_pullX");
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
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

         pixPosEndcapCanv->cd(3);
          htmp = (TH1F*)gDirectory->Get("pix_posendcap_resY");
          hname="Residual locY - Positive Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixPosEndcapCanv->cd(4);
          htmp = (TH1F*)gDirectory->Get("pix_posendcap_pullY");
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
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

        // ENDCAP SECTION B: negative EC -----------------------------------
        helperCanvas->cd();

        TString pixelNegEcCut = "DetectorType==1 && HitSurfaceZ<-400 && ";

         // -------------------------------------------------
                  pixelDraw  = "residualLocX>>pix_negendcap_resX(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.06,0.06)"; 
          tree->Draw(pixelDraw, pixelNegEcCut+drawCuts);
         // -------------------------------------------------
                  pixelDraw  = "pullLocX>>pix_negendcap_pullX(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixelNegEcCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "residualLocY>>pix_negendcap_resY(";
                  pixelDraw += bins;
                  pixelDraw += ",-0.6,0.6)"; 
          tree->Draw(pixelDraw, pixelNegEcCut+drawCuts);
         // ------------------------------------------------- 
                  pixelDraw  = "pullLocY>>pix_negendcap_pullY(";
                  pixelDraw += bins;
                  pixelDraw += ",-3.5,3.5)"; 
          tree->Draw(pixelDraw, pixelNegEcCut+drawCuts); 
         // now draw them -------------------------------------------------------------        
         TCanvas* pixNegEndcapCanv = new TCanvas("pixNegEndcapCanv", "Pixel Negative Endcap", 160, 160, 600, 620);
         pixNegEndcapCanv->Divide(2,2);    
         pixNegEndcapCanv->cd(1);
          htmp = (TH1F*)gDirectory->Get("pix_negendcap_resX");
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
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);

         pixNegEndcapCanv->cd(2);
          htmp = (TH1F*)gDirectory->Get("pix_negendcap_pullX");
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
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

         pixNegEndcapCanv->cd(3);
          htmp = (TH1F*)gDirectory->Get("pix_negendcap_resY");
          hname="Residual locY - Negative Endcap";
          htmp->SetTitle("Residual locY - Negative Endcap");
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ----------------------------------------
          pixelResHistograms.push_back(htmp);
          pixelResHistogramNames.push_back(hname);
          pixelResSigma.push_back(sigma);
          pixelResSigmaError.push_back(sigmaError);
          pixelResMean.push_back(mean);
          pixelResMeanError.push_back(meanError);


         pixNegEndcapCanv->cd(4);
          htmp = (TH1F*)gDirectory->Get("pix_negendcap_pullY");
          hname="Pull dist locY - Negative Endcap";
          htmp->SetTitle(hname);
          htmp->Draw();          
          htmp->Fit("gaus");
          gaus    = htmp->GetFunction("gaus");
          sigma       = gaus->GetParameter(2);
          sigmaError  = gaus->GetParError(2);
          mean        = gaus->GetParameter(1);
          meanError   = gaus->GetParError(1);
          // ---------------------------------------------
          pixelPullDistHistograms.push_back(htmp);
          pixelPullDistHistogramNames.push_back(hname);
          pixelPullDistSigma.push_back(sigma);
          pixelPullDistSigmaError.push_back(sigmaError);
          pixelPullDistMean.push_back(mean);
          pixelPullDistMeanError.push_back(meanError);

       // Now per layer --------------------------------------------------------------------
       // --------> Barrel Layers

           std::cout << "[i] Starting dedicated Pixel session ... " << std::endl;

           double   radii[4]       = { 0., 56., 118., 200.};
           TString  id[3]           = { "_B_Layer", "_1st_Layer", "_2nd_Layer" };
           TString  name[3]         = { "B Layer", "1st Layer", "2nd Layer" };

           // Pixel Barrel
           for (int ilay = 0; ilay<3; ++ilay){
                 helperCanvas->cd();
                 // the residual in x
                 TString cut  = "DetectorType==1 && HitSurfaceRadius>";
                         cut += radii[ilay];
                         cut += " && HitSurfaceRadius<";
                         cut += radii[ilay+1];
                         cut += " && abs(HitSurfaceZ) < 445"; 
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += id[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.06, 0.06)";

                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += id[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
                // the residual in y
                         ide  = "residualLocY>>resPixLocY";
                         ide += id[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.6, 0.6)";
                 tree->Draw(ide, cut);               
                // the pull in y
                         ide  = "pullLocY>>pullPixLocY";
                         ide += id[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
               // Pixel Barrel Canvae
               TCanvas* pixBarrelCanv = new TCanvas("pixBarrelCanv"+id[ilay], "Pixel Barrel - "+name[ilay], 180+ilay*20, 180+ilay*20, 600, 620);
               pixBarrelCanv->Divide(2,2);
               pixBarrelCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+id[ilay]);
                 hname="Residual locX - Barrel ";
                 hname+=name[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

               pixBarrelCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+id[ilay]);
                 hname="Pull dist locX - Barrel ";
                 hname+=name[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
               pixBarrelCanv->cd(3);
                htmp = (TH1F*)gDirectory->Get("resPixLocY"+id[ilay]);
                 hname="Residual locY - Barrel ";
                 hname+=name[ilay];
                 htmp->SetTitle(hname);
                 htmp->SetTitle(name[ilay]);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

               pixBarrelCanv->cd(4);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocY"+id[ilay]);
                 hname="Pull dist locY - Barrel ";
                 hname+=name[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
            }
            
           // Pixel positive endcap section

           double   minZpos[2]       = { 420., 600.};
           double   maxZpos[2]       = { 600., 800.};
           TString  idec[2]       = { "_ecpos_1st_Layer", "_ecpos_2nd_Layer" };
           TString  nameec[2]     = { "1st Layer", "2nd Layer" };

           // Pixel Barrel
           for (int ilay = 0; ilay<2; ++ilay){
                 helperCanvas->cd();
                 // the residual in x
                 TString cut  = "DetectorType==1 && HitSurfaceZ>";
                         cut += minZpos[ilay];
                         cut += " && HitSurfaceZ<";
                         cut += maxZpos[ilay];
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += idec[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.06, 0.06)";
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += idec[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
                // the residual in y
                         ide  = "residualLocY>>resPixLocY";
                         ide += idec[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.6, 0.6)";
                 tree->Draw(ide, cut);               
                // the pull in y
                         ide  = "pullLocY>>pullPixLocY";
                         ide += idec[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
               // Pixel Barrel Canvae
               TCanvas* pixPosECCanv = new TCanvas("pixPosECCanv"+idec[ilay], "Pixel Positive EC - "+nameec[ilay], 240+ilay*20, 180+ilay*20, 600, 620);
               pixPosECCanv->Divide(2,2);
               pixPosECCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+idec[ilay]);
                 hname="Residual locX - Positive EC ";
                 hname+=nameec[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

               pixPosECCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+idec[ilay]);
                 hname="Pull dist locX  - Positive EC ";
                 hname+=nameec[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
               pixPosECCanv->cd(3);
                htmp = (TH1F*)gDirectory->Get("resPixLocY"+idec[ilay]);
                 hname="Residual locY - Positive EC ";
                 hname+=nameec[ilay];
                 htmp->SetTitle(hname);
                 htmp->SetTitle(nameec[ilay]);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

               pixPosECCanv->cd(4);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocY"+idec[ilay]);
                 hname="Pull dist locY - Positive EC ";
                 hname+=nameec[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
            }

           // Pixel positive endcap section

           double   minZneg[2]       = { -420., -600.};
           double   maxZneg[2]       = { -600., -800.};
           TString  idecn[2]         = { "_ecneg_1st_Layer", "_ecneg_2nd_Layer" };
           TString  nameecn[2]       = { "1st Layer", "2nd Layer" };

           // Pixel Barrel
           for (int ilay = 0; ilay<2; ++ilay){
                 helperCanvas->cd();
                 // the residual in x
                 TString cut  = "DetectorType==1 && HitSurfaceZ<";
                         cut += minZneg[ilay];
                         cut += " && HitSurfaceZ>";
                         cut += maxZneg[ilay];
                 TString ide  = "residualLocX>>resPixLocX";
                         ide += idecn[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.06, 0.06)";
                 tree->Draw(ide, cut);
                 // the pull in x
                         ide  = "pullLocX>>pullPixLocX";
                         ide += idecn[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
                // the residual in y
                         ide  = "residualLocY>>resPixLocY";
                         ide += idecn[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-0.6, 0.6)";
                 tree->Draw(ide, cut);               
                // the pull in y
                         ide  = "pullLocY>>pullPixLocY";
                         ide += idecn[ilay];
                         ide += "(";
                         ide += bins;
                         ide += ",-3.5, 3.5)";
                 tree->Draw(ide, cut);
               // Pixel Barrel Canvae
               TCanvas* pixPosECCanv = new TCanvas("pixNegECCanv"+idecn[ilay], "Pixel Negative EC - "+nameecn[ilay], 280+ilay*20, 180+ilay*20, 600, 620);
               pixPosECCanv->Divide(2,2);
               pixPosECCanv->cd(1);
                 htmp = (TH1F*)gDirectory->Get("resPixLocX"+idecn[ilay]);
                 hname="Residual locX - Positive EC ";
                 hname+=nameec[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

             pixPosECCanv->cd(2);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocX"+idecn[ilay]);
                 hname="Pull dist locX  - Positive EC ";
                 hname+=nameecn[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
             pixPosECCanv->cd(3);
                htmp = (TH1F*)gDirectory->Get("resPixLocY"+idecn[ilay]);
                 hname="Residual locY - Positive EC ";
                 hname+=nameecn[ilay];
                 htmp->SetTitle(hname);
                 htmp->SetTitle(nameecn[ilay]);
                 htmp->Draw();
                 htmp->Fit("gaus");
                 gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ----------------------------------------
                 pixelResHistograms.push_back(htmp);
                 pixelResHistogramNames.push_back(hname);
                 pixelResSigma.push_back(sigma);
                 pixelResSigmaError.push_back(sigmaError);
                 pixelResMean.push_back(mean);
                 pixelResMeanError.push_back(meanError);

              pixPosECCanv->cd(4);
                 htmp = (TH1F*)gDirectory->Get("pullPixLocY"+idecn[ilay]);
                 hname="Pull dist locY - Positive EC ";
                 hname+=nameecn[ilay];
                 htmp->SetTitle(hname);
                 htmp->Draw();
                 htmp->Fit("gaus");
               gaus    = htmp->GetFunction("gaus");
                 sigma       = gaus->GetParameter(2);
                 sigmaError  = gaus->GetParError(2);
                 mean        = gaus->GetParameter(1);
                 meanError   = gaus->GetParError(1);
                 // ---------------------------------------------
                 pixelPullDistHistograms.push_back(htmp);
                 pixelPullDistHistogramNames.push_back(hname);
                 pixelPullDistSigma.push_back(sigma);
                 pixelPullDistSigmaError.push_back(sigmaError);
                 pixelPullDistMean.push_back(mean);
                 pixelPullDistMeanError.push_back(meanError);
               
            }

        } 


       
       int etaSlices = 10;

       double etaSlice = etaCut/etaSlices;

       Float_t etaValues[etaSlices-1];
       Float_t etaErrors[etaSlices-1];

       // residuals
       Float_t pixelResSigmaValues_locX[etaSlices-1];
       Float_t pixelResSigmaErrors_locX[etaSlices-1];
       Float_t pixelResMeanValues_locX[etaSlices-1];
       Float_t pixelResMeanErrors_locX[etaSlices-1];

       Float_t pixelResSigmaValues_locY[etaSlices-1];
       Float_t pixelResSigmaErrors_locY[etaSlices-1];
       Float_t pixelResMeanValues_locY[etaSlices-1];
       Float_t pixelResMeanErrors_locY[etaSlices-1];

       // pulls
       Float_t pixelPullsSigmaValues_locX[etaSlices-1];
       Float_t pixelPullsSigmaErrors_locX[etaSlices-1];
       Float_t pixelPullsRMSValues_locX[etaSlices-1];
       Float_t pixelPullsRMSErrors_locX[etaSlices-1];
       Float_t pixelPullsMeanValues_locX[etaSlices-1];
       Float_t pixelPullsMeanErrors_locX[etaSlices-1];

       Float_t pixelPullsSigmaValues_locY[etaSlices-1];
       Float_t pixelPullsSigmaErrors_locY[etaSlices-1];
       Float_t pixelPullsRMSValues_locY[etaSlices-1];
       Float_t pixelPullsRMSErrors_locY[etaSlices-1];
       Float_t pixelPullsMeanValues_locY[etaSlices-1];
       Float_t pixelPullsMeanErrors_locY[etaSlices-1];


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

       helperCanvas->cd();     

       TString histName     = "pix_resX_etaSlice";
               histName    += i;

       TString pixelDraw_r  = "residualLocX>>";
               pixelDraw_r += histName;
               pixelDraw_r += "(";
               pixelDraw_r += bins;
               pixelDraw_r += ",-0.06,0.06)"; 
       tree->Draw(pixelDraw_r, "DetectorType==1 && "+drawCuts_rp);
       
       TH1F* htmp_r         = (TH1F*)gDirectory->Get(histName);
   
       htmp_r->Fit("gaus");
       TF1* gaus_r          = htmp_r->GetFunction("gaus");
       double sigma_r       = gaus_r->GetParameter(2);
       double sigmaError_r  = gaus_r->GetParError(2);
       double mean_r        = gaus_r->GetParameter(1);
       double meanError_r   = gaus_r->GetParError(1);
       // ----------------------------------------
       pixelResSigmaValues_locX[i] = sigma_r;
       pixelResSigmaErrors_locX[i] = sigmaError_r;
       pixelResMeanValues_locX[i]  = mean_r;
       pixelResMeanErrors_locX[i]  = meanError_r;
   
       std::cout << "[m] ResX done for slice : " << i << std::endl;

       histName     = "pix_resY_etaSlice";
       histName    += i;
   
       pixelDraw_r  = "residualLocY>>";
       pixelDraw_r += histName;
       pixelDraw_r += "(";
       pixelDraw_r += bins;
       pixelDraw_r += ",-0.06,0.06)"; 
       tree->Draw(pixelDraw_r, "DetectorType==1 && "+drawCuts_rp);
       
       htmp_r         = (TH1F*)gDirectory->Get(histName);
       htmp_r->Fit("gaus");
       gaus_r               = htmp_r->GetFunction("gaus");
       sigma_r              = gaus_r->GetParameter(2);
       sigmaError_r         = gaus_r->GetParError(2);
       mean_r               = gaus_r->GetParameter(1);
       meanError_r          = gaus_r->GetParError(1);
       // ----------------------------------------
       pixelResSigmaValues_locY[i] = sigma_r;
       pixelResSigmaErrors_locY[i] = sigmaError_r;
       pixelResMeanValues_locY[i]  = mean_r;
       pixelResMeanErrors_locY[i]  = meanError_r;

       std::cout << "[m] ResY done for slice : " << i << std::endl;


       // ---------------- Pulls -------------------------

       histName     = "pix_pullX_etaSlice";
       histName    += i;

       TString pixelDraw_p  = "pullLocX>>";
               pixelDraw_p += histName;
               pixelDraw_p += "(";
               pixelDraw_p += bins;
               pixelDraw_p += ",-3.5,3.5)"; 
       tree->Draw(pixelDraw_p, "DetectorType==1 && "+drawCuts_rp);
       
       TH1F* htmp_p         = (TH1F*)gDirectory->Get(histName);
       htmp_p->Fit("gaus");
       TF1* gaus_p          = htmp_p->GetFunction("gaus");
       double rms_p         = htmp_p->GetRMS();
       double rmsError_p    = htmp_p->GetRMSError();

       double sigma_p       = gaus_p->GetParameter(2);
       double sigmaError_p  = gaus_p->GetParError(2);
       double mean_p        = gaus_p->GetParameter(1);
       double meanError_p   = gaus_p->GetParError(1);
       // ----------------------------------------
       pixelPullsSigmaValues_locX[i] = sigma_p;
       pixelPullsSigmaErrors_locX[i] = sigmaError_p;
       pixelPullsRMSValues_locX[i]   = rms_p;
       pixelPullsRMSErrors_locX[i]   = rmsError_p;
       pixelPullsMeanValues_locX[i]  = mean_p;
       pixelPullsMeanErrors_locX[i]  = meanError_p;
      
       std::cout << "[m] PullX done for slice : " << i << std::endl;

       histName     = "pix_pullY_etaSlice";
       histName    += i;

       pixelDraw_p  = "pullLocY>>";
       pixelDraw_p += histName;
       pixelDraw_p += "(";
       pixelDraw_p += bins;
       pixelDraw_p += ",-3.5,3.5)"; 
       tree->Draw(pixelDraw_p, "DetectorType==1 && "+drawCuts_rp);
       
       htmp_p               = (TH1F*)gDirectory->Get(histName);
       htmp_p->Fit("gaus");
       gaus_p               = htmp_p->GetFunction("gaus");
       rms_p                = htmp_p->GetRMS();
       rmsError_p           = htmp_p->GetRMSError();
       sigma_p              = gaus_p->GetParameter(2);
       sigmaError_p         = gaus_p->GetParError(2);
       mean_p               = gaus_p->GetParameter(1);
       meanError_p          = gaus_p->GetParError(1);
       // ----------------------------------------
       pixelPullsSigmaValues_locY[i] = sigma_p;
       pixelPullsSigmaErrors_locY[i] = sigmaError_p;
       pixelPullsRMSValues_locY[i]   = rms_p;
       pixelPullsRMSErrors_locY[i]   = rmsError_p;
       pixelPullsMeanValues_locY[i]  = mean_p;
       pixelPullsMeanErrors_locY[i]  = meanError_p;

       std::cout << "[m] PullY done for slice : " << i << std::endl;

      }
 
     // now draw the result !!
     TCanvas* residualSlices = new TCanvas("PixelResidualSlices", "Residuals vs. #eta", 400,100,600,620);
     residualSlices->Divide(2,2);
     residualSlices->cd(1);
      TGraphErrors* resXGraph = new TGraphErrors(etaSlices, etaValues, pixelResSigmaValues_locX, etaErrors, pixelResSigmaErrors_locX);
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
      TGraphErrors* meanXGraph = new TGraphErrors(etaSlices, etaValues, pixelResMeanValues_locX, etaErrors, pixelResMeanErrors_locX);
      meanXGraph->SetTitle("#mu_{locX}");
      meanXGraph->SetMarkerStyle(1);
      meanXGraph->SetMarkerSize(0.8);
      meanXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* meanXGraph_Xaxis = meanXGraph->GetXaxis();
      meanXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* meanXGraph_Yaxis = meanXGraph->GetYaxis();
      meanXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     meanXGraph->Draw("ap");


     residualSlices->cd(3);
      TGraphErrors* resYGraph = new TGraphErrors(etaSlices, etaValues, pixelResSigmaValues_locY, etaErrors, pixelResSigmaErrors_locY);
      resYGraph->SetTitle("#sigma_{locY}");
      resYGraph->SetMarkerStyle(1);
      resYGraph->SetMarkerSize(0.8);
      resYGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* resYGraph_Xaxis = resYGraph->GetXaxis();
      resYGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* resYGraph_Yaxis = resYGraph->GetYaxis();
      resYGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     resYGraph->Draw("ap");


     residualSlices->cd(4);
      TGraphErrors* meanYGraph = new TGraphErrors(etaSlices, etaValues, pixelResMeanValues_locY, etaErrors, pixelResMeanErrors_locY);
      meanYGraph->SetTitle("#mu_{locY}");
      meanYGraph->SetMarkerStyle(1);
      meanYGraph->SetMarkerSize(0.8);
      
      meanYGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* meanYGraph_Xaxis = meanYGraph->GetXaxis();
      meanYGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* meanYGraph_Yaxis = meanYGraph->GetYaxis();
      meanYGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     meanYGraph->Draw("ap");


     //
     TCanvas* pullSlices = new TCanvas("PixelPullSlices", "Pulls vs. #eta", 400,100,600,620);
     pullSlices->Divide(2,2);
     pullSlices->cd(1);
      TGraphErrors* pullXGraph  = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsSigmaValues_locX, 
                                                      etaErrors,
                                                      pixelPullsSigmaErrors_locX);

      TGraphErrors* pullRMSXGraph  = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsRMSValues_locX, 
                                                      etaErrors,
                                                      pixelPullsRMSErrors_locX);

      pullXGraph->SetTitle("#sigma_{pulls,locX}");
      pullXGraph->SetLineColor(1);
      pullXGraph->SetMarkerColor(kBlack);
      pullXGraph->SetMarkerStyle(8);
      pullXGraph->SetMarkerSize(1.0);
      pullXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      pullRMSXGraph->SetLineColor(38);
      pullRMSXGraph->SetMarkerColor(kBlue);
      pullRMSXGraph->SetMarkerStyle(21);
      pullRMSXGraph->SetMarkerSize(1.0);


      TAxis* pullXGraph_Xaxis = pullXGraph->GetXaxis();
      pullXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullXGraph_Yaxis = pullXGraph->GetYaxis();
      pullXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

   pullXGraph->Draw("ap");
   pullRMSXGraph->Draw("p, same");
    

     pullSlices->cd(2);
      TGraphErrors* pullMeanXGraph = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsMeanValues_locX, 
                                                      etaErrors,
                                                      pixelPullsMeanErrors_locX);
      pullMeanXGraph->SetTitle("#mu_{pulls, locX}");
      pullMeanXGraph->SetMarkerStyle(1);
      pullMeanXGraph->SetMarkerSize(1);
      pullMeanXGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      TAxis* pullMeanXGraph_Xaxis = pullMeanXGraph->GetXaxis();
      pullMeanXGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullMeanXGraph_Yaxis = pullMeanXGraph->GetYaxis();
      pullMeanXGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     pullMeanXGraph->Draw("ap");

     pullSlices->cd(3);
      TGraphErrors* pullYGraph  = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsSigmaValues_locY, 
                                                      etaErrors,
                                                      pixelPullsSigmaErrors_locY);

      TGraphErrors* pullYRMSGraph  = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsRMSValues_locY, 
                                                      etaErrors,
                                                      pixelPullsRMSErrors_locY);

      pullYGraph->SetTitle("#sigma_{pulls, locY}");
      pullYGraph->SetLineColor(1);
      pullYGraph->SetMarkerColor(kBlack);
      pullYGraph->SetMarkerStyle(8);
      pullYGraph->SetMarkerSize(1.0);
      pullYGraph->ComputeRange(xmin,ymin,xmax,ymax);
      
      pullYRMSGraph->SetLineColor(38);
      pullYRMSGraph->SetMarkerColor(kBlue);
      pullYRMSGraph->SetMarkerStyle(21);
      pullYRMSGraph->SetMarkerSize(1.0);
   
      TAxis* pullYGraph_Xaxis = pullYGraph->GetXaxis();
      pullYGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullYGraph_Yaxis = pullYGraph->GetYaxis();
      pullYGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

    pullYGraph->Draw("ap");
    pullYRMSGraph->Draw("p, same");


     pullSlices->cd(4);
      TGraphErrors* pullMeanYGraph = new TGraphErrors(etaSlices,
                                                      etaValues, 
                                                      pixelPullsMeanValues_locY, 
                                                      etaErrors,
                                                      pixelPullsMeanErrors_locY);
      pullMeanYGraph->SetTitle("#mu_{pulls, locY}");
      pullMeanYGraph->SetMarkerStyle(1);
      pullMeanYGraph->SetMarkerSize(0.8);
      pullMeanYGraph->ComputeRange(xmin,ymin,xmax,ymax);

      TAxis* pullMeanYGraph_Xaxis = pullMeanYGraph->GetXaxis();
      pullMeanYGraph_Xaxis->Set(etaSlices-1, 0., xmax);

      TAxis* pullMeanYGraph_Yaxis = pullMeanYGraph->GetYaxis();
      pullMeanYGraph_Yaxis->Set(etaSlices-1, ymin, ymax);

     pullMeanYGraph->Draw("ap");

       // cleanup
       delete helperCanvas;
    }

    } else {
      std::cout << "[e] Not enough arguments given! " << std::endl;
      std::cout << "[i] Arguments required: filename (-.root), Directory, Tree, bins, pcut [MeV] , etacut [eta/100]" << std::endl;
      std::cout << "[i] sample usage for a File 'ID_rec.root' from standard InDetRecExample : " << std::endl;
      std::cout << std::endl;
      std::cout << "   PixelResidualsAndPulls ID_rec FitterValidation OutputTrack 65 1000 250" << std::endl;
      std::cout << std::endl;
    }

    theApp.Run();
    return(0);
}
