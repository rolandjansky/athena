#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include "TStyle.h"
#include "TString.h"
#include "TLatex.h"
#include "TColor.h"
#include "TFile.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

typedef std::vector<double> vec;

void plotElectronEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotElectronTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotMuonEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotMuonTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotPhotonEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_plotPt);
void plotElectronEnergyResolution(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotFlavourTagEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt, char flavour, TString Tagger, int operating_point, bool track_confirmation);
void plotMETResolution(Upgrade::UpgradePerformanceFunctions *m_upgrade, float sumEtGeV, float METxGeV, float METyGeV);

void plotTrackJetConfirmEff(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_pt);
void plotJetTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade);

void plotPUjetPerformance(Upgrade::UpgradePerformanceFunctions *upgrade);
void plotLargeRjetSmearing(Upgrade::UpgradePerformanceFunctions *upgrade);
void plotJER(Upgrade::UpgradePerformanceFunctions *upgrade, double CtermPenalty = 0.00);

using namespace Upgrade;

int main() {
  // This hack is needed to force dictionary loading in ROOT 5
  gROOT->ProcessLine("#include <vector>");

  // Style choices for axis on plots
  Int_t font = 42; // Helvetica
  Double_t tsize = 0.05;
  gStyle->SetTextFont(font);
  gStyle->SetTextSize(tsize);
  for (TString ax : {"x", "y", "z"}) {
    gStyle->SetLabelFont(font, ax); gStyle->SetTitleFont(font, ax);
    gStyle->SetLabelSize(tsize, ax); gStyle->SetTitleSize(tsize, ax);
  }
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  UpgradePerformanceFunctions *m_upgrade = new UpgradePerformanceFunctions("UpgradePerformanceFunctions");
  std::cout << "Layout is " << m_upgrade->getLayout() << ", and mu value is " << m_upgrade->getAvgMu() << std::endl;

  plotElectronEfficiency(m_upgrade, 0.5, 32.0);

  plotPUjetPerformance(m_upgrade);
  plotLargeRjetSmearing(m_upgrade);
  plotJER(m_upgrade, 0.00);
  //  plotElectronTriggerEfficiency(m_upgrade,0.5,32.0);
  //  plotElectronEnergyResolution(m_upgrade,3.3,100.0);
  //  plotMuonEfficiency(m_upgrade,2,80.0);
  //  plotMuonTriggerEfficiency(m_upgrade,0.61,45.0);
  //  plotPhotonEfficiency(m_upgrade,1.0,80.0);
  //  plotFlavourTagEfficiency(m_upgrade,1.5,45,'B',"mv1",70,true);
  //  plotMETResolution(m_upgrade, 100,200,0);
  //  plotTrackJetConfirmEff(m_upgrade,35);
  //  plotJetTriggerEfficiency(m_upgrade);

  return 0;
}


void plotElectronEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Efficiency Plots" << std::endl;

  TH2D *gold_ElectronEfficiency = new TH2D("gold_ElectronEfficiency", "gold_ElectronEfficiency", 1000, 0, 120, 1000, 0, 5);
  // TH2D *silver_ElectronEfficiency = new TH2D("silverElectronEfficiency","silver_ElectronEfficiency",1000,0,120,1000,0,5);
  // TH2D *bronze_ElectronEfficiency = new TH2D("bronze_ElectronEfficiency","bronze_ElectronEfficiency",1000,0,120,1000,0,5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getElectronEfficiency(pt_f * 1000, eta_f);
      gold_ElectronEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      // result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
      // silver_ElectronEfficiency->SetBinContent(i,j,result_f);
      // if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
      //   silver_pt.push_back(result_f);
      // }

      // result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
      // bronze_ElectronEfficiency->SetBinContent(i,j,result_f);
      // if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
      //   bronze_pt.push_back(result_f);
      // }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getElectronEfficiency(pt_f * 1000, eta_f);
        gold_eta.push_back(result_f);

        // result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
        // silver_eta.push_back(result_f);

        // result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
        // bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0 / 1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  // TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  // silver_graph_eta->SetLineColor(4);
  // silver_graph_eta->SetMarkerColor(4);
  // silver_graph_eta->GetXaxis()->SetTitle("Eta");
  // TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  // bronze_graph_eta->SetLineColor(1);
  // bronze_graph_eta->SetMarkerColor(1);
  // bronze_graph_eta->GetXaxis()->SetTitle("Eta");

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  // TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  // silver_graph_pt->SetLineColor(4);
  // silver_graph_pt->SetMarkerColor(4);
  // TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  // bronze_graph_pt->SetLineColor(1);
  // bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D
  gStyle->SetNumberContours(100);
  TCanvas *can = new TCanvas("can", "Electron Efficiency", 1000, 1000);
  can->Divide(2, 2);

  can->cd(1);
  gold_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_ElectronEfficiency->GetYaxis()->SetTitle("Eta");
  gold_ElectronEfficiency->SetTitle("Tight Gold ElectronEfficiency");
  gold_ElectronEfficiency->SetTitle("Tight Gold ElectronEfficiency");
  gold_ElectronEfficiency->SetStats(0);
  gold_ElectronEfficiency->Draw("colz");

  // can->cd(2);
  // silver_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  // silver_ElectronEfficiency->GetYaxis()->SetTitle("Eta");
  // silver_ElectronEfficiency->SetTitle("Tight Silver ElectronEfficiency");
  // silver_ElectronEfficiency->SetStats(0);
  // silver_ElectronEfficiency->Draw("colz");

  // can->cd(3);
  // bronze_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  // bronze_ElectronEfficiency->GetYaxis()->SetTitle("Eta");
  // bronze_ElectronEfficiency->SetTitle("Tight Bronze ElectronEfficiency");
  // bronze_ElectronEfficiency->SetStats(0);
  // bronze_ElectronEfficiency->Draw("colz");

  can->cd(2);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Electron Efficiency in eta for pt = "
                   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Electron Efficiency");
  gold_graph_eta->Draw("ALP");
  // silver_graph_eta->Draw("LP");
  // bronze_graph_eta->Draw("LP");

  can->cd(3);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Electron Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Electron Efficiency");
  gold_graph_pt->Draw("ALP");
  // silver_graph_pt->Draw("LP");
  // bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("ElectronEfficiency.pdf");
  can->Close();

  return;
}

void plotElectronTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Trigger Efficiency Plots" << std::endl;


  TH2D *gold_ElectronTriggerEfficiency = new TH2D("gold_ElectronTriggerEfficiency", "gold_ElectronTriggerEfficiency", 1000, 0, 120, 1000, -5, 5);
  // TH2D *silver_ElectronTriggerEfficiency = new TH2D("silverElectronTriggerEfficiency","silver_ElectronTriggerEfficiency",1000,0,120,1000,-5,5);
  // TH2D *bronze_ElectronTriggerEfficiency = new TH2D("bronze_ElectronTriggerEfficiency","bronze_ElectronTriggerEfficiency",1000,0,120,1000,-5,5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = -5.;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f * 1000, eta_f);
      gold_ElectronTriggerEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      // result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
      // silver_ElectronTriggerEfficiency->SetBinContent(i,j,result_f);
      // if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
      //   silver_pt.push_back(result_f);
      // }

      // result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
      // bronze_ElectronTriggerEfficiency->SetBinContent(i,j,result_f);
      // if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
      //   bronze_pt.push_back(result_f);
      // }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f * 1000, eta_f);
        gold_eta.push_back(result_f);

        // result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
        // silver_eta.push_back(result_f);

        // result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
        // bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 10.0 / 1000.0; //iterate eta from -5 to 5 in .01 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  // TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  // silver_graph_eta->SetLineColor(4);
  // silver_graph_eta->SetMarkerColor(4);
  // TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  // bronze_graph_eta->SetLineColor(1);
  // bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);

  // TGraph *gold_graph_pt2 = new TGraph(pt.size(),&pt[800],&gold_pt[800]);
  // gold_graph_pt2->SetLineColor(2);
  // gold_graph_pt2->SetMarkerColor(2);


// TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  // silver_graph_pt->SetLineColor(4);
  // silver_graph_pt->SetMarkerColor(4);
  // TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  // bronze_graph_pt->SetLineColor(1);
  // bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can", "ElectronTrigger Efficiency", 1000, 1000);
  can->Divide(2, 2);

  can->cd(1);
  gold_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  gold_ElectronTriggerEfficiency->SetTitle("Tight Gold ElectronTriggerEfficiency");
  gold_ElectronTriggerEfficiency->SetStats(0);
  gold_ElectronTriggerEfficiency->Draw("colz");

  // can->cd(2);
  // silver_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  // silver_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  // silver_ElectronTriggerEfficiency->SetTitle("Tight Silver ElectronTriggerEfficiency");
  // silver_ElectronTriggerEfficiency->SetStats(0);
  // silver_ElectronTriggerEfficiency->Draw("colz");

  // can->cd(3);
  // bronze_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  // bronze_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  // bronze_ElectronTriggerEfficiency->SetTitle("Tight Bronze ElectronTriggerEfficiency");
  // bronze_ElectronTriggerEfficiency->SetStats(0);
  // bronze_ElectronTriggerEfficiency->Draw("colz");

  can->cd(2);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Electron Trigger Efficiency in eta for pt = "
                   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Electron Trigger Efficiency");
  gold_graph_eta->Draw("ALP");
  // silver_graph_eta->Draw("LP");
  // bronze_graph_eta->Draw("LP");

  can->cd(3);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Electron Trigger Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Electron Trigger Efficiency");
  gold_graph_pt->Draw("ALP");
  // silver_graph_pt->Draw("LP");
  // bronze_graph_pt->Draw("LP");

  // plot_eta = "3.0";
  // can->cd(4);
  // std::stringstream pt_stringstream;
  // pt_stringstream << "Sweep of Electron Trigger Efficiency in pt for eta = " << plot_eta;
  // std::string pt_string(pt_stringstream.str());
  // const char *pt_title = pt_string.c_str();
  // gold_graph_pt2->SetTitle(pt_title);
  // gold_graph_pt2->GetXaxis()->SetTitle("Pt (GeV)");
  // gold_graph_pt2->GetYaxis()->SetTitle("Electron Trigger Efficiency");
  // gold_graph_pt2->Draw("ALP");

  can->Update();
  can->Print("ElectronTriggerEfficiency.pdf");
  can->Close();

  return;
}


void plotJetTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade) {

  std::cout << "Generating Jet Trigger Efficiency Plots" << std::endl;

  TH1F *h_1jtrig_pt = new TH1F("gold_Jet1jTriggerEfficiencyPt", "gold_Jet1jTriggerEfficiencyPt; p_{T}^{LeadJet} [GeV]", 200, 0., 200.); h_1jtrig_pt->Sumw2();
  TH1F *h_1jtrig_eta = new TH1F("gold_Jet1jTriggerEfficiencyEta", "gold_Jet1jTriggerEfficiencyEta; |#eta^{LeadJet}|", 100, 0., 5.); h_1jtrig_eta->Sumw2();
  TH1F *h_3jtrig_pt = new TH1F("gold_Jet3jTriggerEfficiencyPt", "gold_Jet3jTriggerEfficiencyPt; p_{T}^{ThirdJet} [GeV]", 200, 0., 200.); h_3jtrig_pt->Sumw2();
  TH1F *h_3jtrig_eta = new TH1F("gold_Jet3jTriggerEfficiencyEta", "gold_Jet3jTriggerEfficiencyEta; |#eta^{ThirdJet}|", 100, 0., 5.); h_3jtrig_eta->Sumw2();
  TH1F *h_4jtrig_pt = new TH1F("gold_Jet4jTriggerEfficiencyPt", "gold_Jet4jTriggerEfficiencyPt; p_{T}^{FourthJet} [GeV]", 200, 0., 200.); h_4jtrig_pt->Sumw2();
  TH1F *h_4jtrig_eta = new TH1F("gold_Jet4jTriggerEfficiencyEta", "gold_Jet4jTriggerEfficiencyEta; |#eta^{FourthJet}|", 100, 0.0, 5.); h_4jtrig_eta->Sumw2();

  for (int j = 0; j < 100; j++) { //Begin eta loop
    h_1jtrig_eta->Fill(j * 0.05 - 0.001, m_upgrade->getSingleJetTriggerEfficiency(200000., j * 0.05));
    h_3jtrig_eta->Fill(j * 0.05 - 0.001, m_upgrade->getThreeJetTriggerEfficiency(200000., 0., 200000., 0., 200000., j * 0.05));
    h_4jtrig_eta->Fill(j * 0.05 - 0.001, m_upgrade->getFourJetTriggerEfficiency(200000., 0., 200000., 0., 200000., 0., 200000., j * 0.05));
  }
  for (int j = 0; j < 200; j++) { //Begin pt loop
    h_1jtrig_pt->Fill(j * 1., m_upgrade->getSingleJetTriggerEfficiency(j * 1000., 0.));
    h_3jtrig_pt->Fill(j * 1., m_upgrade->getThreeJetTriggerEfficiency(200000., 0., 200000., 0., j * 1000., 0.));
    h_4jtrig_pt->Fill(j * 1., m_upgrade->getFourJetTriggerEfficiency(200000., 0., 200000., 0., 200000., 0., j * 1000., 0.));
  }

  TCanvas *can1 = new TCanvas("can1", "Jet Trigger pT Efficiency", 1000, 1000);

  h_1jtrig_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  h_1jtrig_pt->GetYaxis()->SetTitle("Jet Trigger p_{T} Efficiency");

  h_1jtrig_pt->SetTitle("Jet Trigger p_{T} Efficiency");

  h_1jtrig_pt->SetMarkerColor(1);
  h_1jtrig_pt->SetLineColor(1);
  h_3jtrig_pt->SetMarkerColor(2);
  h_4jtrig_pt->SetMarkerColor(4);
  h_3jtrig_pt->SetLineColor(2);
  h_4jtrig_pt->SetLineColor(4);

  h_1jtrig_pt->SetMaximum(1.3);
  h_1jtrig_eta->SetMaximum(1.3);

  h_1jtrig_pt->Draw("hist");
  h_3jtrig_pt->Draw("hist same");
  h_4jtrig_pt->Draw("hist same");

  can1->Print("JetTriggerPtEfficiency.pdf");

  TCanvas *can2 = new TCanvas("can2", "Jet Trigger eta Efficiency", 1000, 1000);

  h_1jtrig_eta->GetXaxis()->SetTitle("|#eta|");
  h_1jtrig_eta->GetYaxis()->SetTitle("Jet Trigger |#eta| Efficiency");

  h_1jtrig_eta->SetTitle("Jet Trigger |#eta| Efficiency");

  h_1jtrig_eta->SetMarkerColor(1);
  h_1jtrig_eta->SetLineColor(1);
  h_3jtrig_eta->SetMarkerColor(2);
  h_4jtrig_eta->SetMarkerColor(4);
  h_3jtrig_eta->SetLineColor(2);
  h_4jtrig_eta->SetLineColor(4);

  h_1jtrig_eta->Draw("hist");
  h_3jtrig_eta->Draw("hist same");
  h_4jtrig_eta->Draw("hist same");

  can2->Print("JetTriggerEtaEfficiency.pdf");



}


void plotMuonEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight muon efficiency for pt and eta
  std::cout << "Generating Muon Efficiency Plots" << std::endl;

  TH2D *gold_MuonEfficiency = new TH2D("gold_MuonEfficiency", "gold_MuonEfficiency", 1000, 0, 120, 1000, 0, 5);
  TH2D *silver_MuonEfficiency = new TH2D("silverMuonEfficiency", "silver_MuonEfficiency", 1000, 0, 120, 1000, 0, 5);
  TH2D *bronze_MuonEfficiency = new TH2D("bronze_MuonEfficiency", "bronze_MuonEfficiency", 1000, 0, 120, 1000, 0, 5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
      gold_MuonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
      silver_MuonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }

      result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
      bronze_MuonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
        gold_eta.push_back(result_f);

        result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
        silver_eta.push_back(result_f);

        result_f = m_upgrade->getMuonEfficiency(pt_f * 1000, eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0 / 1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *silver_graph_eta = new TGraph(eta.size(), &eta[0], &silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  silver_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *bronze_graph_eta = new TGraph(eta.size(), &eta[0], &bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);
  bronze_graph_eta->GetXaxis()->SetTitle("Eta");

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(), &pt[0], &silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(), &pt[0], &bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D
  gStyle->SetNumberContours(100);
  TCanvas *can = new TCanvas("can", "Muon Efficiency", 1000, 1000);
  can->Divide(2, 3);

  can->cd(1);
  gold_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_MuonEfficiency->GetYaxis()->SetTitle("Eta");
  gold_MuonEfficiency->SetTitle("Tight Gold MuonEfficiency");
  gold_MuonEfficiency->SetTitle("Tight Gold MuonEfficiency");
  gold_MuonEfficiency->SetStats(0);
  gold_MuonEfficiency->Draw("colz");

  can->cd(2);
  silver_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  silver_MuonEfficiency->GetYaxis()->SetTitle("Eta");
  silver_MuonEfficiency->SetTitle("Tight Silver MuonEfficiency");
  silver_MuonEfficiency->SetStats(0);
  silver_MuonEfficiency->Draw("colz");

  can->cd(3);
  bronze_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_MuonEfficiency->GetYaxis()->SetTitle("Eta");
  bronze_MuonEfficiency->SetTitle("Tight Bronze MuonEfficiency");
  bronze_MuonEfficiency->SetStats(0);
  bronze_MuonEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Muon Efficiency in eta for pt = "
                   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Muon Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Muon Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Muon Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("MuonEfficiency.pdf");
  can->Close();

  return;
}


void plotMuonTriggerEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight muon efficiency for pt and eta
  std::cout << "Generating Muon Trigger Efficiency Plots" << std::endl;

  TH2D *gold_MuonTriggerEfficiency = new TH2D("gold_MuonTriggerEfficiency", "gold_MuonTriggerEfficiency", 1000, 0, 120, 1000, -3, 3);
  TH2D *silver_MuonTriggerEfficiency = new TH2D("silverMuonTriggerEfficiency", "silver_MuonTriggerEfficiency", 1000, 0, 120, 1000, -3, 3);
  TH2D *bronze_MuonTriggerEfficiency = new TH2D("bronze_MuonTriggerEfficiency", "bronze_MuonTriggerEfficiency", 1000, 0, 120, 1000, -3, 3);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = -3.;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
      gold_MuonTriggerEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
      silver_MuonTriggerEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }

      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
      bronze_MuonTriggerEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
        gold_eta.push_back(result_f);

        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
        silver_eta.push_back(result_f);

        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f * 1000, eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 6.0 / 1000.0; //iterate eta from -3 to 3 in .006 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(), &eta[0], &silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(), &eta[0], &bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(), &pt[0], &silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(), &pt[0], &bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can", "MuonTrigger Efficiency", 1000, 1000);
  can->Divide(2, 3);

  can->cd(1);
  gold_MuonTriggerEfficiency->SetTitle("Tight Gold MuonTriggerEfficiency");
  gold_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  gold_MuonTriggerEfficiency->SetStats(0);
  gold_MuonTriggerEfficiency->Draw("colz");

  can->cd(2);
  silver_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  silver_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  silver_MuonTriggerEfficiency->SetTitle("Tight Silver MuonTriggerEfficiency");
  silver_MuonTriggerEfficiency->SetStats(0);
  silver_MuonTriggerEfficiency->Draw("colz");

  can->cd(3);
  bronze_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  bronze_MuonTriggerEfficiency->SetTitle("Tight Bronze MuonTriggerEfficiency");
  bronze_MuonTriggerEfficiency->SetStats(0);
  bronze_MuonTriggerEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Muon Trigger Efficiency in eta for pt = "
                   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Muon Trigger Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Muon Trigger Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Muon Trigger Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("MuonTriggerEfficiency.pdf");
  can->Close();

  return;
}

void plotPhotonEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight photon efficiency for pt and eta
  std::cout << "Generating photon Efficiency Plots" << std::endl;

  TH2D *gold_PhotonEfficiency = new TH2D("gold_PhotonEfficiency", "gold_PhotonEfficiency", 1000, 0, 120, 1000, 0, 5);
  TH2D *silver_PhotonEfficiency = new TH2D("silverPhotonEfficiency", "silver_PhotonEfficiency", 1000, 0, 120, 1000, 0, 5);
  TH2D *bronze_PhotonEfficiency = new TH2D("bronze_PhotonEfficiency", "bronze_PhotonEfficiency", 1000, 0, 120, 1000, 0, 5);
  //TH2D *gold_PhotonFakeRate = new TH2D("gold_PhotonFakeRate","gold_PhotonFakeRate",1000,0,120,1000,0,5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
      gold_PhotonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
      silver_PhotonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }

      result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
      bronze_PhotonEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
        gold_eta.push_back(result_f);

        result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
        silver_eta.push_back(result_f);

        result_f = m_upgrade->getPhotonEfficiency(pt_f * 1000);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0 / 1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *silver_graph_eta = new TGraph(eta.size(), &eta[0], &silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  silver_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *bronze_graph_eta = new TGraph(eta.size(), &eta[0], &bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);
  bronze_graph_eta->GetXaxis()->SetTitle("Eta");

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(), &pt[0], &silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(), &pt[0], &bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D
  gStyle->SetNumberContours(100);
  TCanvas *can = new TCanvas("can", "Photon Efficiency", 1000, 1000);
  can->Divide(2, 3);

  can->cd(1);
  gold_PhotonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_PhotonEfficiency->GetYaxis()->SetTitle("Eta");
  gold_PhotonEfficiency->SetTitle("Tight Gold PhotonEfficiency");
  gold_PhotonEfficiency->SetTitle("Tight Gold PhotonEfficiency");
  gold_PhotonEfficiency->SetStats(0);
  gold_PhotonEfficiency->Draw("colz");

  can->cd(2);
  silver_PhotonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  silver_PhotonEfficiency->GetYaxis()->SetTitle("Eta");
  silver_PhotonEfficiency->SetTitle("Tight Silver PhotonEfficiency");
  silver_PhotonEfficiency->SetStats(0);
  silver_PhotonEfficiency->Draw("colz");

  can->cd(3);
  bronze_PhotonEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_PhotonEfficiency->GetYaxis()->SetTitle("Eta");
  bronze_PhotonEfficiency->SetTitle("Tight Bronze PhotonEfficiency");
  bronze_PhotonEfficiency->SetStats(0);
  bronze_PhotonEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Photon Efficiency in eta for pt = "
                   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Photon Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Photon Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Photon Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("PhotonEfficiency.pdf");
  can->Close();

  return;
}

void plotElectronEnergyResolution(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Energy Resolution Plots" << std::endl;


  TH2D *gold_ElectronEnergyResolution = new TH2D("gold_ElectronEnergyResolution", "gold_ElectronEnergyResolution", 1000, 0, 120, 1000, 0, 5);
  TH2D *silver_ElectronEnergyResolution = new TH2D("silverElectronEnergyResolution", "silver_ElectronEnergyResolution", 1000, 0, 120, 1000, 0, 5);
  TH2D *bronze_ElectronEnergyResolution = new TH2D("bronze_ElectronEnergyResolution", "bronze_ElectronEnergyResolution", 1000, 0, 120, 1000, 0, 5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan

  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
      gold_ElectronEnergyResolution->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
      silver_ElectronEnergyResolution->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }

      result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
      bronze_ElectronEnergyResolution->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
        gold_eta.push_back(result_f);

        result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
        silver_eta.push_back(result_f);

        result_f = m_upgrade->getElectronEnergyResolution(pt_f * 1000, eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0 / 1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(), &eta[0], &silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(), &eta[0], &bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(), &pt[0], &silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(), &pt[0], &bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can", "Electron EnergyResolution", 1000, 1000);
  can->Divide(2, 3);

  can->cd(1);
  gold_ElectronEnergyResolution->SetTitle("Tight Gold Electron Energy Resolution");
  gold_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)");
  gold_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta");
  gold_ElectronEnergyResolution->SetStats(0);
  gold_ElectronEnergyResolution->Draw("colz");

  can->cd(2);
  silver_ElectronEnergyResolution->SetTitle("Tight Silver Electron Energy Resolution");
  silver_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)");
  silver_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta");
  silver_ElectronEnergyResolution->SetStats(0);
  silver_ElectronEnergyResolution->Draw("colz");

  can->cd(3);
  bronze_ElectronEnergyResolution->SetTitle("Tight Bronze Electron Energy Resolution");
  bronze_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta");
  bronze_ElectronEnergyResolution->SetStats(0);
  bronze_ElectronEnergyResolution->Draw("colz");

  can->cd(4);
  std::string eta_string = "Sweep of Electron Energy Resolution in eta for pt = " + std::to_string(plot_pt) + "GeV";
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Abolute Electron Energy Resolution");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::string pt_string = "Sweep of Electron Energy Resolution in pt for eta = " + std::to_string(plot_eta);
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Abolute Electron Energy Resolution");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("ElectronEnergyResolution.pdf");
  can->Close();

  return;
}

void plotFlavourTagEfficiency(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt, char flavour, TString Tagger, int operating_point, bool track_confirmation) {
  // Make plot of choosen Flavour tagging efficiency for pt and eta
  std::cout << "Generating " << flavour << "Tagging Efficiency plots" << std::endl;

  //Choose file containing histograms for Flavour tagging

  TH2D *gold_FlavourTagEfficiency = new TH2D("gold_FlavourTagEfficiency", "gold_FlavourTagEfficiency", 1000, 0, 120, 1000, 0, 5);

  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> gold_pt; //Store gold pt scan

  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for (int j = 0; j < 1000; j++) { //Begin eta loop

    for (int i = 0; i < 1000; i++) { //Begin pt loop
      result_f = m_upgrade->getFlavourTagEfficiency(pt_f * 1000, eta_f, flavour, Tagger, operating_point, track_confirmation);
      gold_FlavourTagEfficiency->SetBinContent(i, j, result_f);
      if (eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      if (pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)) {
        plot_pt = pt_f;
        eta.push_back(eta_f);

        result_f = m_upgrade->getFlavourTagEfficiency(pt_f * 1000, eta_f, flavour, Tagger, operating_point, track_confirmation);
        gold_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0 / 1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0 / 1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(), &eta[0], &gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(), &pt[0], &gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can", "Flavour Tagging Efficiency", 1000, 1000);
  can->Divide(2, 2);

  std::string flavour_string(1, flavour);

  can->cd(1);
  std::string gold_histo_name = "Gold " + flavour_string + "-Tagging Efficiency";
  gold_FlavourTagEfficiency->SetTitle(gold_histo_name.c_str());
  gold_FlavourTagEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_FlavourTagEfficiency->GetYaxis()->SetTitle("Eta");
  gold_FlavourTagEfficiency->SetStats(0);
  gold_FlavourTagEfficiency->Draw("colz");

  can->cd(3);
  std::string eta_string = "Sweep of " + flavour_string + "-Tagging efficiency in eta for pt = " + std::to_string(plot_pt) + "GeV";
  gold_graph_eta->SetTitle(eta_string.c_str());
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Flavour Tagging Efficiency");
  gold_graph_eta->Draw("ALP");

  can->cd(4);
  std::string pt_string = "Sweep of " + flavour_string + "-Tagging efficiency in pt for eta = " + std::to_string(plot_eta);
  gold_graph_pt->SetTitle(pt_string.c_str());
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Flavour Tagging Efficiency");
  gold_graph_pt->Draw("ALP");

  std::string filename = flavour_string + "-TagEfficiency.pdf";

  can->Update();
  can->Print(filename.c_str());
  can->Close();

  return;
}


void plotMETResolution(Upgrade::UpgradePerformanceFunctions *m_upgrade, float sumEtGeV, float METxGeV, float METyGeV) {
  // Make plot of Missing Transverse Energy performance
  std::cout << "Generating Missing Transverse Energy performance plots" << std::endl;

  //Choose file containing histograms for Missing Transverse Energy
  //m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");

  // Set random number generator seed for MET smearing function

  // Set average pileup for analysis (only mu = 200 exists currently)
  std::cout << "Resetting average pileup value to 200" << std::endl;

  // Storage container for MET result defined by UpgradePerformaceFunctions
  UpgradePerformanceFunctions::MET met_result; // Definition: typedef std::pair<double,double> MET
  met_result.first = 0; // initialize METxMeV to 0
  met_result.second = 0; // initialize METyMeV to 0

  TH1D *SmearedMETx = new TH1D("SmearedMETx", "SmearedMETx", 100, -200, 200);
  TH1D *SmearedMETy = new TH1D("SmearedMETy", "SmearedMETy", 100, -200, 200);
  TH1D *SmearedMETphi = new TH1D("SmearedMETphi", "SmearedMETphiSmeared", 100, -4, 4);

  std::vector<float> gold_resol; //Store gold MET resolution
  std::vector<float> silver_resol; //Store silver MET resolution
  std::vector<float> bronze_resol; //Store bronze MET resolution
  std::vector<float> sumEt; // Store sum Et for plotting

  float result_f = 0;
  float temp_sumEt = 0;
  float temp_METx = 0;
  float temp_METy = 0;
  float temp_phi = 0;
  float truth_phi = atan2(METyGeV, METxGeV);

  for (int i = 0; i < 1000000; i++) { //Create 100000 smeared events
    met_result = m_upgrade->getMETSmeared(sumEtGeV * 1000, METxGeV * 1000, METyGeV * 1000);

    temp_METx = met_result.first / 1000; // Smeared METx in GeV
    temp_METy = met_result.second / 1000; // Smeared METy in GeV
    temp_phi = atan2(temp_METy, temp_METx);

    SmearedMETx->Fill(temp_METx - METxGeV); // Center Distribution at 0
    SmearedMETy->Fill(temp_METy - METyGeV); // Center distribution at 0

    if ((temp_phi - truth_phi) <= -M_PI) {
      temp_phi = temp_phi + 2 * M_PI - truth_phi;
    }
    else {
      temp_phi = temp_phi - truth_phi;
    }
    SmearedMETphi->Fill(temp_phi);

  } //End METSmeared loop

  for (int i = 0; i < 70000; i++) { // Generate points for MET Resolution plot
    temp_sumEt = i * 100;
    sumEt.push_back(temp_sumEt / 1000);
    result_f = m_upgrade->getMETResolution(temp_sumEt);
    gold_resol.push_back(result_f / 1000.0);

    result_f = m_upgrade->getMETResolution(temp_sumEt);
    silver_resol.push_back(result_f / 1000.0);

    result_f = m_upgrade->getMETResolution(temp_sumEt);
    bronze_resol.push_back(result_f / 1000.0);
  } //End METResolution loop

  // Create sumEt scan TGraph objects and edit for plotting
  TGraph *bronze_graph_resol = new TGraph(sumEt.size(), &sumEt[0], &bronze_resol[0]);
  bronze_graph_resol->SetLineColor(1);
  bronze_graph_resol->SetMarkerColor(1);
  TGraph *silver_graph_resol = new TGraph(sumEt.size(), &sumEt[0], &silver_resol[0]);
  silver_graph_resol->SetLineColor(4);
  silver_graph_resol->SetMarkerColor(4);
  TGraph *gold_graph_resol = new TGraph(sumEt.size(), &sumEt[0], &gold_resol[0]);
  gold_graph_resol->SetLineColor(2);
  gold_graph_resol->SetMarkerColor(2);

  TCanvas *can = new TCanvas("can", "Missing Transverse Energy Performance", 1000, 1000);
  can->Divide(2, 2);

  can->cd(1);
  SmearedMETx->SetTitle("Smeared MET on X-axis");
  SmearedMETx->GetXaxis()->SetTitle("[Smeared METx - True METx] (GeV)");
  SmearedMETx->SetStats(0);
  SmearedMETx->Draw();

  can->cd(2);
  SmearedMETy->SetTitle("Smeared MET on Y-axis");
  SmearedMETy->GetXaxis()->SetTitle("[Smeared METy - True METy] (GeV)");
  SmearedMETy->SetStats(0);
  SmearedMETy->Draw();

  can->cd(3);
  SmearedMETphi->SetTitle("Smeared MET in phi");
  SmearedMETphi->GetXaxis()->SetTitle("[Smeared Phi - True Phi] (radians)");
  SmearedMETphi->SetStats(0);
  SmearedMETphi->Draw();

  can->cd(4);
  bronze_graph_resol->SetTitle("MET resolution vs raw sumEt");
  bronze_graph_resol->GetXaxis()->SetTitle("raw sumEt (GeV)");
  bronze_graph_resol->GetYaxis()->SetTitle("Absolute MET resolution (GeV)");
  bronze_graph_resol->Draw("ALP");
  silver_graph_resol->Draw("LP");
  gold_graph_resol->Draw("LP");

  can->Update();
  can->Print("METPerformance.pdf");
  can->Close();

  return;
}


void plotTrackJetConfirmEff(Upgrade::UpgradePerformanceFunctions *m_upgrade, float plot_pt) {
  // Make plot of Missing Transverse Energy performance
  std::cout << "Generating Track-Jet Confirmation Efficiency plots" << std::endl;


  //Initialize jet parameters

  // Set pileup (options currently incude 140 and 200

  TCanvas *can = new TCanvas("can", "Track-jet confirmation efficiency", 1000, 1000);
  //can->Divide(2,2);

  // Vectors for storing Track Jet Confirmation Efficiency for gold, silver, bronze
  std::vector<float> gold_HS;
  std::vector<float> gold_PU;
  std::vector<float> silver_HS;
  std::vector<float> silver_PU;
  std::vector<float> bronze_HS;
  std::vector<float> bronze_PU;

  std::vector<float> eta;

  float temp_eta = 0;
  float store_result = 0;

  for (int j = 0; j < 2; j++) { // First loop Scheme = HS, Second loop Scheme = PU
    if (j == 0) { // First loop Scheme = HS
      std::cout << "First Track-jet confirmation loop Scheme = HS and PileupEff = .9" << std::endl;
    }
    else { // Second loop Scheme = PU
      std::cout << "Second Track-jet confirmation loop Scheme = PU and PileupEff = .02" << std::endl;
    }

    // **** Gold Loop **** //
    temp_eta = 0; // Start eta at 0
    for (int i = 0; i < 1000; i++) { // Gold loop to fill vectors
      eta.push_back(temp_eta);

      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "HS");
      gold_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "PU");
      gold_PU.push_back(store_result);

      temp_eta = temp_eta + 5.0 / 1000.0;
    } // End Gold loop to fill vectors

    // **** Silver Loop **** //
    temp_eta = 0; // Start eta at 0
    for (int i = 0; i < 1000; i++) { // Silver loop to fill vectors
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "HS");
      silver_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "PU");
      silver_PU.push_back(store_result);

      temp_eta = temp_eta + 5.0 / 1000.0;
    } // End Silver loop to fill vectors

    // **** Bronze Loop **** //
    temp_eta = 0; // Start eta at 0
    for (int i = 0; i < 1000; i++) { // Bronze loop to fill vectors
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "HS");
      bronze_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt * 1000, temp_eta, "PU");
      bronze_PU.push_back(store_result);

      temp_eta = temp_eta + 5.0 / 1000.0;
    } // End Bronze loop to fill vectors


    if (j == 0) { // Generate and draw plots for Scheme = HS and PileupEff = .9

      // Create Track Jet Confirmation Efficiency for jets from Hard Scatter
      TGraph *bronze_graph1_HS = new TGraph(eta.size(), &eta[0], &bronze_HS[0]);
      bronze_graph1_HS->SetLineColor(1);
      bronze_graph1_HS->SetMarkerColor(1);
      TGraph *silver_graph1_HS = new TGraph(eta.size(), &eta[0], &silver_HS[0]);
      silver_graph1_HS->SetLineColor(4);
      silver_graph1_HS->SetMarkerColor(4);
      TGraph *gold_graph1_HS = new TGraph(eta.size(), &eta[0], &gold_HS[0]);
      gold_graph1_HS->SetLineColor(2);
      gold_graph1_HS->SetMarkerColor(2);

      // Create Track Jet Confirmation Efficiency for jets from Pileup
      TGraph *bronze_graph1_PU = new TGraph(eta.size(), &eta[0], &bronze_PU[0]);
      bronze_graph1_PU->SetLineColor(1);
      bronze_graph1_PU->SetMarkerColor(1);
      TGraph *silver_graph1_PU = new TGraph(eta.size(), &eta[0], &silver_PU[0]);
      silver_graph1_PU->SetLineColor(4);
      silver_graph1_PU->SetMarkerColor(4);
      TGraph *gold_graph1_PU = new TGraph(eta.size(), &eta[0], &gold_PU[0]);
      gold_graph1_PU->SetLineColor(2);
      gold_graph1_PU->SetMarkerColor(2);

      // can->cd(1);
      // gold_graph1_PU->SetTitle("TrackJetConfirmEff for PU with Scheme = HS and PileupEff = .9");
      // gold_graph1_PU->GetXaxis()->SetTitle("Eta");
      // gold_graph1_PU->GetYaxis()->SetTitle("TrackJetConfirmEff");
      // gold_graph1_PU->Draw("ALP");
      // silver_graph1_PU->Draw("LP");
      // bronze_graph1_PU->Draw("LP");

      // can->cd(2);
      // gold_graph1_HS->SetTitle("TrackJetConfirmEff for HS with Scheme = HS and PileupEff = .9");
      // gold_graph1_HS->GetXaxis()->SetTitle("Eta");
      // gold_graph1_HS->GetYaxis()->SetTitle("TrackJetConfirmEff");
      // gold_graph1_HS->Draw("ALP");
      // silver_graph1_HS->Draw("LP");
      // bronze_graph1_HS->Draw("LP");

      // Clear out vectors for second loop plots
      gold_HS.clear();
      gold_PU.clear();
      silver_HS.clear();
      silver_PU.clear();
      bronze_HS.clear();
      bronze_PU.clear();
      eta.clear();
    }

    else { // Generate and draw plots for Scheme = PU and PileupEff = .02
      // Create Track Jet Confirmation Efficiency for jets from Hard Scatter
      TGraph *bronze_graph2_HS = new TGraph(eta.size(), &eta[0], &bronze_HS[0]);
      bronze_graph2_HS->SetLineColor(1);
      bronze_graph2_HS->SetMarkerColor(1);
      TGraph *silver_graph2_HS = new TGraph(eta.size(), &eta[0], &silver_HS[0]);
      silver_graph2_HS->SetLineColor(4);
      silver_graph2_HS->SetMarkerColor(4);
      TGraph *gold_graph2_HS = new TGraph(eta.size(), &eta[0], &gold_HS[0]);
      gold_graph2_HS->SetLineColor(2);
      gold_graph2_HS->SetMarkerColor(2);

      // Create Track Jet Confirmation Efficiency for jets from Pileup
      TGraph *bronze_graph2_PU = new TGraph(eta.size(), &eta[0], &bronze_PU[0]);
      bronze_graph2_PU->SetLineColor(1);
      bronze_graph2_PU->SetMarkerColor(1);
      TGraph *silver_graph2_PU = new TGraph(eta.size(), &eta[0], &silver_PU[0]);
      silver_graph2_PU->SetLineColor(4);
      silver_graph2_PU->SetMarkerColor(4);
      TGraph *gold_graph2_PU = new TGraph(eta.size(), &eta[0], &gold_PU[0]);
      gold_graph2_PU->SetLineColor(2);
      gold_graph2_PU->SetMarkerColor(2);

      // can->cd(3);
      // gold_graph2_PU->SetTitle("TrackJetConfirmEff for PU with Scheme = PU and PileupEff = .02");
      // gold_graph2_PU->GetXaxis()->SetTitle("Eta");
      // gold_graph2_PU->GetYaxis()->SetTitle("TrackJetConfirmEff");
      // gold_graph2_PU->Draw("ALP");
      // silver_graph2_PU->Draw("LP");
      // bronze_graph2_PU->Draw("LP");

      // can->cd(4);
      gold_graph2_HS->SetTitle("TrackJetConfirmEff for HS with Scheme = PU and PileupEff = .02");
      gold_graph2_HS->GetXaxis()->SetTitle("Eta");
      gold_graph2_HS->GetYaxis()->SetTitle("TrackJetConfirmEff");
      gold_graph2_HS->Draw("ALP");
      // silver_graph2_HS->Draw("LP");
      // bronze_graph2_HS->Draw("LP");
    }
  }

  can->Update();
  can->Print("TrackJetConfirmEff.pdf");
  can->Close();

  return;
}

// Plotting of jet pileup stuff
// Dag Gillberg, Feb 2018

TH1F *makeHisto(int Nbins, double xmin, double xmax, int col = kBlue, int ls = 1, TString tit = "") {
  TH1F *h = new TH1F("", tit, Nbins, xmin, xmax);
  h->SetLineColor(col); h->SetLineStyle(ls); h->SetLineWidth(2); h->SetStats(0);
  h->GetYaxis()->SetTitleOffset(1.2);
  return h;
}

void drawText(double x, double y, TString txt, int col = kBlack) {
  static TLatex *tex = new TLatex(); tex->SetTextSize(0.045); tex->SetNDC(); tex->SetTextColor(col); tex->DrawLatex(x, y, txt);
}

void plotPUjetPerformance(Upgrade::UpgradePerformanceFunctions *upgrade) {
  TCanvas *can = new TCanvas();
  can->SetTopMargin(0.04); can->SetRightMargin(0.04); can->SetBottomMargin(0.12); can->SetLeftMargin(0.14);
  TString pdf("HLLHC_PUjetPerformance.pdf");
  can->Print(pdf + "[");

  printf("\n===========\n plotPUjetPerformance\n============\n\n");

  // Since we want to look at pileup jets, let's create an emtpy HS-jet vector
  Upgrade::Jets hsJets;
  Upgrade::Jets recoJets = upgrade->getSmearedJets(hsJets);

  // PU overlay
  int Nevents = 5000;
  upgrade->getPileupJets();
  printf("Will sample %i PU overlay events (will take a while, sorry)\n", Nevents);

  auto *h_NPUjets = makeHisto(30, -0.5, 29.5, kAzure, 1, ";Number of PU jets, #it{p}_{T} > 30 GeV;Number of events");
  auto *h_pTPUjets = makeHisto(40, 0, 200, kAzure, 1, ";#it{p}_{T} [GeV];Number of PU jets");
  auto *h_pTPUjetsJVT = makeHisto(40, 0, 200, kRed, 1, ";#it{p}_{T} [GeV];Number of PU jets");
  auto *h_etaPUjets = makeHisto(50, -5.0, 5, kAzure, 1, ";#it{#eta};Number of PU jets");
  auto *h_etaPUjetsJVT = makeHisto(50, -5.0, 5, kRed, 1, ";#it{#eta};Number of PU jets");

  for (int ievt = 0; ievt < Nevents; ++ievt) {
    std::vector<TLorentzVector> all_pujets = upgrade->getPileupJets();
    std::vector<TLorentzVector> pujets;
    for (auto pujet : all_pujets) {
      double jvtEff = upgrade->getJVTeff_PUjet(pujet.Pt(), pujet.Eta());
      if ( gRandom->Rndm() > jvtEff ) continue;
      pujets.push_back(pujet);
    }
    if (ievt % 100 == 0) printf("Evt %4i has %3lu PU jets, %lu survived JVT\n", ievt, all_pujets.size(), pujets.size());
    h_NPUjets->Fill(all_pujets.size());
    for (auto j : all_pujets) h_pTPUjets->Fill(j.Pt() / 1000);
    for (auto j : pujets) h_pTPUjetsJVT->Fill(j.Pt() / 1000);
    for (auto j : all_pujets) h_etaPUjets->Fill(j.Eta());
    for (auto j : pujets) h_etaPUjetsJVT->Fill(j.Eta());
  }
  h_NPUjets->Draw();
  drawText(0.5, 0.88, "Prior to any JVT cuts");
  can->Print(pdf);

  h_pTPUjets->Draw();
  h_pTPUjetsJVT->Draw("same"); h_pTPUjetsJVT->Draw("same e");
  drawText(0.5, 0.88, "PU jets, no JVT", kAzure); drawText(0.5, 0.82, "PU jets, standard JVT selection", kRed);
  can->Print(pdf);

  h_etaPUjets->Draw();
  h_etaPUjetsJVT->Draw("same"); h_etaPUjetsJVT->Draw("same e");
  drawText(0.5, 0.88, "PU jets, no JVT", kAzure); drawText(0.5, 0.82, "PU jets, standard JVT selection", kRed);
  can->Print(pdf);

  can->SetLogy();
  h_pTPUjets->GetYaxis()->SetRangeUser(0.5, h_pTPUjets->GetMaximum() * 2);
  h_pTPUjets->Draw();
  h_pTPUjetsJVT->Draw("same"); h_pTPUjetsJVT->Draw("same e");
  drawText(0.5, 0.88, "PU jets, no JVT", kAzure); drawText(0.5, 0.82, "PU jets, standard JVT selection", kRed);
  can->Print(pdf);

  h_etaPUjets->GetYaxis()->SetRangeUser(0.5, h_etaPUjets->GetMaximum() * 2);
  h_etaPUjets->Draw();
  h_etaPUjetsJVT->Draw("same"); h_etaPUjetsJVT->Draw("same e");
  drawText(0.5, 0.88, "PU jets, no JVT", kAzure); drawText(0.5, 0.82, "PU jets, standard JVT selection", kRed);
  can->Print(pdf);
  can->SetLogy(0);

  // will plot JVT efficinecy vs eta for the following pT and eta values
  vec pts({25, 40, 60, 80, 120});
  vec etas({0.5, 1.5, 2.5, 3.2, 3.75});

  // create axis and canvas
  TH1F *axis = makeHisto(1, 0, 4.5, 1, 1, ";Jet #eta;JVT efficiency");
  TH1F *pTaxis = makeHisto(200, 20, 120, 1, 1, ";Jet #it{p}_{T};JVT efficiency");
  axis->GetYaxis()->SetRangeUser(0, 1.2);
  pTaxis->GetYaxis()->SetRangeUser(0, 1.2);

  // make 3 sets of plots for ITk only, full vs limited HGTD
  for (TString det : {"ITk_only", "HGTD0", "HGTD1"}) {

    for (int old = 0; old < 1; ++old) {
      axis->Draw(); drawText(0.7, 0.88, det);
      drawText(0.15, 0.88, old ? "Old implementation" : "New implementation (Mar '18)");
      drawText(0.2, 0.45, "HS jets", kOrange); drawText(0.45, 0.45, "PU jets", kAzure);
      for (int pti = 0; pti < 5; ++pti) {
        TH1F *PUeff = new TH1F("", "", 180, 0, 4.5);
        TH1F *HSeff = new TH1F("", "", 180, 0, 4.5);
        PUeff->SetLineColor(kAzure + pti); PUeff->SetLineStyle(pti + 1); PUeff->SetLineWidth(2);
        HSeff->SetLineColor(kOrange + pti); HSeff->SetLineStyle(pti + 1); HSeff->SetLineWidth(2);
        for (int bin = 1; bin <= PUeff->GetNbinsX(); ++bin) {
          double eta = PUeff->GetBinCenter(bin);
          if (old == 0) {
            PUeff->SetBinContent(bin, upgrade->getJVTeff_PUjet(pts[pti] * 1000, eta));
            HSeff->SetBinContent(bin, upgrade->getJVTeff_HSjet(pts[pti] * 1000, eta));
          } else {
            PUeff->SetBinContent(bin, upgrade->getTrackJetConfirmEff(pts[pti] * 1000, eta, "PU"));
            HSeff->SetBinContent(bin, upgrade->getTrackJetConfirmEff(pts[pti] * 1000, eta, "HS"));
          }
        }
        PUeff->Draw("hist l same"); HSeff->Draw("hist l same");
        drawText(0.45, 0.4 - 0.05 * pti, Form("#it{p}_{T} = %.0f GeV", pts[pti]), kAzure + pti);
        drawText(0.20, 0.4 - 0.05 * pti, Form("#it{p}_{T} = %.0f GeV", pts[pti]), kOrange + pti);
      }
      can->Print(pdf);

      pTaxis->Draw(); drawText(0.7, 0.88, det);
      drawText(0.15, 0.88, old ? "Old implementation" : "New implementation (Mar '18)");
      drawText(0.2, 0.45, "HS jets", kOrange); drawText(0.45, 0.45, "PU jets", kAzure);
      for (int etai = 0; etai < 5; ++etai) {
        double eta = etas[etai];
        TH1F *PUeff = makeHisto(200, 20, 120, kAzure + etai, etai + 1);
        TH1F *HSeff = makeHisto(200, 20, 120, kOrange + etai, etai + 1);
        for (int bin = 1; bin <= PUeff->GetNbinsX(); ++bin) {
          double pT = PUeff->GetBinCenter(bin);
          if (old == 0) {
            PUeff->SetBinContent(bin, upgrade->getJVTeff_PUjet(pT * 1000, eta));
            HSeff->SetBinContent(bin, upgrade->getJVTeff_HSjet(pT * 1000, eta));
          } else {
            PUeff->SetBinContent(bin, upgrade->getTrackJetConfirmEff(pT * 1000, eta, "PU"));
            HSeff->SetBinContent(bin, upgrade->getTrackJetConfirmEff(pT * 1000, eta, "HS"));
          }
        }
        PUeff->Draw("hist l same"); HSeff->Draw("hist l same");
        drawText(0.45, 0.4 - 0.05 * etai, Form("|#it{#eta}| = %.2f", eta), kAzure + etai);
        drawText(0.20, 0.4 - 0.05 * etai, Form("|#it{#eta}| = %.2f", eta), kOrange + etai);
      } // for each eta value
      can->Print(pdf);
    } // for old vs new implementation
  } // for each detector (ITk only vs HGTD)
  can->Print(pdf + "]");
}

void plotLargeRjetSmearing(Upgrade::UpgradePerformanceFunctions *upgrade) {
  TString pdf("HLLHC_largeRjetSmearing.pdf");
  TCanvas *can = new TCanvas();
  can->SetTopMargin(0.04); can->SetRightMargin(0.04); can->SetBottomMargin(0.12); can->SetLeftMargin(0.12);
  can->Print(pdf + "[");
  // See https://cds.cern.ch/record/2290109/

  printf("JMR for pT=400 and m=200 is: %.3f\n", upgrade->getLargeRjetJMR(400e3, 200e3));

  TH1F *pTaxis = makeHisto(5, 200, 2000, 1, 1, ";Large-R jet #it{p}_{T} [GeV];Jet mass resolution #sigma_{#it{m}} / #it{m}");
  pTaxis->SetStats(0); pTaxis->GetYaxis()->SetRangeUser(0, 0.5);

  TH1F *jmrQCD    = makeHisto(180, 200, 2000, kOrange + 1);
  TH1F *jmrWprime = makeHisto(180, 200, 2000, kAzure);
  TH1F *jmrZprime = makeHisto(180, 200, 2000, kGreen + 1);
  for (double mOpT : {0.1, 0.25}) {
    for (int bin = 1; bin <= jmrQCD->GetNbinsX(); ++bin) {
      double pT = jmrQCD->GetBinCenter(bin) * 1000;
      double mass = mOpT * pT;
      jmrQCD->SetBinContent(bin, upgrade->getLargeRjetJMR(pT, mass));
      jmrWprime->SetBinContent(bin, upgrade->getLargeRjetJMR(pT, mass, UpgradePerformanceFunctions::Wprime));
      jmrZprime->SetBinContent(bin, upgrade->getLargeRjetJMR(pT, mass, UpgradePerformanceFunctions::Zprime));
    }

    pTaxis->Draw();
    jmrQCD->Draw("hist l same"); jmrWprime->Draw("hist l same"); jmrZprime->Draw("hist l same");
    drawText(0.18, 0.88, Form("Anti-#it{k_{t}} #it{R} = 1.0, trimmed, #LT#it{m}#GT = 200, #it{m} / #it{p}_{T} = %.2f", mOpT));
    drawText(0.6, 0.83, "Sample topology");
    drawText(0.6, 0.78, "QCD", kOrange + 1); drawText(0.6, 0.73, "W'", kAzure); drawText(0.6, 0.68, "Z'", kGreen + 1);
    can->Print(pdf);
  }

  pTaxis->SetYTitle("Jet #it{p}_{T} resolution #sigma_{#it{p}_{T}} / #it{p}_{T}");
  pTaxis->GetYaxis()->SetRangeUser(0, 0.4);
  for (int bin = 1; bin <= jmrQCD->GetNbinsX(); ++bin) {
    double pT = jmrQCD->GetBinCenter(bin) * 1000;
    double mass = 0.2 * pT;
    jmrQCD->SetBinContent(bin, upgrade->getLargeRjetJER(pT, mass));
    jmrWprime->SetBinContent(bin, upgrade->getLargeRjetJER(pT, mass, UpgradePerformanceFunctions::Wprime));
    jmrZprime->SetBinContent(bin, upgrade->getLargeRjetJER(pT, mass, UpgradePerformanceFunctions::Zprime));
  }
  pTaxis->Draw();
  jmrQCD->Draw("hist l same"); jmrWprime->Draw("hist l same"); jmrZprime->Draw("hist l same");
  drawText(0.18, 0.88, "Anti-#it{k_{t}} #it{R} = 1.0, trimmed, #LT#it{m}#GT = 200, #it{m} / #it{p}_{T} = 0.2");
  drawText(0.6, 0.83, "Sample topology");
  drawText(0.6, 0.78, "QCD", kOrange + 1); drawText(0.6, 0.73, "W'", kAzure); drawText(0.6, 0.68, "Z'", kGreen + 1);
  can->Print(pdf);

  // Shoot in a jet with pT=400, m=80 GeV to the detector.
  // See what mass is reconstructed
  TH1F *mAxis = makeHisto(1, 0, 200, 1, 1, ";Large-R jet #it{m} [GeV];Number of jets");
  mAxis->SetStats(0);
  TH1F *jmQCD    = makeHisto(100, 0, 200, kOrange + 1);
  TH1F *jmWprime = makeHisto(100, 0, 200, kAzure);
  TH1F *jmZprime = makeHisto(100, 0, 200, kGreen + 1);

  double pT_truth = 400e3, mass_truth = 80e3;
  for (int i = 0; i < 10000; ++i) {
    jmQCD->Fill(upgrade->getSmearedLargeRjet(pT_truth, 0.0, 0.0, mass_truth).M() / 1000);
    jmWprime->Fill(upgrade->getSmearedLargeRjet(pT_truth, 0.0, 0.0, mass_truth, UpgradePerformanceFunctions::Wprime).M() / 1000);
    jmZprime->Fill(upgrade->getSmearedLargeRjet(pT_truth, 0.0, 0.0, mass_truth, UpgradePerformanceFunctions::Zprime).M() / 1000);
  }

  mAxis->SetMaximum(1.2 * jmZprime->GetMaximum());
  mAxis->Draw();
  drawText(0.18, 0.88, Form("Anti-#it{k_{t}} #it{R} = 1.0, #LT#it{m}#GT = 200. Truth #it{m} = %.0f GeV, #it{p}_{T} = %.0f GeV",
                            mass_truth / 1000, pT_truth / 1000));
  drawText(0.6, 0.83, "Sample topology");
  drawText(0.6, 0.78, "QCD", kOrange + 1); drawText(0.6, 0.73, "W'", kAzure); drawText(0.6, 0.68, "Z'", kGreen + 1);
  jmZprime->Draw("same"); jmWprime->Draw("same");
  jmQCD->Draw("same");
  can->Print(pdf);

  mass_truth = 15e3;
  jmQCD    = makeHisto(100, 0, 50, kOrange + 1, 1, ";Large-R jet #it{m} [GeV];Number of jets");
  for (int i = 0; i < 10000; ++i)
    jmQCD->Fill(upgrade->getSmearedLargeRjet(pT_truth, 0.0, 0.0, mass_truth).M() / 1000);

  jmQCD->Draw();
  drawText(0.18, 0.88, Form("Anti-#it{k_{t}} #it{R} = 1.0, #LT#it{m}#GT = 200. Truth #it{m} = %.0f GeV, #it{p}_{T} = %.0f GeV",
                            mass_truth / 1000, pT_truth / 1000));
  drawText(0.6, 0.83, "Sample topology"); drawText(0.6, 0.78, "QCD", kOrange + 1);
  can->Print(pdf);

  can->Print(pdf + "]");
  printf("\nProduced %s\n", pdf.Data());
}

void plotJER(Upgrade::UpgradePerformanceFunctions *upgrade, double CtermPenalty) {
  TString pdf("HLLHC_jetSmearing.pdf");
  upgrade->setConstantTermPenaltyFactor(CtermPenalty);
  TCanvas *can = new TCanvas();
  can->SetLogx();
  can->SetTopMargin(0.04); can->SetRightMargin(0.04); can->SetBottomMargin(0.12); can->SetLeftMargin(0.12);
  can->Print(pdf + "[");

  TH1F *pTaxis = makeHisto(1, 30, 2000, 1, 1, ";Jet #it{p}_{T} [GeV];JER #sigma_{#it{p}_{T}} / #it{p}_{T}");
  pTaxis->GetYaxis()->SetRangeUser(0, 0.5); pTaxis->GetXaxis()->SetMoreLogLabels();
  vec etas({0.5, 1.0, 1.5, 2.5, 3.0, 3.7});
  std::vector<int> cols({kRed, kOrange, kGreen + 1, kBlue, kViolet, kBlack});
  printf(" JER is %.3f\n", upgrade->getJetEnergyResolution(40e3, 1.0, 1));

  for (int par = 0; par < 2; ++par) {

    pTaxis->Draw();
    for (size_t etai = 0; etai < etas.size(); ++etai) {
      double eta = etas[etai];
      TH1F *jer = makeHisto(117, 30, 1200, cols[etai], etai + 1);
      for (int bin = 1; bin <= jer->GetNbinsX(); ++bin) {
        double pT = jer->GetBinCenter(bin);
        jer->SetBinContent(bin, upgrade->getJetEnergyResolution(pT * 1000, eta, 1));
      }
      jer->Draw("same hist l");
      drawText(0.60, 0.6 - 0.05 * etai, Form("|#it{#eta}| = %.2f", eta), cols[etai]);
    }
    drawText(0.60, 0.85, Form("%s #it{#mu} = %.0f", "Previous", 200.0));
    can->Print(pdf);

    for (double mu : {200, 22, 0}) {

      pTaxis->Draw();
      for (size_t etai = 0; etai < etas.size(); ++etai) {
        double eta = etas[etai];
        TH1F *jer = makeHisto(117, 30, 1200, cols[etai], etai + 1);
        for (int bin = 1; bin <= jer->GetNbinsX(); ++bin) {
          double pT = jer->GetBinCenter(bin);
          if (par == 0) jer->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::TopoEM));
          else
            jer->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::PFlow));
        }
        jer->Draw("same hist l");
        drawText(0.60, 0.6 - 0.05 * etai, Form("|#it{#eta}| = %.2f", eta), cols[etai]);
      }
      drawText(0.60, 0.85, Form("%s #it{#mu} = %.0f", par == 0 ? "EM+JES" : "PFlow", mu));
      drawText(0.60, 0.80, Form("C-term penalty: %.2f", CtermPenalty));
      can->Print(pdf);
    }
  }

  double mu = 200;
  for (size_t etai = 0; etai < etas.size(); ++etai) {
    double eta = etas[etai];
    pTaxis->Draw();
    TH1F *jerCur   = makeHisto(117, 30, 1200, cols[0]);
    TH1F *jerTopo  = makeHisto(117, 30, 1200, cols[1]);
    TH1F *jerPflow = makeHisto(117, 30, 1200, cols[2]);
    for (int bin = 1; bin <= jerCur->GetNbinsX(); ++bin) {
      double pT = jerCur->GetBinCenter(bin);
      jerTopo->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::TopoEM));
      jerPflow->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::PFlow));
      jerCur->SetBinContent(bin, upgrade->getJetEnergyResolution(pT * 1000, eta, true));
    }
    jerCur->Draw("same hist l"); jerTopo->Draw("same histl"); jerPflow->Draw("same hist l");
    drawText(0.60, 0.85, Form("|#it{#eta}| = %.2f", eta));
    drawText(0.60, 0.80, "Previous", cols[0]);
    drawText(0.60, 0.75, "TopoEM", cols[1]);
    drawText(0.60, 0.70, "PFlow", cols[2]);
    drawText(0.60, 0.65, Form("#it{#mu} = %.0f", mu));
    drawText(0.60, 0.60, Form("C-term penalty: %.2f", CtermPenalty));
    can->Print(pdf);
  }

  can->SetLogx(0);
  vec pts({30, 40, 60, 100, 1000});
  TH1F *eta_axis = makeHisto(1, 0, 4.5, 1, 1, ";Jet |#it{#eta}|;JER #sigma_{#it{p}_{T}} / #it{p}_{T}");
  eta_axis->GetYaxis()->SetRangeUser(0, 0.5);

  for (double pT : pts) {
    eta_axis->Draw();
    TH1F *jerCur   = makeHisto(90, 0, 4.5, cols[0]);
    TH1F *jerTopo  = makeHisto(90, 0, 4.5, cols[1]);
    TH1F *jerPflow = makeHisto(90, 0, 4.5, cols[2]);
    for (int bin = 1; bin <= jerCur->GetNbinsX(); ++bin) {
      double eta = jerCur->GetBinCenter(bin);
      jerTopo->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::TopoEM));
      jerPflow->SetBinContent(bin, upgrade->getJER(pT * 1000, eta, mu, UpgradePerformanceFunctions::PFlow));
      jerCur->SetBinContent(bin, upgrade->getJetEnergyResolution(pT * 1000, eta, true));
    }
    jerCur->Draw("same hist l"); jerTopo->Draw("same histl"); jerPflow->Draw("same hist l");
    drawText(0.60, 0.85, Form("#it{p}_{T} = %.0f GeV", pT));
    drawText(0.60, 0.80, "Previous", cols[0]);
    drawText(0.60, 0.75, "TopoEM", cols[1]);
    drawText(0.60, 0.70, "PFlow", cols[2]);
    drawText(0.60, 0.65, Form("#it{#mu} = %.0f", mu));
    drawText(0.60, 0.60, Form("C-term penalty: %.2f", CtermPenalty));
    can->Print(pdf);
  }

  //for (int etai=0;etai<40;++etai)
  //  printf("p-flow JER, pT=34 GeV, eta = %.1f : %.1f%%\n",0.1*etai,upgrade->getJER(34000,0.1*etai,200,UpgradePerformanceFunctions::PFlow)*100);
  can->Print(pdf + "]");
  printf("\nProduced %s\n", pdf.Data());
}
