/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweight.h"
#include "APWeightEntry.h"
#include "TH1D.h"
#include "TStyle.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
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
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetTitleYOffset(1.55);

  //Open file containing muon trigger maps
  TFile *muonmap_file = TFile::Open("../Common/EF_mu18_muid_Tutorial.root");
  //Read in maps from file and project into 2D
  THnSparseD* numerator_mu = (THnSparseD*) muonmap_file->Get("passes");
  THnSparseD* denominator_mu = (THnSparseD*) muonmap_file->Get("trials");
  TH1D* numerator_mu_proj = (TH1D*) numerator_mu->Projection(0);
  TH1D* denominator_mu_proj = (TH1D*) denominator_mu->Projection(0);

  //Open file containing pseudoevents
  TFile *event_file = TFile::Open("../Common/LeptonDistributions.root");
  //Read in Tree from file
  TTree *event_tree = (TTree*) event_file->Get("LeptonDistributions");
  //Create and connect variables to tree
  vector<float> *mu_pt = 0;
  event_tree->SetBranchAddress("mu_pt", &mu_pt);

  //Create Instance of APReweight
  //////////////////////////////////////////////////////////////////////////////
  //APReweight* weighter_mu = new APReweight(denominator_mu_proj, numerator_mu_proj, true);
  //////////////////////////////////////////////////////////////////////////////

  //Create 2D histogram for the reweighted distribution
  TH1D *original_hist = new TH1D("original_hist", "Unweighted events", 60, 0., 60.);
  original_hist->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  original_hist->GetYaxis()->SetTitle("Events");
  TH1D *reweight_hist = new TH1D("reweight_hist", "Reweighted events", 60, 0., 60.);
  reweight_hist->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  reweight_hist->GetYaxis()->SetTitle("Events");
  reweight_hist->SetLineColor(2);


  //============= Event Loop ===================================================
  for (unsigned int i = 0, I = event_tree->GetEntries(); i < I; ++i) {
    event_tree->GetEntry(i);

    //Muon reweighting
    for (unsigned int j = 0, J = mu_pt->size(); j < J; ++j) {
      if ((*mu_pt)[j] < 60000.) {
        //Fill histogram with single muon p_T, weighted with the according trigger weight from turn-on curve
        ////////////////////////////////////////////////////////////////////////
        //reweight_hist->Fill((*mu_pt)[j]/1000.,weighter_mu->GetWeight((*mu_pt)[j])->GetExpectancy());
        ////////////////////////////////////////////////////////////////////////
        original_hist->Fill((*mu_pt)[j] / 1000.);
      }
    }
  }


  //============= Finalize =====================================================
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  original_hist->Draw("E");
  original_hist->SetMinimum(0.);
  original_hist->SetMaximum(1.2 * original_hist->GetMaximum());
  //Draw the reweighted histogram on top of the unweighted one
  //////////////////////////////////////////////////////////////////////////////
  //reweight_hist->Draw("ESAME");
  //////////////////////////////////////////////////////////////////////////////
  TLegend *legend = canvas->BuildLegend(0.6, 0.2, 0.9, 0.4);
  legend->SetFillColor(0);
  canvas->Print("weighting_1D.eps", "eps");

  TGraphAsymmErrors *MuonMap = new TGraphAsymmErrors();
  MuonMap->BayesDivide(numerator_mu_proj, denominator_mu_proj);
  MuonMap->GetXaxis()->SetTitle("Muon p_{T} [MeV]");
  MuonMap->GetYaxis()->SetTitle("Efficiency");
  MuonMap->GetYaxis()->SetTitleOffset(1.2);
  MuonMap->Draw("APZ");
  canvas->Print("MuonMap.eps", "eps");

  delete canvas;
  delete numerator_mu;
  delete denominator_mu;
  delete numerator_mu_proj;
  delete denominator_mu_proj;
  delete reweight_hist;
  //Cleanup the APReweight instance
  //////////////////////////////////////////////////////////////////////////////
  //delete weighter_mu;
  //////////////////////////////////////////////////////////////////////////////

  return 0;
}
