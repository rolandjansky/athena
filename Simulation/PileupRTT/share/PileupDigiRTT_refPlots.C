#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TBranch.h"
#include "TLegend.h"
#include "TChain.h"
#include "TCanvas.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

void MakeRefHistos(TChain *chain, TCanvas *canvas, char *psFile)
{
    // Set style
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    gStyle->SetMarkerStyle(6);

    // Fix vector<something> issues - seems to break script with recent root versions..
    //gSystem->Load("libCintex");
    //ROOT::Cintex::Cintex::Enable();
    
    // List of plots to draw
    char  histoName[100];
    char  canvName[100];
    char  plotType[100];
    char* branchNames[8] = { 
                             "NHardPrimVertices",
                             "NPileupInTimeEvents",
                             "hardPrimVtxX",
                             "hardPrimVtxY",
                             "hardPrimVtxZ",
                             "pileupVtxX",
                             "pileupVtxY",
                             "pileupVtxZ"
                            };
   
    // Draw plots and save them
    sprintf(plotType,"png");
    for (unsigned int i = 0; i < 8; i++) {
      sprintf(canvName,"%s.%s",branchNames[i],plotType);
      chain->Draw(branchNames[i]);
      canvas->SaveAs(canvName);
      canvas->Print(psFile);
    }

    chain->Draw("NPileupInTimeEvents:IEvent","","profile");
    canvas->SaveAs("AvgNPileupInTimeEvents.png");
    canvas->Print(psFile);

    chain->Draw("NPileupEvents:IEvent","","profile");
    canvas->SaveAs("AvgNPileupEvents.png");
    canvas->Print(psFile);
    
    chain->Draw("pileupVtxZ","(fabs(pileupVtxZ)<200.) && (fabs(pileupVtxY)<1.) && (fabs(pileupVtxX)<1.)");
    canvas->SaveAs("pileupPrimVtxZ.png");
    canvas->Print(psFile);

    chain->Draw("hardPrimVtxY:hardPrimVtxX:hardPrimVtxZ");
    canvas->SaveAs("hardPrimVtxXYZ.png");
    canvas->Print(psFile);

    TH1F *h_pileupTime = new TH1F("h_pileupTime","h_pileupTime",70,-900,825);
    chain->Draw("pileupEventTime>>h_pileupTime");
    canvas->SaveAs("pileupEventTime.png");
    canvas->Print(psFile);

    TH1I *h_eventBCID = new TH1I("h_eventBCID","h_eventBCID",500,0,500);
    chain->Draw("BCID>>h_eventBCID");
    canvas->SaveAs("eventBCID.png");
    canvas->Print(psFile);
}

void MakeMCTestHistos(TChain *chain, TCanvas *canvas, char *psFile)
{
    // Set style
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    gStyle->SetMarkerStyle(6);
    
    TH1I *h_signalEventTest = new TH1I("h_signalEventTest","h_signalEventTest",2,-0.5,1.5);
    chain->Draw("SignalEventTest>>h_signalEventTest");
    if(h_signalEventTest->GetMean() == 1){
      h_signalEventTest->SetFillColor(kGreen);
    } else {
      h_signalEventTest->SetFillColor(kRed);
    }
    chain->Draw("SignalEventTest>>h_signalEventTest");
    canvas->SaveAs("signalEventTest.png");
    canvas->Print(psFile);

    chain->Draw("GenEventNo>>h_genEventNumber");
    canvas->SaveAs("genEventNumber.png");
    canvas->Print(psFile);

    TH1I *h_subEventTest = new TH1I("h_subEventTest","h_subEventTest",2,-0.5,1.5);
    chain->Draw("SubEventTest>>h_subEventTest");
    if(h_subEventTest->GetMean() == 1){
      h_subEventTest->SetFillColor(kGreen);
    } else {
      h_subEventTest->SetFillColor(kRed);
    }
    chain->Draw("SubEventTest>>h_subEventTest");
    canvas->SaveAs("subEventTest.png");
    canvas->Print(psFile);

    TH1I *h_pileupEventBCID = new TH1I("h_pileupEventBCID","h_pileupEventBCID",500,0,500);
    chain->Draw("BCID>>h_pileupEventBCID");
    canvas->SaveAs("pileupEventBCID.png");
    canvas->Print(psFile);
    
}


Double_t getSCTOccupancy(TH2D *hist, TString loc = "", Long64_t nEvents)
{

    Int_t nModules;
         if ( loc.Contains("l0") )  nModules = 32*12;//nphi*neta
    else if ( loc.Contains("l1") )  nModules = 40*12;
    else if ( loc.Contains("l2") )  nModules = 48*12;
    else if ( loc.Contains("l3") )  nModules = 56*12;
    else { cout<<"Wrong SCT layer tag!"<<endl; return 0.0; }
    Double_t STRIP_MAX = 1536.;

    Double_t occ = 0.0;
    hist->Scale( 100. / (Double_t)(nEvents*nModules*STRIP_MAX) );
    for (Int_t x = 1; x <= hist->GetNbinsX(); x++) {
      for (Int_t y = 1; y <= hist->GetNbinsY(); y++) {
        if (hist->GetBinContent(x,y) > 0.) occ += hist->GetBinContent(x,y);
      }
    }
    return occ;
}

Double_t getOccupancy(TH2D *hist, TString loc = "", Long64_t nEvents)
{
    Int_t nModules;
         if ( loc.Contains("ecA") ) nModules = 144;
    else if ( loc.Contains("ecC") ) nModules = 144;
    else if ( loc.Contains("l0") )  nModules = 286;
    else if ( loc.Contains("l1") )  nModules = 494;
    else if ( loc.Contains("l2") )  nModules = 676;
    else { cout<<"Wrong Pixel layer tag!"<<endl; return 0.0; }
    Double_t pixelsPerModule = 46080.;

    Double_t occ = 0.0;
    hist->Scale( 100. / (Double_t)(nEvents*nModules*pixelsPerModule) );
    for (Int_t x = 1; x <= hist->GetNbinsX(); x++) {
      for (Int_t y = 1; y <= hist->GetNbinsY(); y++) {
        if (hist->GetBinContent(x,y) > 0.) occ += hist->GetBinContent(x,y);
      }
    }
    return occ;
}


void PileupDigiRTT_refPlots(TString inFile = "PileupDigiRTT.ntuple.root" )
{
    // Set style
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    gStyle->SetMarkerStyle(6);

    // Fix vector<something> issues - seems to break script with recent root versions..
    //gSystem->Load("libCintex");
    //ROOT::Cintex::Cintex::Enable();

    // Load the ntuple in a chain
    TChain *chain = new TChain("CollectionTree");
    chain->Add(inFile);
    TChain *chain2 = new TChain("EventInfoTree");
    chain2->Add(inFile);
    TChain *chain3 = new TChain("ValidationTree");
    chain3->Add(inFile);
    TChain *chain4 = new TChain("PixelOccupancyTree");
    chain4->Add(inFile);
    TChain *chain5 = new TChain("MCEventInfoTree");
    chain5->Add(inFile);

    chain->AddFriend("EventInfoTree");
    chain->AddFriend("ValidationTree");
    chain->AddFriend("PixelOccupancyTree");
    chain->AddFriend("MCEventInfoTree");

    Long64_t nEvents = chain->GetEntries();
    cout << "Number of events: " << nEvents << endl;

    // The canvas for our fitted spectrum
    TCanvas *canv = new TCanvas("canv","canv",8,49,501,375);
  
    char psoutfile[200];
    char psbegin[200];
    char psend[200];
 
    //  Setup the output PostScript file
    sprintf(psoutfile,"PileupDigiRTT.plots.ps");
    sprintf(psbegin,"%s[",psoutfile);
    sprintf(psend,"%s]",psoutfile);
   
    // Open the output .ps file
    // Make histograms from the TTree
    canv->Print(psbegin);
    MakeRefHistos(chain,canv,psoutfile);
        
    /// Draw histograms
    TFile *file = TFile::Open(inFile);
    TH2D *h2D;
    TH1F *h;
    
    /// List of plots & filetype to save them as (compare to HTML file!)
    char  histoName[100];
    char  canvName[100];
    char  plotType[100];
    sprintf(plotType,"png");
    char* pileupPlots[11] = {"eventPercent",
                             "NMinBiasRepeat",
                             "TimeMinBiasRepeat",
                             "NCavernBckgRepeat",
                             "TimeCavernBckgRepeat",
                             "NBeamGasHaloRepeat",
                             "TimeBeamGasHaloRepeat",
                             "NZeroBiasRepeat",
                             "TimeZeroBiasRepeat",
                             "NUnknownRepeat",
                             "TimeUnknownRepeat"
                            };
    char* pixelPlots[5]   = {"pixel_A_occupancy",
                             "pixel_B0_occupancy",
                             "pixel_B1_occupancy",
                             "pixel_B2_occupancy",
                             "pixel_C_occupancy"
                            };
    char* pixLayers[5]    = {"ecA","l0","l1","l2","ecC"};
    char* sctPlots[4]     = {"sct_B0_occupancy",
                             "sct_B1_occupancy",
                             "sct_B2_occupancy",
                             "sct_B3_occupancy"
                            };
    char* sctLayers[4]    = {"l0","l1","l2","l3"};
    
    ///======================================
    /// Event percentage recycling
    ///======================================
    
    for (unsigned int i = 0; i < 11; i++) {
    
      sprintf(histoName,"Pileup/%s",pileupPlots[i]);
      sprintf(canvName,"%s.%s",pileupPlots[i],plotType);
      
      h = (TH1F*)gDirectory->Get(histoName);
      if (h) {
        h->SetFillColor(kBlue);
        h->Draw();
        canv->SaveAs(canvName);
        canv->Print(psoutfile);
      }
    }
    
    ///======================================
    /// Pixel occupancy
    ///======================================
    TH1F *h_pixelOccupancy = new TH1F("pixelOccupancy","pixelOccupancy (%)",5,0.5,5.5);
    for (unsigned int i = 0; i < 5; i++) {
    
      sprintf(histoName,"Pixel/%s",pixelPlots[i]);
      sprintf(canvName,"%s.%s",pixelPlots[i],plotType);
      
      h2D = (TH2D*)gDirectory->Get(histoName);
      if (h2D) {
        h2D->Draw("colz");
        canv->SaveAs(canvName);
        canv->Print(psoutfile);
        h_pixelOccupancy->SetBinContent(i+1,getOccupancy(h2D,pixLayers[i],nEvents));
      }
    }
    
    // Pixel Occupancy Summary
    if(h2D) {   
      h_pixelOccupancy->GetXaxis()->SetBinLabel(1,"EC_A");
      h_pixelOccupancy->GetXaxis()->SetBinLabel(2,"L0");
      h_pixelOccupancy->GetXaxis()->SetBinLabel(3,"L1");
      h_pixelOccupancy->GetXaxis()->SetBinLabel(4,"L2");
      h_pixelOccupancy->GetXaxis()->SetBinLabel(5,"EC_C");
      h_pixelOccupancy->SetMarkerSize(1.5);
      h_pixelOccupancy->SetMarkerStyle(20);
      h_pixelOccupancy->Draw("P");
      canv->SaveAs("pixelOcc_Summary.png");
      canv->Print(psoutfile); 
    }
    
    ///======================================
    /// SCT occupancy
    ///======================================
    TH1F *h_sct_Occupancy = new TH1F("sct_Occupancy","sct_Occupancy (%)",4,0.5,4.5);
    for (unsigned int i = 0; i < 4; i++) {
    
      sprintf(histoName,"SCT/%s",sctPlots[i]);
      sprintf(canvName,"%s.%s",sctPlots[i],plotType);
      
      h2D = (TH2D*)gDirectory->Get(histoName);
      if (h2D) {
        h2D->Draw("colz");
        canv->SaveAs(canvName);
        canv->Print(psoutfile);
        h_sct_Occupancy->SetBinContent(i+1,getSCTOccupancy(h2D,sctLayers[i],nEvents));
      }
    }

    // SCT Occupancy Summary
    if(h2D) {
      h_sct_Occupancy->GetXaxis()->SetBinLabel(1,"L0");
      h_sct_Occupancy->GetXaxis()->SetBinLabel(2,"L1");
      h_sct_Occupancy->GetXaxis()->SetBinLabel(3,"L2");
      h_sct_Occupancy->GetXaxis()->SetBinLabel(4,"L3");
      h_sct_Occupancy->SetMarkerSize(1.5);
      h_sct_Occupancy->SetMarkerStyle(20);
      h_sct_Occupancy->Draw("P");
      canv->SaveAs("sct_Occ_Summary.png");
      canv->Print(psoutfile);
    }

    ///======================================
    /// MC Collection Checker
    ///======================================
    TChain *mychain = new TChain("MCTestTree");
    mychain->Add(inFile);
    MakeMCTestHistos(mychain,canv,psoutfile);


    // Close the .ps file
    canv->Print(psend);
}
