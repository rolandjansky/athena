/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TH2F.h"
#include "TGraph.h"

void track_pixel_res_hitcount(char FILE[100]="ntuple.root") {

  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPadBorderMode(0);
  //gStyle->SetFillStyle(0);
  //gStyle->SetFillColor(10);
  gStyle->SetOptFit(1111);
  
  gROOT->Reset();


  //const int LAYERS = 3;
  //const int MODULES_IN_PHI = 2;
  //const int CHIPS_PER_MODULE_IN_COL=8;
  //const int CHIPS_PER_MODULE_IN_ROW=2;
  const int MAX_PIX_HITS = 8;           //max no. of pixel hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event
  const char HISTOTYPE[10] = "";
  //const char FILE[40] = "ntuple.root";
  //const char FILE[100] = "ntuple.87new-STBLine-biased.root";
  //const int ROW_RESOLUTION=4;
  //const int COL_RESOLUTION=4;

  //int layer,phi,row,col,module,tot;
  //int i;
  //char options[80];
  //float treevariable1;
  //float treevariable2;
  char name[180],name2[80];

  //layer=0;
  //phi=0;

  TFile *file = new TFile(FILE);
  TTree* tree = (TTree*)gDirectory->Get("TB/tree");

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("trk_mc_*",0);
//  tree->SetBranchStatus("Event",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, ntracks=0;
  int pixel_nhits[MAX_TRACKS];
  float pixel_res_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_res_eta[MAX_TRACKS][MAX_PIX_HITS];
  float pixel_pull_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_pull_eta[MAX_TRACKS][MAX_PIX_HITS];

  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nPixelHits", pixel_nhits);
  tree->SetBranchAddress("trk_PixResPhi", pixel_res_phi);
  tree->SetBranchAddress("trk_PixResEta", pixel_res_eta);
  tree->SetBranchAddress("trk_PixPullPhi", pixel_pull_phi);
  tree->SetBranchAddress("trk_PixPullEta", pixel_pull_eta);


  cout<< "got pointers to ntuple data" <<endl;

  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: Pixel Residuals Eta");
  c11->SetFillColor(10);
  TCanvas *c12 = new TCanvas("c12","Tracking Offline Monitor: Pixel Residuals Phi");
  c12->SetFillColor(10);
  //sprintf(name,"%d events", entries);
  //c11->SetLabel(name);

  cout<< "making canvases..." <<endl;

  c11->Divide(4,2);
  c12->Divide(4,2);
  TPad* subcanvas;
  for (int i=1; i <= 4; i++) {
    sprintf(name,"c11_%d", i);
    subcanvas = (TPad*)(c11->GetPrimitive(name));
    subcanvas->SetFillStyle(4000);
    sprintf(name,"c12_%d", i);
    subcanvas = (TPad*)(c12->GetPrimitive(name));
    subcanvas->SetFillStyle(4000);
  }

  cout << "canvases done" << endl;

  TH1F *histoResPhi[4];
  TH1F *histoResEta[4];
  TH1F *histoPullPhi[4];
  TH1F *histoPullEta[4];

  for (int i=1; i <= 4; i++) {
    sprintf(name,"Pixel Residuals Phi %d hits", i);
    sprintf(name2,"PixResPhi_%d", i);
    histoResPhi[i-1] = new TH1F(name2,name,100,-0.08,0.08);
    //histoResPhi[i-1]->SetXTitle("r_{#phi}  = #phi_{track} - #phi_{hit} [mm]");
    //histoResPhi->GetXaxis()->SetTitleFont(22);
    //histoResPhi->GetXaxis()->SetTitleSize(.04);

    sprintf(name,"Pixel Residuals Eta %d hits", i);
    sprintf(name2,"PixResEta_%d", i);
    histoResEta[i-1] = new TH1F(name2,name,100,-0.6,0.6);
    //histoResEta->SetXTitle("r_{#eta}  = #eta_{track} - #eta_{hit} [mm]");
    //histoResEta->GetXaxis()->SetTitleFont(22);
    //histoResEta->GetXaxis()->SetTitleSize(.04);

    sprintf(name,"Pixel Pull Phi %d hits", i);
    sprintf(name2,"PixPullPhi_%d", i);
    histoPullPhi[i-1] = new TH1F(name2,name,100,-5.,5.);
    //histoPullPhi->SetXTitle("#frac{ r_{#phi} }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
    //histoPullPhi->GetXaxis()->SetTitleSize(0.03);
    //histoPullPhi->GetXaxis()->SetTitleOffset(1.0);
    //histoPullPhi->GetXaxis()->SetTitleFont(22);
    //histoPullPhi->GetXaxis()->SetTitleSize(.02);

    sprintf(name,"Pixel Pull Eta %d hits", i);
    sprintf(name2,"PixPullEta_%d", i);
    histoPullEta[i-1] = new TH1F(name2,name,100,-5.,5.);
    //histoPullEta->SetXTitle("#frac{ r_{#eta} }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
    //histoPullEta->GetXaxis()->SetTitleSize(0.03);
    //histoPullEta->GetXaxis()->SetTitleOffset(1.0);
    //histoPullEta->GetXaxis()->SetTitleFont(22);
    //histoPullEta->GetXaxis()->SetTitleSize(.03);
  }

  cout << "histos initialized" << endl;
  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      // loop over all pixel hits of current track
      for (hit=0; hit<pixel_nhits[track]; hit++) {
        if (pixel_nhits[track] < 4) {
          histoResPhi[pixel_nhits[track]-1]->Fill(pixel_res_phi[track][hit]);
          histoResEta[pixel_nhits[track]-1]->Fill(pixel_res_eta[track][hit]);
          histoPullPhi[pixel_nhits[track]-1]->Fill(pixel_pull_phi[track][hit]);
          histoPullEta[pixel_nhits[track]-1]->Fill(pixel_pull_eta[track][hit]);
        }
        else {
          histoResPhi[3]->Fill(pixel_res_phi[track][hit]);
          histoResEta[3]->Fill(pixel_res_eta[track][hit]);
          histoPullPhi[3]->Fill(pixel_pull_phi[track][hit]);
          histoPullEta[3]->Fill(pixel_pull_eta[track][hit]);
        }
      }
    }
  }


  cout << "histos filled" << endl;

  for (int i=1; i <= 4; i++) {
    c12->cd(i);
    //c11_1->SetFillColor(10);
    histoResPhi[i-1]->SetFillStyle(4000);
    histoResPhi[i-1]->Draw(HISTOTYPE);
    histoResPhi[i-1]->Fit("gaus");
    histoResPhi[i-1]->GetFunction("gaus")->SetLineColor(kRed);
    histoResPhi[i-1]->GetFunction("gaus")->SetLineWidth(2);
    c12->Update();

    c11->cd(i);
    //c11_2->SetFillColor(10);
    histoResEta[i-1]->SetFillStyle(4000);
    histoResEta[i-1]->Draw(HISTOTYPE);
    histoResEta[i-1]->Fit("gaus");
    histoResEta[i-1]->GetFunction("gaus")->SetLineColor(kRed);
    histoResEta[i-1]->GetFunction("gaus")->SetLineWidth(2);
    c11->Update();

    c12->cd(i+4);
    //c11_3->SetFillColor(10);
    histoPullPhi[i-1]->SetFillStyle(4000);
    histoPullPhi[i-1]->Draw(HISTOTYPE);
    histoPullPhi[i-1]->Fit("gaus");
    histoPullPhi[i-1]->GetFunction("gaus")->SetLineColor(kRed);
    histoPullPhi[i-1]->GetFunction("gaus")->SetLineWidth(2);
    c12->Update();

    c11->cd(i+4);
    //c11_4->SetFillColor(10);
    histoPullEta[i-1]->SetFillStyle(4000);
    histoPullEta[i-1]->Draw(HISTOTYPE);
    histoPullEta[i-1]->Fit("gaus");
    histoPullEta[i-1]->GetFunction("gaus")->SetLineColor(kRed);
    histoPullEta[i-1]->GetFunction("gaus")->SetLineWidth(2);
    c11->Update();
  }

  c11->cd();
  sprintf(name,"%d events", entries);
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name);
  sprintf(name,"%s", FILE);
  text = pt->AddText(name);
  pt->Draw();
  //c11->Modified();
  c11->Update();

  c12->cd();
  sprintf(name,"%d events", entries);
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name);
  sprintf(name,"%s", FILE);
  text = pt->AddText(name);
  pt->Draw();
  //c11->Modified();
  c12->Update();

  cout << "done" << endl;
}

