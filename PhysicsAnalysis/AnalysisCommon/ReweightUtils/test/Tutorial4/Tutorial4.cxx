/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweight2D.h"
#include "APWeightEntry.h"
#include "APEvtWeight.h"
#include "APWeightSum.h"
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
  TFile *muonmap_file = TFile::Open("../Common/EF_mu18_muid_Tutorial.root");

  //Read in maps from file and project into 2D
  THnSparseD* numerator_mu = (THnSparseD*) muonmap_file->Get("passes");
  numerator_mu->GetAxis(0)->SetRange(34, 100);
  THnSparseD* denominator_mu = (THnSparseD*) muonmap_file->Get("trials");
  denominator_mu->GetAxis(0)->SetRange(34, 100);
  TH2D* numerator_mu_proj = (TH2D*) numerator_mu->Projection(2, 1);
  TH2D* denominator_mu_proj = (TH2D*) denominator_mu->Projection(2, 1);

  //Rebin 2D histograms:
  int N_Eta = 50;
  int N_Phi = 50;
  numerator_mu_proj->Rebin2D(100 / N_Eta, 100 / N_Phi);
  denominator_mu_proj->Rebin2D(100 / N_Eta, 100 / N_Phi);
  
  //Open file containing pseudoevents
  TFile *event_file = TFile::Open("../Common/DiLeptonDistributions.root");
  //Read in Tree from file
  TTree *event_tree = (TTree*) event_file->Get("LeptonDistributions");
  //Create and connect variables to tree
  vector<float> *mu_pt = 0;
  vector<float> *mu_eta = 0;
  vector<float> *mu_phi = 0;

  event_tree->SetBranchAddress("mu_pt", &mu_pt);
  event_tree->SetBranchAddress("mu_eta", &mu_eta);
  event_tree->SetBranchAddress("mu_phi", &mu_phi);
  
  //Create Instance of APReweight2D
  //////////////////////////////////////////////////////////////////////////////
  APReweight2D* weighter_mu = new APReweight2D(denominator_mu_proj, numerator_mu_proj, true);
  //////////////////////////////////////////////////////////////////////////////
  
  //Create Instance of APWeightSum
  //////////////////////////////////////////////////////////////////////////////
  APWeightSum* sum_evts = new APWeightSum();
  //////////////////////////////////////////////////////////////////////////////
  
  //============= Event Loop ===================================================
  for (unsigned int i = 0, I = event_tree->GetEntries(); i < I; ++i) {
    event_tree->GetEntry(i);
    
    //Create object specific event weights
    ////////////////////////////////////////////////////////////////////////////
    APEvtWeight MuonWeightA(APEvtWeight::kMuon);
    APEvtWeight MuonWeightB(APEvtWeight::kMuon);
    ////////////////////////////////////////////////////////////////////////////
     
    //Muon reweighting
    ////////////////////////////////////////////////////////////////////////////
    //for (unsigned int j = 0, J = mu_eta->size(); j < J; ++j) MuonWeightA.AddWeightToEvt(weighter_mu->GetWeight((*mu_eta)[j],(*mu_phi)[j]));
    MuonWeightA.AddWeightToEvt(weighter_mu->GetWeight((*mu_eta)[0],(*mu_phi)[0]));
    MuonWeightB.AddWeightToEvt(weighter_mu->GetWeight((*mu_eta)[1],(*mu_phi)[1]));
    ////////////////////////////////////////////////////////////////////////////
    
    //Logical OR of both muon triggers (default)
    ////////////////////////////////////////////////////////////////////////////
    APEvtWeight ORedWeight = MuonWeightA || MuonWeightB;
    ////////////////////////////////////////////////////////////////////////////
        
    //Logical AND of both muon triggers (dimuon trigger)
    ////////////////////////////////////////////////////////////////////////////
    //APEvtWeight ANDedWeight = MuonWeightA && MuonWeightB;
    ////////////////////////////////////////////////////////////////////////////

    //Add event weight to counter
    ////////////////////////////////////////////////////////////////////////////
    //sum_evts->AddEvt(&MuonWeightA);
    sum_evts->AddEvt(&ORedWeight);
    //sum_evts->AddEvt(&ANDedWeight);
    ////////////////////////////////////////////////////////////////////////////
  }

  //============= Finalize =====================================================
  cout << "--------------------------------------------------" << endl; 
  cout << "             Sum of Weights: " << sum_evts->GetSumW() << endl;
  cout << "                    Entries: " << sum_evts->GetKUnweighted() << endl;
  cout << "         Standard Deviation: " << sum_evts->GetStdDev() << endl;
  cout << "--------------------------------------------------" << endl; 
  cout << "        StdDev Uncorrelated: " << sqrt(sum_evts->GetVarianceNoCorr()) << endl;
  cout << "    StdDev Fully Correlated: " << sqrt(sum_evts->GetVarianceFullCorr()) << endl;
  cout << "--------------------------------------------------" << endl; 
   
  delete numerator_mu;
  delete denominator_mu;
  delete weighter_mu;
  
  return 0;
}
