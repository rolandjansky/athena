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
  all_roi_types.insert("RoI_EF_mu");
  TMNUtil util;
  util.setRoITypes(all_roi_types);
  util.init(t, t_mapping);


  // dclearing 
  TrigMuon Muon(t_mu);
  TH1F* h_MuFast_pt_in = new TH1F("h_MuFast_pt_in", "", 100, 0, 100);
  TH1F* h_MuFast_pt_pass = new TH1F("h_MuFast_pt_pass", "", 100, 0, 100);
  TH2F* h_MuFastMuComb_col_eta = new TH2F("h_MuFastMuComb_col_eta", "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  TH2F* h_MuCombEFMuon_col_eta = new TH2F("h_MuCombEFMuon_col_eta", "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  TH1F* h_MuComb_eta_pass = new TH1F("h_MuComb_eta_pass", "", 64, -3.2, 3.2);
  TH1F* h_TrigMuonEF_eta = new TH1F("h_TrigMuonEF_eta", "", 64, -3.2, 3.2);

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


    // Declear temporary container
    vector<float> MuFast_pt_in;
    vector<float> MuFast_pt_pass;
    multimap<float, float> MuFastMuComb_col_eta;
    multimap<float, float> MuCombEFMuon_col_eta;
    vector<float> MuComb_eta_pass;
    vector<float> TrigMuonEF_eta;
    // Fill variables
    fillVar( MuFast_pt_in, "L2_mu10", false, chain_entries, roi_links, "MuonFeature", Muon.Mufast_MuFast_pt, Muon.Mufast_N );
    fillVar( MuFast_pt_pass, "L2_mu10", true, chain_entries, roi_links, "MuonFeature", Muon.Mufast_MuFast_pt, Muon.Mufast_N );
    fillVarPair( MuFastMuComb_col_eta ,"L2_mu10", chain_entries, roi_links, "MuonFeature", "CombinedMuonFeature",
		 Muon.Mufast_MuFast_eta, Muon.Mufast_N, Muon.Mucomb_MuComb_eta, Muon.Mucomb_N);

    vector<ChainEntry>::const_iterator pChain;
    for (pChain=chain_entries.begin(); pChain!=chain_entries.end(); ++pChain) {
      if( pChain->getName()=="EF_mu6" ) {
	//cout << pChain->getName() << ":" << endl;
	std::vector<CombLinks> EFCombs = roi_links.getCombLinks(*pChain);
	//cout << "Size of EF Comblinks : " << EFCombs.size() << endl;
	fillVar( TrigMuonEF_eta, EFCombs, false, "TrigMuonEFInfoContainer", Muon.TrigMuonEF_MuonEF_eta, Muon.TrigMuonEF_N);

	std::vector<CombLinks> L2Combs = getSameRoI( EFCombs, roi_links, 1001, "Muon_ROI");
	//cout << "Size of L2 CombLinks : " << L2Combs.size() << endl;
	fillVar( MuComb_eta_pass, L2Combs, true, "CombinedMuonFeature", Muon.Mucomb_MuComb_eta, Muon.Mucomb_N);
      }
    }

    fillTH1abs( h_MuFast_pt_in, MuFast_pt_in );
    fillTH1abs( h_MuFast_pt_pass, MuFast_pt_pass );
    fillTH2( h_MuFastMuComb_col_eta, MuFastMuComb_col_eta );

    fillTH1( h_TrigMuonEF_eta, TrigMuonEF_eta );
    fillTH1( h_MuComb_eta_pass, MuComb_eta_pass );


    int nMuComb = MuComb_eta_pass.size();
    int nTrigMuonEF = TrigMuonEF_eta.size();
    if(nMuComb != nTrigMuonEF) {
      cout << "Number of L2pass && EFin is not same" << endl;
      cout << "L2 : " << nMuComb << endl;
      cout << "EF : " << nTrigMuonEF << endl;
      continue;
    }

    for(int i=0; i<nMuComb; i++){
      h_MuCombEFMuon_col_eta->Fill( MuComb_eta_pass[i], TrigMuonEF_eta[i] );
    }
  }// -------------------- Event Loop for Filling Histograms -------------------- //



  //-------------------- Draw Histograms  --------------------//
  gStyle->SetPalette(1,0);
  TCanvas* c = new TCanvas( "c", "", 600,600);
  c->Divide(2,2);
  c->cd(1);
  h_MuFast_pt_in->SetLabelSize(0.065, "XY");
  h_MuFast_pt_in->SetXTitle("MuFast_pt[GeV]");
  h_MuFast_pt_in->SetNdivisions(505);
  h_MuFast_pt_in->Draw();
  h_MuFast_pt_pass->SetFillColor(kRed);
  h_MuFast_pt_pass->Draw("same");

  c->cd(2);
  h_MuFastMuComb_col_eta->SetLabelSize(0.065, "XY");
  h_MuFastMuComb_col_eta->SetXTitle("MuFast_eta");
  h_MuFastMuComb_col_eta->SetYTitle("MuComb_eta");
  h_MuFastMuComb_col_eta->Draw("colz");

  c->cd(3);
  h_MuComb_eta_pass->SetLabelSize(0.065, "XY");
  h_MuComb_eta_pass->SetXTitle("MuComb_eta_pass");
  h_MuComb_eta_pass->SetNdivisions(505);
  h_MuComb_eta_pass->Draw();
  h_TrigMuonEF_eta->SetLineColor(kRed);
  h_TrigMuonEF_eta->Draw("same");

  c->cd(4);
  h_MuCombEFMuon_col_eta->SetLabelSize(0.065, "XY");
  h_MuCombEFMuon_col_eta->SetXTitle("MuComb_eta");
  h_MuCombEFMuon_col_eta->SetYTitle("TrigMuonEF_eta");
  h_MuCombEFMuon_col_eta->Draw("colz");
}
