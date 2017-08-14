//
//    Look at local hit positions
//
#ifndef __CINT__
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
using std::cout;
using std::endl;
#endif

void stripHit() {
    gROOT->SetStyle("Plain");

    TString inputFilename = "SiHitAnalOutput.root"; 
    cout << "Opening " << inputFilename << " for reading" << endl;
    TFile *f = TFile::Open(inputFilename);

    UInt_t nStripHits;
    std::vector<double> *stripHitLocX = 0;
    std::vector<double> *stripHitLocY = 0;
    std::vector<double> *stripHitLocZ = 0;
    std::vector<double> *stripHitBarrelEC = 0;
    std::vector<double> *stripHitLayer = 0;
    std::vector<double> *stripHitEta = 0;
    std::vector<double> *stripHitPhi = 0;
    std::vector<double> *stripHitSide = 0;

    gStyle->SetOptStat(110010); // 110010: overflow, underflow, entries
 
    TH1F *h_nStripHits = new TH1F("nStripHits", "No strip hits per event; nHits; nEvents", 110, -10., 100.0);

    TH1F *h_BlocalX = new TH1F("BlocalX", "B Hit local X-coordinate; x [mm]; nEvents", 120, -60.0, 60.0);
    TH1F *h_BlocalY = new TH1F("BlocalY", "B Hit local Y-coordinate; y [mm]; nEvents", 120, -60.0, 60.0);
    TH1F *h_BlocalZ = new TH1F("BlocalZ", "B Hit local Z-coordinate; z [mm]; nEvents", 40, -0.2, 0.2);
    TH2F *h_BlocalYX = new TH2F("BlocalYX", "B Hit local coords Y vs X; x [mm]; y[mm]", 120, -60.0, 60.0, 120, -60.0, 60.0);

    TH1F *h_EClocalX = new TH1F("EClocalX", "EC Hit local X-coordinate; x [mm]; nEvents", 700, 300., 1000.);
    TH1F *h_EClocalY = new TH1F("EClocalY", "EC Hit local Y-coordinate; y [mm]; nEvents", 140, -70.0, 70.0);
    TH1F *h_EClocalZ = new TH1F("EClocalZ", "EC Hit local Z-coordinate; z [mm]; nEvents", 40, -0.2, 0.2);
    TH2F *h_EClocalYX = new TH2F("EClocalYX", "EC Hit local coords Y vs X; x [mm]; y[mm]", 700, 300., 1000., 140, -70.0, 70.0);

    gStyle->SetOptStat(0);
    TH2F *h_localYXzoomI = new TH2F("localYXzoomI", "Hit local coords Y vs X zoom inner; x [mm]; y[mm]",
                                    500, 350., 400.0, 50, -50.0, 50.0);
    TH2F *h_localYXzoomO = new TH2F("localYXzoomO", "Hit local coords Y vs X zoom outer; x [mm]; y[mm]",
                                    120, 950., 980.0, 110, -55.0, 55.0);

    gStyle->SetOptStat(110010); // 110010: overflow, underflow, entries

    TH1F *h_stripHitBarrelEC = new TH1F("bec", "Barrel_endcap index; index; nG4Hits", 10, -5., 5.);
    TH1F *h_stripHitLayer = new TH1F("layer", "Layer_disk index; index; nG4Hits", 10, 0., 10.);
    TH1F *h_stripHitEta = new TH1F("eta", "EtaModule index; index; nG4Hits", 120, -60., 60.);
    TH1F *h_stripHitPhi = new TH1F("phi", "PhiModule index; index; nG4Hits", 100, 0., 100.);
    TH1F *h_stripHitSide = new TH1F("side", "Module-Side index; index; nG4Hits", 2, 0., 2.);
    cout << "Booked histograms\n";
//
// Event loop
//
    TTree *t = (TTree*)f->Get("hitTree");
    t->SetBranchAddress("NStripHits", &nStripHits);
//
//    Hits are stored in the SiHit frame: 0-coord is z, 1-coord is y, 2-coord is x where x, y, z are the GeoModel frame
//    Swap everything to local frame here
//
    t->SetBranchAddress("StripHit_LocalX", &stripHitLocZ);
    t->SetBranchAddress("StripHit_LocalY", &stripHitLocY);
    t->SetBranchAddress("StripHit_LocalZ", &stripHitLocX);

    t->SetBranchAddress("StripHit_BarrelEC", &stripHitBarrelEC);
    t->SetBranchAddress("StripHit_LayerDisk", &stripHitLayer);
    t->SetBranchAddress("StripHit_EtaModule", &stripHitEta);
    t->SetBranchAddress("StripHit_PhiModule", &stripHitPhi);
    t->SetBranchAddress("StripHit_Side", &stripHitSide);

    Int_t nentries = (Int_t)t->GetEntries();
    cout << "Reading " << nentries << " entries" << endl;
    for (Long64_t ientry = 0; ientry < nentries; ientry++) {
        t->GetEntry(ientry);
        h_nStripHits->Fill(nStripHits);
        for (Int_t s = 0; s < nStripHits; s++) {
            if (stripHitBarrelEC->at(s) == 0) {
                h_BlocalX->Fill(stripHitLocX->at(s));
                h_BlocalY->Fill(stripHitLocY->at(s));
                h_BlocalZ->Fill(stripHitLocZ->at(s));
                h_BlocalYX->Fill(stripHitLocX->at(s), stripHitLocY->at(s));
            }
            else {
                h_EClocalX->Fill(stripHitLocX->at(s));
                h_EClocalY->Fill(stripHitLocY->at(s));
                h_EClocalZ->Fill(stripHitLocZ->at(s));
                h_EClocalYX->Fill(stripHitLocX->at(s), stripHitLocY->at(s));
            }
            h_localYXzoomI->Fill(stripHitLocX->at(s), stripHitLocY->at(s));
            h_localYXzoomO->Fill(stripHitLocX->at(s), stripHitLocY->at(s));

            h_stripHitBarrelEC->Fill(stripHitBarrelEC->at(s));
            h_stripHitLayer->Fill(stripHitLayer->at(s));
            h_stripHitEta->Fill(stripHitEta->at(s));
            h_stripHitPhi->Fill(stripHitPhi->at(s));
            h_stripHitSide->Fill(stripHitSide->at(s));

        }
    }
//
//    Draw plots
//
    gStyle->SetOptStat(110010); // 110010: overflow, underflow, entries
    TCanvas *c1 = new TCanvas("c_nStripHits","nStripHits", 800, 600);
    h_nStripHits->Draw();
    c1->SaveAs("nStripHits.pdf");

    TCanvas *c2 = new TCanvas("c_BlocalHit","Barrel Local Hit Positions", 800, 600);
    c2->Divide(2, 2);
    c2->cd(1);
    h_BlocalX->Draw();
    c2->cd(2);
    h_BlocalZ->Draw();
    c2->cd(3);
    h_BlocalYX->Draw("COLZ");
    c2->cd(4);
    h_BlocalY->Draw();
    c2->SaveAs("localHit.pdf");

    TCanvas *c3 = new TCanvas("c_EClocalHit","EC Local Hit Positions", 800, 600);
    c3->Divide(2, 2);
    c3->cd(1);
    h_EClocalX->Draw();
    c3->cd(2);
    h_EClocalZ->Draw();
    c3->cd(3);
    h_EClocalYX->Draw("COLZ");
    c3->cd(4);
    h_EClocalY->Draw();
    c3->SaveAs("localHit.pdf");

    TCanvas *c4 = new TCanvas("c_Indexes","Hit module indexes", 1000, 600);
    c4->Divide(3, 2);
    c4->cd(1);
    h_stripHitBarrelEC->Draw();
    c4->cd(2);
    h_stripHitLayer->Draw();
    c4->cd(3);
    h_stripHitPhi->Draw();
    c4->cd(4);
    h_stripHitEta->Draw();
    c4->cd(5);
    h_stripHitSide->Draw();
    c4->SaveAs("indexes.pdf");

    TCanvas *c5 = new TCanvas("c_Zoom","Local Hit Positions Zoomed", 800, 600);
    c5->Divide(2, 1);
    c5->cd(1);
    h_localYXzoomI->Draw("COLZ");
    c5->cd(2);
    h_localYXzoomO->Draw("COLZ");
    c5->SaveAs("zoom.pdf");

}
