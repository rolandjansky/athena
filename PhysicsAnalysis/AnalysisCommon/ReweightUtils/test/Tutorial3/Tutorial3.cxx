/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweightND.h"
#include "APWeightEntry.h"
#include "APEvtWeight.h"
#include "TH2D.h"
#include "TStyle.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {

  //============= Initialize ===================================================
  gROOT->ProcessLine("#include <vector>");
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetTitleYOffset(1.55);

  //Open file containing muon trigger maps
  TFile *muonmap_file = TFile::Open("../Common/MuonMaps.root");
  //Open file containing muon trigger maps
  TFile *electronmap_file = TFile::Open("../Common/ElectronMaps.root");
  //Read in maps from file and project into 2D
  THnSparseD* numerator_mu = (THnSparseD*) muonmap_file->Get("passes");
  THnSparseD* denominator_mu = (THnSparseD*) muonmap_file->Get("trials");
  THnSparseD* numerator_el = (THnSparseD*) electronmap_file->Get("passes");
  THnSparseD* denominator_el = (THnSparseD*) electronmap_file->Get("trials");

  //Open file containing pseudoevents
  TFile *event_file = TFile::Open("../Common/LeptonDistributions.root");
  //Read in Tree from file
  TTree *event_tree = (TTree*) event_file->Get("LeptonDistributions");
  //Create and connect variables to tree
  vector<float> *mu_eta = 0;
  vector<float> *mu_phi = 0;
  vector<float> *el_eta = 0;
  vector<float> *el_phi = 0;
  float etmiss;
  float sumet;
  event_tree->SetBranchAddress("mu_eta", &mu_eta);
  event_tree->SetBranchAddress("mu_phi", &mu_phi);
  event_tree->SetBranchAddress("el_eta", &el_eta);
  event_tree->SetBranchAddress("el_phi", &el_phi);
  event_tree->SetBranchAddress("etmiss", &etmiss);
  event_tree->SetBranchAddress("sumet", &sumet);
  
  //Create Instances of APReweightND for muons & electrons
  //////////////////////////////////////////////////////////////////////////////
  APReweightND* weighter_mu = new APReweightND(denominator_mu, numerator_mu, true);
  //APReweightND* weighter_el = new APReweightND(denominator_el, numerator_el, true);
  //////////////////////////////////////////////////////////////////////////////

  //Create 2D histogram for the reweighted distribution
  TH2D *reweight_hist = new TH2D("reweight_hist","Reweighted events",80,0.,100.,120,0.,200.);
  reweight_hist->GetXaxis()->SetTitle("MET [GeV]");
  reweight_hist->GetYaxis()->SetTitle("SUMET [GeV]");
  
  //============= Event Loop ===================================================
  for (unsigned int i = 0, I = event_tree->GetEntries(); i < I; ++i) {
    event_tree->GetEntry(i);
    
    //Create object specific event weights
    ////////////////////////////////////////////////////////////////////////////
    APEvtWeight MuonWeight(APEvtWeight::kMuon);
    //APEvtWeight ElectronWeight(APEvtWeight::kElectron);
    ////////////////////////////////////////////////////////////////////////////
     
    //Muon reweighting
    for (unsigned int j = 0, J = mu_eta->size(); j < J; ++j) {
      double variables[2] = {(*mu_eta)[j],(*mu_phi)[j]};
      MuonWeight.AddWeightToEvt(weighter_mu->GetWeight(variables));
    }
    
    //Electron reweighting
    ////////////////////////////////////////////////////////////////////////////
    //for (unsigned int j = 0, J = el_eta->size(); j < J; ++j) {
    //  double variables[2] = {(*el_eta)[j],(*el_phi)[j]};
    //  ElectronWeight.AddWeightToEvt(weighter_el->GetWeight(variables));
    //}
    ////////////////////////////////////////////////////////////////////////////
    
    //Logical OR of muon and electron trigger
    ////////////////////////////////////////////////////////////////////////////
    //APEvtWeight ORedWeight = MuonWeight || ElectronWeight;
    ////////////////////////////////////////////////////////////////////////////
    
    //Fill histogram (Remember to change MuonWeight -> ORedWeight for 2nd part of tutorial 3)
    ////////////////////////////////////////////////////////////////////////////
    reweight_hist->Fill(etmiss/1000.,sumet/1000.,MuonWeight.GetWeight());
    ////////////////////////////////////////////////////////////////////////////
  }

  //============= Finalize =====================================================
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 900);
  //Fixed normalization for better comparability (comment for last part of tutorial 3)
  //////////////////////////////////////////////////////////////////////////////
  reweight_hist->GetZaxis()->SetRangeUser(0.,70.);
  //////////////////////////////////////////////////////////////////////////////
  reweight_hist->Draw("COLZ");
  canvas->Print("reweight_hist.eps", "eps");
  
  delete canvas;
  delete numerator_mu;
  delete denominator_mu;
  delete numerator_el;
  delete denominator_el;
  delete reweight_hist;
  //Cleanup
  //////////////////////////////////////////////////////////////////////////////
  delete weighter_mu;
  //delete weighter_el;
  //////////////////////////////////////////////////////////////////////////////

  return 0;
}
