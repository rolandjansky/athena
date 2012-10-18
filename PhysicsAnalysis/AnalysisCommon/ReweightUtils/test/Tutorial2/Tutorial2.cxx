/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweight2D.h"
#include "APWeightEntry.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TStyle.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TLegend.h"
#include "TRandom3.h"
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
  gStyle->SetPadLeftMargin(0.13);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetTitleYOffset(1.6);

  //Open file containing muon trigger maps (only change in last part of tutorial 2!)
  //////////////////////////////////////////////////////////////////////////////
  TFile *muonmap_file = TFile::Open("../Common/EF_mu18_muid_Tutorial.root");
  //TFile *muonmap_file = TFile::Open("../Common/EF_mu18_muid_Tutorial_upToPeriodD2.root");
  //////////////////////////////////////////////////////////////////////////////

  //Read in maps from file and project into 2D
  THnSparseD* numerator_mu = (THnSparseD*) muonmap_file->Get("passes");
  numerator_mu->GetAxis(0)->SetRange(34, 100);
  THnSparseD* denominator_mu = (THnSparseD*) muonmap_file->Get("trials");
  denominator_mu->GetAxis(0)->SetRange(34, 100);
  TH2D* numerator_mu_proj = (TH2D*) numerator_mu->Projection(2, 1);
  TH2D* denominator_mu_proj = (TH2D*) denominator_mu->Projection(2, 1);

  //Rebin 2D histograms:
  //////////////////////////////////////////////////////////////////////////////
  int N_Eta = 25;
  int N_Phi = 25;
  numerator_mu_proj->Rebin2D(100 / N_Eta, 100 / N_Phi);
  denominator_mu_proj->Rebin2D(100 / N_Eta, 100 / N_Phi);
  //////////////////////////////////////////////////////////////////////////////

  //Open file containing pseudoevents from data & prediction
  TFile *event_file = TFile::Open("../Common/TopMuonPhiHists.root");
  //Read in data and prediction histograms from file
  TH1D *data_pdf = (TH1D*) event_file->Get("data");
  TH1D *pred_pdf = (TH1D*) event_file->Get("prediction");

  //Create Instance of APReweight2D
  //////////////////////////////////////////////////////////////////////////////
  //APReweight2D* weighter_mu = new APReweight2D(denominator_mu_proj, numerator_mu_proj, true);
  //////////////////////////////////////////////////////////////////////////////

  //Create 2D histogram for the reweighted distribution
  TH1D *data_hist = new TH1D("data_hist", "Data", 25, -3.1415, 3.1415);
  data_hist->GetXaxis()->SetTitle("Muon #phi");
  data_hist->GetYaxis()->SetTitle("Events");
  data_hist->SetLineColor(2);
  data_hist->SetLineWidth(2);
  TH1D *pred_hist = new TH1D("pred_hist", "Prediction", 25, -3.1415, 3.1415);
  pred_hist->GetXaxis()->SetTitle("Muon #phi");
  pred_hist->GetYaxis()->SetTitle("Events");
  pred_hist->SetLineWidth(2);


  //============= Event Loop ===================================================
  for (unsigned int i = 0; i < 100000; ++i) {
    //Generate data and prediction phi values from PDFs
    double phi_data = data_pdf->GetRandom();
    double phi_pred = pred_pdf->GetRandom();

    //Muon reweighting
    ////////////////////////////////////////////////////////////////////////////
    data_hist->Fill(phi_data);
    //Unweighted Case (comment the next line for the weighted case (2nd part of tutorial 2))
    pred_hist->Fill(phi_pred);
    //Weighted Case (uncomment the next line for the weighted case (2nd part of tutorial 2))
    //pred_hist->Fill(phi_pred, weighter_mu->GetWeight(gRandom->Uniform(-2.4, 2.4), phi_pred)->GetExpectancy());
    ////////////////////////////////////////////////////////////////////////////
  }

  //============= Finalize =====================================================
  data_hist->Scale(18854. / data_hist->GetSum());
  pred_hist->Scale(18952. / pred_hist->GetSum());

  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  pred_hist->Draw("");
  pred_hist->SetMinimum(0.);
  pred_hist->SetMaximum(1.4 * pred_hist->GetMaximum());
  data_hist->Draw("ESAME");
  TLegend *legend = canvas->BuildLegend(0.6, 0.2, 0.9, 0.4);
  legend->SetFillColor(0);
  canvas->Print("TopWeightingMuonPhi.eps", "eps");

  canvas->SetLeftMargin(0.12);
  canvas->SetRightMargin(0.15);
  numerator_mu_proj->Divide(denominator_mu_proj);
  numerator_mu_proj->GetXaxis()->SetTitle("Muon #eta");
  numerator_mu_proj->GetYaxis()->SetTitle("Muon #phi");
  numerator_mu_proj->GetYaxis()->SetTitleOffset(1.2);
  numerator_mu_proj->Draw("COLZ");
  canvas->Print("MuonMap.eps", "eps");

  delete canvas;
  delete numerator_mu;
  delete denominator_mu;
  delete numerator_mu_proj;
  delete denominator_mu_proj;
  delete data_hist;
  delete pred_hist;
  delete data_pdf;
  delete pred_pdf;
  //Cleanup
  //////////////////////////////////////////////////////////////////////////////
  //delete weighter_mu;
  //////////////////////////////////////////////////////////////////////////////

  return 0;
}
