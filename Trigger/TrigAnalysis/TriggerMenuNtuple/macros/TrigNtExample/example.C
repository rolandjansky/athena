/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  example.C
*/
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/TMNUtil.h"
#include "TrigMuon.h"
#include "myTrigVarAnalysis.h"

using namespace std;


void example(TTree* t, TTree* t_mapping, TTree* t_mu);

void example(const string filename="ntuple/trigntuple.root") {
  // declearing TTree
  TFile* f = new TFile(filename.c_str());
  TTree* t = dynamic_cast<TTree*>(f->Get("Menu/TrigMenu"));
  TTree* t_mapping = dynamic_cast<TTree*>(f->Get("Menu/ChainFeatureMapping"));
  TTree* t_mu = dynamic_cast<TTree*>(f->Get("Muon/TrigMuon"));

  example(t, t_mapping, t_mu);
}

void example(TTree* t, TTree* t_mapping, TTree* t_mu) {

  gROOT->cd();

  // preparing for ChainEntry && RoILinks
  std::vector<ChainEntry> chain_entries;
  std::vector<ChainEntry>::const_iterator p_chain;
  RoILinks roi_links;
  std::set<std::string> all_roi_types;
  all_roi_types.insert("RoI_L2_mu");
  TMNUtil util;
  util.setRoITypes(all_roi_types);
  util.init(t, t_mapping);


  // declearation objects for muon analysis
  TrigMuon Muon(t_mu);
  myTrigVarAnalysis muVar( "L2_mu10", false );
  myTrigVarAnalysis muVar_pass( "L2_mu10", true );
  muVar.define();
  muVar_pass.define();

  int nentries = t->GetEntries();
  int ientry;
  // -------------------- Event Loop for Filling Histograms -------------------- //
  for (ientry=0; ientry<nentries; ++ientry) {
    cout << "******************** Event:" << ientry+1 << " ********************" << endl;
    // GetEntry
    t->GetEntry(ientry);
    Muon.GetEntry(ientry);
    // Reconstruct ChainEntries & RoILinks
    util.reconstructChainEntryRoILinks(chain_entries, roi_links);

    // Fill histograms
    muVar.fill(chain_entries, roi_links, Muon);
    muVar_pass.fill(chain_entries, roi_links, Muon);

    
  }// -------------------- Event Loop for Filling Histograms -------------------- //




  //-------------------- Draw Histograms  --------------------//
  gStyle->SetPalette(1,0);
  TCanvas* c = new TCanvas( "c", "", 900, 900 );
  c->Divide(3,3);
  // L1MuonRoI
  c->cd(1);
  TH1F* L1RoIPt = static_cast<TH1F*>(muVar.getHistL1RoIPt());
  L1RoIPt->SetLabelSize(0.065, "XY");
  L1RoIPt->SetXTitle("L1MuonRoIPt [GeV]");
  L1RoIPt->SetNdivisions(505);
  L1RoIPt->Draw();
  c->cd(2);
  TH1F* L1RoIEta = static_cast<TH1F*>(muVar.getHistL1RoIEta());
  L1RoIEta->SetLabelSize(0.065, "XY");
  L1RoIEta->SetXTitle("L1MuonRoIEta");
  L1RoIEta->SetNdivisions(505);
  L1RoIEta->Draw();
  c->cd(3);
  TH1F* L1RoIPhi = static_cast<TH1F*>(muVar.getHistL1RoIPhi());
  L1RoIPhi->SetLabelSize(0.065, "XY");
  L1RoIPhi->SetXTitle("L1MuonRoIPhi");
  L1RoIPhi->SetNdivisions(505);
  L1RoIPhi->Draw();
  
  // MuFast
  c->cd(4);
  TH1F* MuFastPt = static_cast<TH1F*>(muVar.getHistMuFastPt());
  TH1F* MuFastPt_pass = static_cast<TH1F*>(muVar_pass.getHistMuFastPt());
  MuFastPt->SetLabelSize(0.065, "XY");
  MuFastPt->SetXTitle("MuFastPt [GeV]");
  MuFastPt->SetNdivisions(505);
  MuFastPt->Draw();
  MuFastPt_pass->SetFillColor(2);
  MuFastPt_pass->Draw("same");
  c->cd(5);
  TH1F* MuFastEta = static_cast<TH1F*>(muVar.getHistMuFastEta());
  TH1F* MuFastEta_pass = static_cast<TH1F*>(muVar_pass.getHistMuFastEta());
  MuFastEta->SetLabelSize(0.065, "XY");
  MuFastEta->SetXTitle("MuFastEta");
  MuFastEta->SetNdivisions(505);
  MuFastEta->Draw();
  MuFastEta_pass->SetFillColor(2);
  MuFastEta_pass->Draw("same");
  c->cd(6);
  TH1F* MuFastPhi = static_cast<TH1F*>(muVar.getHistMuFastPhi());
  TH1F* MuFastPhi_pass = static_cast<TH1F*>(muVar_pass.getHistMuFastPhi());
  MuFastPhi->SetLabelSize(0.065, "XY");
  MuFastPhi->SetXTitle("MuFastPhi");
  MuFastPhi->SetNdivisions(505);
  MuFastPhi->Draw();
  MuFastPhi_pass->SetFillColor(2);
  MuFastPhi_pass->Draw("same");
  
  // MuFast vs MuComb
  c->cd(7);
  TH2F* MuFastMuCombPtPt = static_cast<TH2F*>(muVar_pass.getHistColMuFastPtMuCombPt());
  MuFastMuCombPtPt->SetLabelSize(0.065, "XY");
  MuFastMuCombPtPt->SetXTitle("MuFastPt [GeV]");
  MuFastMuCombPtPt->SetYTitle("MuCombPt [GeV]");
  MuFastMuCombPtPt->SetNdivisions(505);
  MuFastMuCombPtPt->Draw("colz");
  c->cd(8);
  TH2F* MuFastMuCombEtaEta = static_cast<TH2F*>(muVar_pass.getHistColMuFastEtaMuCombEta());
  MuFastMuCombEtaEta->SetLabelSize(0.065, "XY");
  MuFastMuCombEtaEta->SetXTitle("MuFastEta");
  MuFastMuCombEtaEta->SetYTitle("MuCombEta");
  MuFastMuCombEtaEta->SetNdivisions(505);
  MuFastMuCombEtaEta->Draw("colz");
  c->cd(9);
  TH2F* MuFastMuCombPhiPhi = static_cast<TH2F*>(muVar_pass.getHistColMuFastPhiMuCombPhi());
  MuFastMuCombPhiPhi->SetLabelSize(0.065, "XY");
  MuFastMuCombPhiPhi->SetXTitle("MuFastPhi");
  MuFastMuCombPhiPhi->SetYTitle("MuCombPhi");
  MuFastMuCombPhiPhi->SetNdivisions(505);
  MuFastMuCombPhiPhi->Draw("colz");

}
