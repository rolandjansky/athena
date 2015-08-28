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
  
  // Fix vector<something> issues
  gSystem->Load("libCintex");
  ROOT::Cintex::Cintex::Enable();
  
  // List of plots to draw
  char  histoName[100];
  char  canvName[100];
  char  plotType[100];
  char* branchNames[32] = {
    "Mueta",
    "Muphi",
    "Mupt",
    "MuAuth",
    "Muchi2",
    "Muetisol",
    "NElectrons",
    "ElectronEta",
    "ElectronPt",
    "NJets",
    "JetEta",
    "JetPt",
    "NJetEles",
    "JetEleEta",
    "JetElePhi",
    "JetEleEEM",
    "JetEleEHAD",
    "iFirstCont",
    "ndigit_tot",
    "NJetEles",
    "NJetElesEM", 
    "NJetElesHAD", 
    "NJetElesEM_zeroEt", 
    "NJetElesHAD_zeroEt", 
    "JetEleEta",
    "JetElePhi",
    "JetEleEEM",
    "JetEleEHAD", 
    "JetEleTotEt",
    "TirggerTower_EM_Et",
    "TirggerTower_HAD_Et",
    "TirggerTower_EMet_to_HADet"
   };
   
  // Draw plots and save them
  sprintf(plotType,"png");
  for (unsigned int i = 0; i < 32; i++) {
    sprintf(canvName,"%s.%s",branchNames[i],plotType);
    chain->Draw(branchNames[i]);
    canvas->SaveAs(canvName);
    canvas->Print(psFile);
  }

}

void OverlayRTT_Plots(TString inFile = "rttmon.aan.root" )
{
  // Set style
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetMarkerStyle(6);

  gSystem->Load("libCintex");
  ROOT::Cintex::Cintex::Enable();

  // Load the ntuple in a chain
  TChain *chain = new TChain("CollectionTree");
  chain->Add(inFile);
  Long64_t nEvents = chain->GetEntries();
 
 // The canvas for our fitted spectrum
 TCanvas *canv = new TCanvas("canv","canv",8,49,501,375);
 
 char psoutfile[200];
 char psbegin[200];
 char psend[200];
 
  //  Setup the output PostScript file
  sprintf(psoutfile,"OverlayRTT.plots.ps");
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
      char* OverlayPlots[3] = {"jeteletotEt","jeteleEM_Et","jeteleHAD_Et"
      };
   
        for (unsigned int i = 0; i < 3; i++) {
   
          sprintf(histoName,"Overlay/%s",OverlayPlots[i]);
          sprintf(canvName,"%s.%s",OverlayPlots[i],plotType);
     
          h = (TH1F*)gDirectory->Get(histoName);
          if (h) {
            h->SetFillColor(kBlue);
            h->Draw();
            canv->SaveAs(canvName);
            canv->Print(psoutfile);
          }
        }
            // Close the .ps file
            canv->Print(psend);
}
