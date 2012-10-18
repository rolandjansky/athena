/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweight.h"
#include "APReweight2D.h"
#include "APReweight3D.h"
#include "APReweightND.h"
#include "APWeightEntry.h"
#include "APWeightHist.h"
#include "APEvtWeight.h"
#include "APWeightSum.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

int CombineWeights() {
APWeightEntry *a = new APWeightEntry(100,60,1.0,true);  
APWeightEntry *b = new APWeightEntry(120,12,1.0,true);  
APWeightEntry *c = new APWeightEntry(1000,300,1.0,true);

APEvtWeight weight1(APEvtWeight::kMuonMO);
APEvtWeight weight2(APEvtWeight::kElectronMO);
                                   
weight1.AddWeightToEvt(a);                                   
weight1.AddWeightToEvt(b);
weight1.AddWeightToEvt(c);
                       
weight2.AddWeightToEvt(a);
weight2.AddWeightToEvt(c);

cout << "Efficiency a:         " << a->GetExpectancy() << "+-" << sqrt(a->GetVariance()) << endl;
cout << "Efficiency b:         " << b->GetExpectancy() << "+-" << sqrt(b->GetVariance()) << endl;
cout << "Efficiency c:         " << c->GetExpectancy() << "+-" << sqrt(c->GetVariance()) << endl;

cout << "Weight 1 (a||b||c):   " << weight1.GetWeight() << "+-" << weight1.GetStdDev() << endl;
cout << "Weight 2 (a||c):      " << weight2.GetWeight() << "+-" << weight2.GetStdDev() << endl;

APEvtWeight weight3 = weight1 && weight2;
APEvtWeight weight4 = weight1 || weight2;

cout << "Weight 1 && Weight 2: " << weight3.GetWeight() << "+-" << weight3.GetStdDev() << endl;
cout << "Weight 1 || Weight 2: " << weight4.GetWeight() << "+-" << weight4.GetStdDev() << endl;

return 0;
}

int ErrorPropagation() {
  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with Gaussian distributions
  std::vector< double > event_values_xA, event_values_yA, event_values_zA, event_values_combA, event_values_isoA;
  std::vector< double > event_values_xB, event_values_yB, event_values_zB, event_values_combB, event_values_isoB;
  int bins[5] = {5, 5, 5, 5, 5};
  double min[5] = {-2.4, -2.4, -2.4, -2.4, -2.4};
  double max[5] = {2.4, 2.4, 2.4, 2.4, 2.4};

  THnSparseD* test_successesA = new THnSparseD("successesA", "successesA", 5, bins, min, max);
  THnSparseD* test_trialsA = new THnSparseD("trialsA", "trialsA", 5, bins, min, max);
  
  THnSparseD* test_successesB = new THnSparseD("successesB", "successesB", 5, bins, min, max);
  THnSparseD* test_trialsB = new THnSparseD("trialsB", "trialsB", 5, bins, min, max);
  
  for (int i = 0; i < 100000; ++i) {  
    double rnum_x = gRandom->Uniform(-2.4, 2.4);
    double rnum_y = gRandom->Uniform(-2.4, 2.4);
    double rnum_z = gRandom->Uniform(-2.4, 2.4);
    double rnum_comb = gRandom->Uniform(-2.4, 2.4);
    double rnum_iso  = gRandom->Uniform(-2.4, 2.4);
    double tuple[5] = {rnum_x, rnum_y, rnum_z, rnum_comb, rnum_iso};
    test_trialsA->Fill(tuple);
    if (gRandom->Rndm() < 0.8) test_successesA->Fill(tuple);
    
    event_values_xA.push_back(rnum_x);
    event_values_yA.push_back(rnum_y);
    event_values_zA.push_back(rnum_z);
    event_values_combA.push_back(rnum_comb);
    event_values_isoA.push_back(rnum_iso);
  }

  for (int i = 0; i < 100000; ++i) {  
    double rnum_x = gRandom->Uniform(-2.4, 2.4);
    double rnum_y = gRandom->Uniform(-2.4, 2.4);
    double rnum_z = gRandom->Uniform(-2.4, 2.4);
    double rnum_comb = gRandom->Uniform(-2.4, 2.4);
    double rnum_iso  = gRandom->Uniform(-2.4, 2.4);
    double tuple[5] = {rnum_x, rnum_y, rnum_z, rnum_comb, rnum_iso};
    test_trialsB->Fill(tuple);
    if (gRandom->Rndm() < 0.9) test_successesB->Fill(tuple);
    
    event_values_xB.push_back(rnum_x);
    event_values_yB.push_back(rnum_y);
    event_values_zB.push_back(rnum_z);
    event_values_combB.push_back(rnum_comb);
    event_values_isoB.push_back(rnum_iso);
  }
  
  //Create Instance of APReweight
  APReweightND* weighter_A = new APReweightND(test_trialsA, test_successesA, true);
  APReweightND* weighter_B = new APReweightND(test_trialsB, test_successesB, true);
  
  APWeightSum* combined_sum = new APWeightSum;
  
  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values_xA.size(); i < I; i+=2) {
    APEvtWeight *weight_A = new APEvtWeight(APEvtWeight::kMuonMO);
    APEvtWeight *weight_B = new APEvtWeight(APEvtWeight::kElectronMO);
    
    double loop_val1[5] = {event_values_xA[i], event_values_yA[i], event_values_zA[i],event_values_combA[i],event_values_isoA[i]};
    weight_A->AddWeightToEvt(weighter_A->GetWeight(loop_val1));
    double loop_val2[5] = {event_values_xA[i+1], event_values_yA[i+1], event_values_zA[i+1],event_values_combA[i+1],event_values_isoA[i+1]};
    weight_A->AddWeightToEvt(weighter_A->GetWeight(loop_val2));
    double loop_val3[5] = {event_values_xB[i], event_values_yB[i], event_values_zB[i],event_values_combB[i],event_values_isoB[i]};
    weight_B->AddWeightToEvt(weighter_B->GetWeight(loop_val3));
    double loop_val4[5] = {event_values_xB[i+1], event_values_yB[i+1], event_values_zB[i+1],event_values_combB[i+1],event_values_isoB[i+1]};
    weight_B->AddWeightToEvt(weighter_B->GetWeight(loop_val4));

    APEvtWeight comb_weight = *weight_A && *weight_B;
    
    cout << comb_weight.GetWeight() << " +- " << comb_weight.GetVariance() << endl;
    
    combined_sum->AddEvt(&comb_weight);
    
    delete weight_A;
    delete weight_B;
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  cout << "Sum: " << combined_sum->GetSumW() << " +- " << combined_sum->GetStdDev() << endl;

  //Cleanup
  delete weighter_A;
  delete weighter_B;

  return 0;
}

int MCReweighting() {

  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with Gaussian distributions
  std::vector< double > event_values;
  TH1D* test_target = new TH1D("target", "target", 50, -5., 5.);
  TH1D* test_source = new TH1D("source", "source", 50, -5., 5.);
  for (int i = 0; i < 10000; ++i) {
    double rnum = gRandom->Gaus(0., 1.);
    test_source->Fill(rnum);
    event_values.push_back(rnum);
  }
  for (int i = 0; i < 10000; ++i) {
    test_target->Fill(gRandom->Gaus(0., 0.5));
  }

  //Create Instance of APReweight
  APReweight* weighter = new APReweight(test_source, test_target);

  //Create Instance of WeightHist histogram and default histogram
  APWeightHist* weighted_hist = new APWeightHist("weighted_hist", "weighted_hist", 50, -5., 5.);
  TH1D* weighted_TH1D = new TH1D("MC_weighted_TH1D", "weighted_TH1D", 50, -5., 5.);

  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values.size(); i < I; ++i) {
    weighted_hist->Fill(event_values[i], weighter->GetWeight(event_values[i]));
    weighted_TH1D->Fill(event_values[i], weighter->GetWeight(event_values[i])->GetExpectancy());
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  weighted_hist->GetYaxis()->SetRangeUser(0., 1.2 * weighted_hist->GetMaximum());

  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  weighted_TH1D->Draw("E");
  canvas->Print("MC_weighted_TH1D.eps", "eps");
  weighted_hist->Draw("E");
  canvas->Print("MC_weighted_hist.eps", "eps");

  //Compute full statistical uncertainties
  weighted_hist->ComputeGraph();

  //Draw Full histogram
  weighted_hist->GetGraphStatUncert()->Draw("APZ");
  canvas->Print("MC_weighted_hist_full.eps", "eps");

  //Draw comparison
  weighted_hist->SetLineColor(2);
  weighted_hist->SetLineStyle(2);
  weighted_hist->SetLineWidth(2);
  weighted_hist->Draw();
  weighted_hist->GetGraphStatUncert()->Draw("PZ");
  canvas->Print("MC_weighted_hist_comp.eps", "eps");

  //Cleanup
  delete canvas;
  delete weighted_hist;
  delete weighter;

  return 0;
}

int MCReweighting2D() {

  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with Gaussian distributions
  std::vector< double > event_values_x, event_values_y;
  TH2D* test_target = new TH2D("target2D", "target", 50, -5., 5., 50, -10., 10.);
  TH2D* test_source = new TH2D("source2D", "source", 50, -5., 5., 50, -10., 10.);
  for (int i = 0; i < 10000; ++i) {
    double rnum_x = gRandom->Gaus(0., 1.);
    double rnum_y = gRandom->Gaus(0., 2.);
    test_source->Fill(rnum_x, rnum_y);
    event_values_x.push_back(rnum_x);
    event_values_y.push_back(rnum_y);
  }
  for (int i = 0; i < 10000; ++i) {
    test_target->Fill(gRandom->Gaus(0., 0.5), gRandom->Gaus(0., 0.9));
  }
  //Create Instance of APReweight
  APReweight2D* weighter = new APReweight2D(test_source, test_target);

  //Create Instance default 2D histogram
  TH2D* weighted_TH2D = new TH2D("MC_weighted_TH2D", "weighted_TH2D", 50, -5., 5., 50, -10., 10.);

  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values_x.size(); i < I; ++i) {
    weighted_TH2D->Fill(event_values_x[i], event_values_y[i], weighter->GetWeight(event_values_x[i], event_values_y[i])->GetExpectancy());
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  weighted_TH2D->Draw("LEGO");
  canvas->Print("MC2D_weighted_TH2D.eps", "eps");
  test_target->Draw("LEGO");
  canvas->Print("MC2D_reference_TH2D.eps", "eps");

  //Cleanup
  delete canvas;
  delete weighter;

  return 0;
}

int MCReweighting3D() {

  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with Gaussian distributions
  std::vector< double > event_values_x, event_values_y, event_values_z;
  TH3D* test_target = new TH3D("target3D", "target", 20, -5., 5., 20, -10., 10., 20, -20., 20.);
  TH3D* test_source = new TH3D("source3D", "source", 20, -5., 5., 20, -10., 10., 20, -20., 20.);
  for (int i = 0; i < 100000; ++i) {
    double rnum_x = gRandom->Gaus(0., 1.);
    double rnum_y = gRandom->Gaus(0., 2.);
    double rnum_z = gRandom->Gaus(0., 4.);
    test_source->Fill(rnum_x, rnum_y, rnum_z);
    event_values_x.push_back(rnum_x);
    event_values_y.push_back(rnum_y);
    event_values_z.push_back(rnum_z);
  }
  for (int i = 0; i < 100000; ++i) {
    test_target->Fill(gRandom->Gaus(0., 0.3), gRandom->Gaus(0., 0.5), gRandom->Gaus(0., 1.5));
  }

  //Create Instance of APReweight
  APReweight3D* weighter = new APReweight3D(test_source, test_target);

  //Create Instance of default 3D histogram
  TH3D* weighted_TH3D = new TH3D("MC_weighted_TH3D", "weighted_TH3D", 20, -5., 5., 20, -10., 10., 20, -20., 20.);

  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values_x.size(); i < I; ++i) {
    weighted_TH3D->Fill(event_values_x[i], event_values_y[i], event_values_z[i], weighter->GetWeight(event_values_x[i], event_values_y[i], event_values_z[i])->GetExpectancy());
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  weighted_TH3D->Draw();
  canvas->Print("MC3D_weighted_TH3D.eps", "eps");
  test_target->Draw();
  canvas->Print("MC3D_reference_TH3D.eps", "eps");

  //Cleanup
  delete canvas;
  delete weighter;

  return 0;
}

int MCReweightingND() {

  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with Gaussian distributions
  std::vector< double > event_values_x, event_values_y, event_values_z, event_values_comb, event_values_iso;
  int bins[5] = {20, 20, 20, 2, 2};
  double min[5] = {-5., -10., -20., 0., 0.};
  double max[5] = {5., 10., 20., 2., 2.};

  THnSparseD* test_target = new THnSparseD("targetND", "target", 5, bins, min, max);
  THnSparseD* test_source = new THnSparseD("sourceND", "source", 5, bins, min, max);
  for (int i = 0; i < 1000000; ++i) {
    double rnum_x = gRandom->Gaus(0., 1.);
    double rnum_y = gRandom->Gaus(0., 2.);
    double rnum_z = gRandom->Gaus(0., 4.);
    double rnum_comb = gRandom->Rndm()*2;
    double rnum_iso  = gRandom->Rndm()*2;
    double tuple[5] = {rnum_x, rnum_y, rnum_z, rnum_comb, rnum_iso};
    test_source->Fill(tuple);
    event_values_x.push_back(rnum_x);
    event_values_y.push_back(rnum_y);
    event_values_z.push_back(rnum_z);
    event_values_comb.push_back(rnum_comb);
    event_values_iso.push_back(rnum_iso);
  }
  for (int i = 0; i < 1000000; ++i) {
    double evt_tuple[5] = {gRandom->Gaus(0., 0.3), gRandom->Gaus(0., 0.5), gRandom->Gaus(0., 1.5), gRandom->Rndm()*2, gRandom->Rndm()*2};
    test_target->Fill(evt_tuple);
  }

  //Create Instance of APReweight
  APReweightND* weighter = new APReweightND(test_source, test_target);

  //Create Instance of default 3D histogram
  TH3D* weighted_TH3D = new TH3D("MC_weighted_THND", "weighted_THND", 20, -5., 5., 20, -10., 10., 2, 0., 2.);

  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values_x.size(); i < I; ++i) {
    double loop_val[5] = {event_values_x[i], event_values_y[i], event_values_z[i],event_values_comb[i],event_values_iso[i]};
    weighted_TH3D->Fill(event_values_x[i], event_values_y[i], event_values_iso[i], weighter->GetWeight(loop_val)->GetExpectancy());
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  weighted_TH3D->Draw();
  canvas->Print("MCND_weighted_THND.eps", "eps");
  test_target->Projection(0,1,4)->Draw();
  canvas->Print("MCND_reference_THND.eps", "eps");

  //Cleanup
  delete canvas;
  delete weighter;

  return 0;
}

int TrigReweighting() {

  /////////////// Initialize ///////////////////////////////////////////////////
  //Create simple dummy histograms with trials/successes
  std::vector< double > event_values;
  TH1D* test_successes = new TH1D("successes", "successes", 50, -3., 3.);
  TH1D* test_trials = new TH1D("trials", "trials", 50, -3., 3.);
  for (int i = 0; i < 10000; ++i) {
    double rnum = gRandom->Uniform(-2.4, 2.4);
    test_trials->Fill(rnum);
    if (gRandom->Rndm() < 0.5) test_successes->Fill(rnum);
    event_values.push_back(rnum);
  }

  //Create Instance of APReweight
  APReweight* weighter = new APReweight(test_trials, test_successes, true);

  //Create Instance of WeightHist histogram and default histogram
  APWeightHist* weighted_hist = new APWeightHist("weighted_hist", "weighted_hist", 50, -3., 3.);
  TH1D* weighted_TH1D = new TH1D("Trig_weighted_TH1D", "weighted_TH1D", 50, -3., 3.);

  /////////////// Event Loop ///////////////////////////////////////////////////
  for (unsigned int i = 0, I = event_values.size(); i < I; ++i) {
    weighted_hist->Fill(event_values[i], weighter->GetWeight(event_values[i]));
    weighted_TH1D->Fill(event_values[i], weighter->GetWeight(event_values[i])->GetExpectancy());
  }

  /////////////// Finalize /////////////////////////////////////////////////////
  //Draw default Histograms
  weighted_hist->GetYaxis()->SetRangeUser(0., 1.2 * weighted_hist->GetMaximum());

  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
  weighted_TH1D->Draw("E");
  canvas->Print("Trig_weighted_TH1D.eps", "eps");
  weighted_hist->Draw("E");
  canvas->Print("Trig_weighted_hist.eps", "eps");

  //Compute full statistical uncertainties
  weighted_hist->ComputeGraph();

  //Draw Full histogram
  weighted_hist->GetGraphStatUncert()->Draw("APZ");
  canvas->Print("Trig_weighted_hist_full.eps", "eps");

  //Draw comparison
  weighted_hist->SetLineColor(2);
  weighted_hist->SetLineStyle(2);
  weighted_hist->SetLineWidth(2);
  weighted_hist->Draw();
  weighted_hist->GetGraphStatUncert()->Draw("PZ");
  canvas->Print("Trig_weighted_hist_comp.eps", "eps");

  delete canvas;
  delete weighted_hist;
  delete weighter;

  return 0;
}

int main(int argc, char **argv) {
  int retval = 0;

  CombineWeights();
  ErrorPropagation();
  //MCReweighting();
  //MCReweighting2D();
  //retval = system("date");
  //MCReweighting3D();
  //retval = system("date");
  //MCReweightingND();
  //retval = system("date; sleep 10");

  //TrigReweighting();

  return retval;
}
