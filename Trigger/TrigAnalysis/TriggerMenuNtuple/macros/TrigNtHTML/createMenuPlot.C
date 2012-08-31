/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  createMenuPlot.C
*/
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "HTMLUtil.h"
#include "TrigMenu.h"
#include "L1_Cluster.h"
#include "L2_Cluster.h"
#include "L2_Track.h"
#include "TrigMuon.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/TMNUtil.h"
#include "TrigVarInElectronChain.h"
#include "TrigVarInMuonChain.h"
#include "TrigVarInJetChain.h"

using namespace std;


void ReadFileToVector(const string& filename, vector<string>& vec);
TChain* CreateChain(std::string ListFile, std::string TreeName);

void createMenuPlot() {
  // ------------------------- Set Up -------------------------//
  const string NtupleList="list/ntuple.list";
  const string eList="list/e_chain.list";
  const string muList="list/mu_chain.list";
  const string jList="list/j_chain.list";
  const string www_dir   = "www/";
  const string file_type = ".gif";
  const string TopText = "ntuple.txt";
  // ----------------------------------------------------------//

  // read chains to check
  vector<string> eChain(0);
  ReadFileToVector( eList, eChain);
  vector<string> muChain(0);
  ReadFileToVector( muList, muChain);
  vector<string> jChain(0);
  ReadFileToVector( jList, jChain);

  // declearing TTree>
  TChain* t = CreateChain( NtupleList ,"/Menu/TrigMenu" );
  TChain* t_mapping = CreateChain( NtupleList ,"/Menu/ChainFeatureMapping");
  TChain* t_eg_l1 = CreateChain( NtupleList ,"/EGamma/L1_Cluster");
  TChain* t_eg_l2cls = CreateChain( NtupleList ,"/EGamma/L2_Cluster");
  TChain* t_eg_l2trk = CreateChain( NtupleList ,"/EGamma/L2_Track");
  TChain* t_mu = CreateChain( NtupleList ,"/Muon/TrigMuon");
  gROOT->cd();

  // preparing ChainEntry && RoILinks
  std::vector<ChainEntry> chain_entries;
  std::vector<ChainEntry>::const_iterator p_chain;
  RoILinks roi_links;
  std::set<std::string> all_roi_types;
  all_roi_types.insert("RoI_L2_mu");
  all_roi_types.insert("RoI_L2_e");
  all_roi_types.insert("RoI_L2_j");
  TrigMenu Menu(t);
  TMNUtil util;
  util.setRoITypes(all_roi_types);
  util.init(t, t_mapping);
  // declearation objects for egamma analysis
  L1_Cluster EMRoI(t_eg_l1);
  L2_Cluster L2Cluster(t_eg_l2cls);
  L2_Track L2Track(t_eg_l2trk);
  int nEchain = eChain.size();
  TrigVarInElectronChain eVar[nEchain];
  TrigVarInElectronChain eVar_pass[nEchain];
  for(int i=0; i<nEchain; i++){
    eVar[i] = TrigVarInElectronChain( eChain[i], false );
    eVar_pass[i] = TrigVarInElectronChain( eChain[i], true );
    eVar[i].define();
    eVar_pass[i].define();
  }
  // declearation objects for muon analysis
  TrigMuon Muon(t_mu);
  int nMUchain = muChain.size();
  TrigVarInMuonChain muVar[nMUchain];
  TrigVarInMuonChain muVar_pass[nMUchain];
  for(int i=0; i<nMUchain; i++){
    muVar[i] = TrigVarInMuonChain( muChain[i], false );
    muVar_pass[i] = TrigVarInMuonChain( muChain[i], true );
    muVar[i].define();
    muVar_pass[i].define();
  }
  // declearation objects for jet analysis
  int nJchain = jChain.size();
  TrigVarInJetChain jVar[nJchain];
  TrigVarInJetChain jVar_pass[nJchain];
  for(int i=0; i<nJchain; i++){
    jVar[i] = TrigVarInJetChain( jChain[i], false );
    jVar_pass[i] = TrigVarInJetChain( jChain[i], true );
    jVar[i].define();
    jVar_pass[i].define();
  }

  int nentries = t->GetEntries();
  int ientry;
  // -------------------- Event Loop for Filling Histograms -------------------- //
  for (ientry=0; ientry<nentries; ++ientry) {
    cout << "******************** Event:" << ientry+1 << " ********************" << endl;
    // GetEntry
    t->GetEntry(ientry);
    Menu.GetEntry(ientry);
    EMRoI.GetEntry(ientry);
    L2Cluster.GetEntry(ientry);
    L2Track.GetEntry(ientry);
    Muon.GetEntry(ientry);
    // Reconstruct ChainEntries & RoILinks
    util.reconstructChainEntryRoILinks(chain_entries, roi_links);
    // Loop For Electron chain
    for(int i=0; i<nEchain; i++){
      eVar[i].fill(chain_entries, roi_links, EMRoI, L2Cluster, L2Track);
      eVar_pass[i].fill(chain_entries, roi_links, EMRoI, L2Cluster, L2Track);
    }
    for(int i=0; i<nMUchain; i++){
      muVar[i].fill(chain_entries, roi_links, Muon);
      muVar_pass[i].fill(chain_entries, roi_links, Muon);
    }
    for(int i=0; i<nJchain; i++){
      jVar[i].fill(chain_entries, roi_links, Menu);
      jVar_pass[i].fill(chain_entries, roi_links, Menu);
    }
  }// Event Loop for Filling Histograms





  //-------------------- Creating Image Files --------------------//
  gStyle->SetPalette(1,0);
  //----- Electron -----//
  for(int i=0; i<nEchain; i++){
    TCanvas* c = new TCanvas( eChain[i].c_str(), "", 600, 1200 );
    c->Divide(3,6);
    // L1EMRoI
    c->cd(1);   
    TH1F* L1RoIEt = static_cast<TH1F*>(eVar[i].getHistL1RoIEt());
    L1RoIEt->SetLabelSize(0.065, "XY");
    L1RoIEt->SetXTitle("L1EMRoIEt [GeV]");
    L1RoIEt->SetNdivisions(505);
    L1RoIEt->Draw();
    c->cd(2);    
    TH1F* L1RoIEta = static_cast<TH1F*>(eVar[i].getHistL1RoIEta());
    L1RoIEta->SetLabelSize(0.065, "XY");
    L1RoIEta->SetXTitle("L1EMRoIEta");
    L1RoIEta->SetNdivisions(505);
    L1RoIEta->Draw();
    c->cd(3);    
    TH1F* L1RoIPhi = static_cast<TH1F*>(eVar[i].getHistL1RoIPhi());
    L1RoIPhi->SetLabelSize(0.065, "XY");
    L1RoIPhi->SetXTitle("L1EMRoIPhi");
    L1RoIPhi->SetNdivisions(505);
    L1RoIPhi->Draw();

    // L2Cluster
    c->cd(4);
    TH1F* L2ClusterEt = static_cast<TH1F*>(eVar[i].getHistL2ClusterEt());
    TH1F* L2ClusterEt_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2ClusterEt());
    L2ClusterEt->SetLabelSize(0.065, "XY");
    L2ClusterEt->SetXTitle("TrigEMClusterEt [GeV]");
    L2ClusterEt->SetNdivisions(505);
    L2ClusterEt->Draw();
    L2ClusterEt_pass->SetFillColor(2);
    L2ClusterEt_pass->Draw("same");
    c->cd(5);
    TH1F* L2ClusterEta = static_cast<TH1F*>(eVar[i].getHistL2ClusterEta());
    TH1F* L2ClusterEta_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2ClusterEta());
    L2ClusterEta->SetLabelSize(0.065, "XY");
    L2ClusterEta->SetXTitle("TrigEMClusterEta");
    L2ClusterEta->SetNdivisions(505);
    L2ClusterEta->Draw();
    L2ClusterEta_pass->SetFillColor(2);
    L2ClusterEta_pass->Draw("same");
    c->cd(6);
    TH1F* L2ClusterPhi = static_cast<TH1F*>(eVar[i].getHistL2ClusterPhi());
    TH1F* L2ClusterPhi_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2ClusterPhi());
    L2ClusterPhi->SetLabelSize(0.065, "XY");
    L2ClusterPhi->SetXTitle("TrigEMClusterPhi");
    L2ClusterPhi->SetNdivisions(505);
    L2ClusterPhi->Draw();
    L2ClusterPhi_pass->SetFillColor(2);
    L2ClusterPhi_pass->Draw("same");

    // L2Track
    c->cd(7);
    TH1F* L2SiTrackPt = static_cast<TH1F*>(eVar[i].getHistL2SiTrackPt());
    TH1F* L2SiTrackPt_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2SiTrackPt());
    L2SiTrackPt->SetLabelSize(0.065, "XY");
    L2SiTrackPt->SetXTitle("L2SiTrackPt [GeV]");
    L2SiTrackPt->SetNdivisions(505);
    L2SiTrackPt->Draw();
    L2SiTrackPt_pass->SetFillColor(2);
    L2SiTrackPt_pass->Draw("same");
    c->cd(8);
    TH1F* L2SiTrackEta = static_cast<TH1F*>(eVar[i].getHistL2SiTrackEta());
    TH1F* L2SiTrackEta_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2SiTrackEta());
    L2SiTrackEta->SetLabelSize(0.065, "XY");
    L2SiTrackEta->SetXTitle("L2SiTrackEta");
    L2SiTrackEta->SetNdivisions(505);
    L2SiTrackEta->Draw();
    L2SiTrackEta_pass->SetFillColor(2);
    L2SiTrackEta_pass->Draw("same");
    c->cd(9);
    TH1F* L2SiTrackPhi = static_cast<TH1F*>(eVar[i].getHistL2SiTrackPhi());
    TH1F* L2SiTrackPhi_pass = static_cast<TH1F*>(eVar_pass[i].getHistL2SiTrackPhi());
    L2SiTrackPhi->SetLabelSize(0.065, "XY");
    L2SiTrackPhi->SetXTitle("L2SiTrackPhi");
    L2SiTrackPhi->SetNdivisions(505);
    L2SiTrackPhi->Draw();
    L2SiTrackPhi_pass->SetFillColor(2);
    L2SiTrackPhi_pass->Draw("same");

    // L1RoI vs L2Cluster
    c->cd(10);
    TH2F* L1RoIL2ClusterEtEt = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIEtL2ClusterEt());
    L1RoIL2ClusterEtEt->SetLabelSize(0.065, "XY");
    L1RoIL2ClusterEtEt->SetXTitle("L1RoIEt [GeV]");
    L1RoIL2ClusterEtEt->SetYTitle("L2ClusterEt [GeV]");
    L1RoIL2ClusterEtEt->SetNdivisions(505);
    L1RoIL2ClusterEtEt->Draw("colz");
    c->cd(11);
    TH2F* L1RoIL2ClusterEtaEta = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIEtaL2ClusterEta());
    L1RoIL2ClusterEtaEta->SetLabelSize(0.065, "XY");
    L1RoIL2ClusterEtaEta->SetXTitle("L1RoIEta");
    L1RoIL2ClusterEtaEta->SetYTitle("L2ClusterEta");
    L1RoIL2ClusterEtaEta->SetNdivisions(505);
    L1RoIL2ClusterEtaEta->Draw("colz");
    c->cd(12);
    TH2F* L1RoIL2ClusterPhiPhi = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIPhiL2ClusterPhi());
    L1RoIL2ClusterPhiPhi->SetLabelSize(0.065, "XY");
    L1RoIL2ClusterPhiPhi->SetXTitle("L1RoIPhi");
    L1RoIL2ClusterPhiPhi->SetYTitle("L2ClusterPhi");
    L1RoIL2ClusterPhiPhi->SetNdivisions(505);
    L1RoIL2ClusterPhiPhi->Draw("colz");

    // L1RoI vs L2SiTrack
    c->cd(13);
    TH2F* L1RoIL2SiTrackEtPt = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIEtL2SiTrackPt());
    L1RoIL2SiTrackEtPt->SetLabelSize(0.065, "XY");
    L1RoIL2SiTrackEtPt->SetXTitle("L1RoIEt [GeV]");
    L1RoIL2SiTrackEtPt->SetYTitle("L2SiTrackEt [GeV]");
    L1RoIL2SiTrackEtPt->SetNdivisions(505);
    L1RoIL2SiTrackEtPt->Draw("colz");
    c->cd(14);
    TH2F* L1RoIL2SiTrackEtaEta = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIEtaL2SiTrackEta());
    L1RoIL2SiTrackEtaEta->SetLabelSize(0.065, "XY");
    L1RoIL2SiTrackEtaEta->SetXTitle("L1RoIEta");
    L1RoIL2SiTrackEtaEta->SetYTitle("L2SiTrackEta");
    L1RoIL2SiTrackEtaEta->SetNdivisions(505);
    L1RoIL2SiTrackEtaEta->Draw("colz");
    c->cd(15);
    TH2F* L1RoIL2SiTrackPhiPhi = static_cast<TH2F*>(eVar_pass[i].getHistColL1RoIPhiL2SiTrackPhi());
    L1RoIL2SiTrackPhiPhi->SetLabelSize(0.065, "XY");
    L1RoIL2SiTrackPhiPhi->SetXTitle("L1RoIPhi");
    L1RoIL2SiTrackPhiPhi->SetYTitle("L2SiTrackPhi");
    L1RoIL2SiTrackPhiPhi->SetNdivisions(505);
    L1RoIL2SiTrackPhiPhi->Draw("colz");


    // L2Cluster vs L2SiTrack
    c->cd(16);
    TH2F* L2ClusterL2SiTrackEtPt = static_cast<TH2F*>(eVar_pass[i].getHistColL2ClusterEtL2SiTrackPt());
    L2ClusterL2SiTrackEtPt->SetLabelSize(0.065, "XY");
    L2ClusterL2SiTrackEtPt->SetXTitle("L2ClusterEt [GeV]");
    L2ClusterL2SiTrackEtPt->SetYTitle("L2SiTrackEt [GeV]");
    L2ClusterL2SiTrackEtPt->SetNdivisions(505);
    L2ClusterL2SiTrackEtPt->Draw("colz");
    c->cd(17);
    TH2F* L2ClusterL2SiTrackEtaEta = static_cast<TH2F*>(eVar_pass[i].getHistColL2ClusterEtaL2SiTrackEta());
    L2ClusterL2SiTrackEtaEta->SetLabelSize(0.065, "XY");
    L2ClusterL2SiTrackEtaEta->SetXTitle("L2ClusterEta");
    L2ClusterL2SiTrackEtaEta->SetYTitle("L2SiTrackEta");
    L2ClusterL2SiTrackEtaEta->SetNdivisions(505);
    L2ClusterL2SiTrackEtaEta->Draw("colz");
    c->cd(18);
    TH2F* L2ClusterL2SiTrackPhiPhi = static_cast<TH2F*>(eVar_pass[i].getHistColL2ClusterPhiL2SiTrackPhi());
    L2ClusterL2SiTrackPhiPhi->SetLabelSize(0.065, "XY");
    L2ClusterL2SiTrackPhiPhi->SetXTitle("L2ClusterPhi");
    L2ClusterL2SiTrackPhiPhi->SetYTitle("L2SiTrackPhi");
    L2ClusterL2SiTrackPhiPhi->SetNdivisions(505);
    L1RoIL2SiTrackPhiPhi->Draw("colz");

    string plotFile = www_dir + eChain[i] + file_type;
    c->Print(plotFile.c_str());
    delete c;
  }// Loop for Electron Chain

  //----- Muon -----//
  for(int i=0; i<nMUchain; i++){
    TCanvas* c = new TCanvas( muChain[i].c_str(), "", 600, 1200 );
    c->Divide(3,6);
    // L1MuonRoI
    c->cd(1);
    TH1F* L1RoIPt = static_cast<TH1F*>(muVar[i].getHistL1RoIPt());
    L1RoIPt->SetLabelSize(0.065, "XY");
    L1RoIPt->SetXTitle("L1MuonRoIPt [GeV]");
    L1RoIPt->SetNdivisions(505);
    L1RoIPt->Draw();
    c->cd(2);
    TH1F* L1RoIEta = static_cast<TH1F*>(muVar[i].getHistL1RoIEta());
    L1RoIEta->SetLabelSize(0.065, "XY");
    L1RoIEta->SetXTitle("L1MuonRoIEta");
    L1RoIEta->SetNdivisions(505);
    L1RoIEta->Draw();
    c->cd(3);
    TH1F* L1RoIPhi = static_cast<TH1F*>(muVar[i].getHistL1RoIPhi());
    L1RoIPhi->SetLabelSize(0.065, "XY");
    L1RoIPhi->SetXTitle("L1MuonRoIPhi");
    L1RoIPhi->SetNdivisions(505);
    L1RoIPhi->Draw();

    // MuFast
    c->cd(4);
    TH1F* MuFastPt = static_cast<TH1F*>(muVar[i].getHistMuFastPt());
    TH1F* MuFastPt_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuFastPt());
    MuFastPt->SetLabelSize(0.065, "XY");
    MuFastPt->SetXTitle("MuFastPt [GeV]");
    MuFastPt->SetNdivisions(505);
    MuFastPt->Draw();
    MuFastPt_pass->SetFillColor(2);
    MuFastPt_pass->Draw("same");
    c->cd(5);
    TH1F* MuFastEta = static_cast<TH1F*>(muVar[i].getHistMuFastEta());
    TH1F* MuFastEta_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuFastEta());
    MuFastEta->SetLabelSize(0.065, "XY");
    MuFastEta->SetXTitle("MuFastEta");
    MuFastEta->SetNdivisions(505);
    MuFastEta->Draw();
    MuFastEta_pass->SetFillColor(2);
    MuFastEta_pass->Draw("same");
    c->cd(6);
    TH1F* MuFastPhi = static_cast<TH1F*>(muVar[i].getHistMuFastPhi());
    TH1F* MuFastPhi_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuFastPhi());
    MuFastPhi->SetLabelSize(0.065, "XY");
    MuFastPhi->SetXTitle("MuFastPhi");
    MuFastPhi->SetNdivisions(505);
    MuFastPhi->Draw();
    MuFastPhi_pass->SetFillColor(2);
    MuFastPhi_pass->Draw("same");

    // MuComb
    c->cd(7);
    TH1F* MuCombPt = static_cast<TH1F*>(muVar[i].getHistMuCombPt());
    TH1F* MuCombPt_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuCombPt());
    MuCombPt->SetLabelSize(0.065, "XY");
    MuCombPt->SetXTitle("MuCombPt [GeV]");
    MuCombPt->SetNdivisions(505);
    MuCombPt->Draw();
    MuCombPt_pass->SetFillColor(2);
    MuCombPt_pass->Draw("same");
    c->cd(8);
    TH1F* MuCombEta = static_cast<TH1F*>(muVar[i].getHistMuCombEta());
    TH1F* MuCombEta_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuCombEta());
    MuCombEta->SetLabelSize(0.065, "XY");
    MuCombEta->SetXTitle("MuCombEta");
    MuCombEta->SetNdivisions(505);
    MuCombEta->Draw();
    MuCombEta_pass->SetFillColor(2);
    MuCombEta_pass->Draw("same");
    c->cd(9);
    TH1F* MuCombPhi = static_cast<TH1F*>(muVar[i].getHistMuCombPhi());
    TH1F* MuCombPhi_pass = static_cast<TH1F*>(muVar_pass[i].getHistMuCombPhi());
    MuCombPhi->SetLabelSize(0.065, "XY");
    MuCombPhi->SetXTitle("MuCombPhi");
    MuCombPhi->SetNdivisions(505);
    MuCombPhi->Draw();
    MuCombPhi_pass->SetFillColor(2);
    MuCombPhi_pass->Draw("same");

    // L1RoI vs MuFast
    c->cd(10);
    TH2F* L1RoIMuFastPtPt = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIPtMuFastPt());
    L1RoIMuFastPtPt->SetLabelSize(0.065, "XY");
    L1RoIMuFastPtPt->SetXTitle("L1RoIPt [GeV]");
    L1RoIMuFastPtPt->SetYTitle("MuFastPt [GeV]");
    L1RoIMuFastPtPt->SetNdivisions(505);
    L1RoIMuFastPtPt->Draw("colz");
    c->cd(11);
    TH2F* L1RoIMuFastEtaEta = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIEtaMuFastEta());
    L1RoIMuFastEtaEta->SetLabelSize(0.065, "XY");
    L1RoIMuFastEtaEta->SetXTitle("L1RoIEta");
    L1RoIMuFastEtaEta->SetYTitle("MuFastEta");
    L1RoIMuFastEtaEta->SetNdivisions(505);
    L1RoIMuFastEtaEta->Draw("colz");
    c->cd(12);
    TH2F* L1RoIMuFastPhiPhi = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIPhiMuFastPhi());
    L1RoIMuFastPhiPhi->SetLabelSize(0.065, "XY");
    L1RoIMuFastPhiPhi->SetXTitle("L1RoIPhi");
    L1RoIMuFastPhiPhi->SetYTitle("MuFastPhi");
    L1RoIMuFastPhiPhi->SetNdivisions(505);
    L1RoIMuFastPhiPhi->Draw("colz");

    // L1RoI vs MuComb
    c->cd(13);
    TH2F* L1RoIMuCombPtPt = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIPtMuCombPt());
    L1RoIMuCombPtPt->SetLabelSize(0.065, "XY");
    L1RoIMuCombPtPt->SetXTitle("L1RoIPt [GeV]");
    L1RoIMuCombPtPt->SetYTitle("MuCombPt [GeV]");
    L1RoIMuCombPtPt->SetNdivisions(505);
    L1RoIMuCombPtPt->Draw("colz");
    c->cd(14);
    TH2F* L1RoIMuCombEtaEta = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIEtaMuCombEta());
    L1RoIMuCombEtaEta->SetLabelSize(0.065, "XY");
    L1RoIMuCombEtaEta->SetXTitle("L1RoIEta");
    L1RoIMuCombEtaEta->SetYTitle("MuCombEta");
    L1RoIMuCombEtaEta->SetNdivisions(505);
    L1RoIMuCombEtaEta->Draw("colz");
    c->cd(15);
    TH2F* L1RoIMuCombPhiPhi = static_cast<TH2F*>(muVar_pass[i].getHistColL1RoIPhiMuCombPhi());
    L1RoIMuCombPhiPhi->SetLabelSize(0.065, "XY");
    L1RoIMuCombPhiPhi->SetXTitle("L1RoIPhi");
    L1RoIMuCombPhiPhi->SetYTitle("MuCombPhi");
    L1RoIMuCombPhiPhi->SetNdivisions(505);
    L1RoIMuCombPhiPhi->Draw("colz");

    // MuFast vs MuComb
    c->cd(16);
    TH2F* MuFastMuCombPtPt = static_cast<TH2F*>(muVar_pass[i].getHistColMuFastPtMuCombPt());
    MuFastMuCombPtPt->SetLabelSize(0.065, "XY");
    MuFastMuCombPtPt->SetXTitle("MuFastPt [GeV]");
    MuFastMuCombPtPt->SetYTitle("MuCombPt [GeV]");
    MuFastMuCombPtPt->SetNdivisions(505);
    MuFastMuCombPtPt->Draw("colz");
    c->cd(17);
    TH2F* MuFastMuCombEtaEta = static_cast<TH2F*>(muVar_pass[i].getHistColMuFastEtaMuCombEta());
    MuFastMuCombEtaEta->SetLabelSize(0.065, "XY");
    MuFastMuCombEtaEta->SetXTitle("MuFastEta");
    MuFastMuCombEtaEta->SetYTitle("MuCombEta");
    MuFastMuCombEtaEta->SetNdivisions(505);
    MuFastMuCombEtaEta->Draw("colz");
    c->cd(18);
    TH2F* MuFastMuCombPhiPhi = static_cast<TH2F*>(muVar_pass[i].getHistColMuFastPhiMuCombPhi());
    MuFastMuCombPhiPhi->SetLabelSize(0.065, "XY");
    MuFastMuCombPhiPhi->SetXTitle("MuFastPhi");
    MuFastMuCombPhiPhi->SetYTitle("MuCombPhi");
    MuFastMuCombPhiPhi->SetNdivisions(505);
    MuFastMuCombPhiPhi->Draw("colz");

    string plotFile = www_dir + muChain[i] + file_type;
    c->Print(plotFile.c_str());
    delete c;
  }// Loop for Muon Chain




  //----- Jet -----//
  for(int i=0; i<nJchain; i++){
    TCanvas* c = new TCanvas( jChain[i].c_str(), "", 600, 600 );
    c->Divide(3,3);
    gStyle->SetPalette(1,0);
    // L1JetRoI
    c->cd(1);
    TH1F* L1RoI4x4Et = static_cast<TH1F*>(jVar[i].getHistL1RoI4x4Et());
    TH1F* L1RoI6x6Et = static_cast<TH1F*>(jVar[i].getHistL1RoI6x6Et());
    TH1F* L1RoI8x8Et = static_cast<TH1F*>(jVar[i].getHistL1RoI8x8Et());
    L1RoI8x8Et->SetLabelSize(0.065, "XY");
    L1RoI8x8Et->SetXTitle("L1JetRoIEt [GeV]");
    L1RoI8x8Et->SetNdivisions(505);
    L1RoI8x8Et->Draw();
    L1RoI6x6Et->SetLineColor(kRed);
    L1RoI4x4Et->SetLineColor(kBlue);
    L1RoI6x6Et->Draw("same");
    L1RoI4x4Et->Draw("same");
    TLegend* legend = new TLegend(0.68, 0.52, 0.88 , 0.82,"","NDC");
    legend -> AddEntry(L1RoI4x4Et, "4 x 4");
    legend -> AddEntry(L1RoI6x6Et, "6 x 6");
    legend -> AddEntry(L1RoI8x8Et, "8 x 8");
    legend -> Draw();
    c->cd(2);
    TH1F* L1RoIEta = static_cast<TH1F*>(jVar[i].getHistL1RoIEta());
    L1RoIEta->SetLabelSize(0.065, "XY");
    L1RoIEta->SetXTitle("L1JetRoIEta");
    L1RoIEta->SetNdivisions(505);
    L1RoIEta->Draw();
    c->cd(3);
    TH1F* L1RoIPhi = static_cast<TH1F*>(jVar[i].getHistL1RoIPhi());
    L1RoIPhi->SetLabelSize(0.065, "XY");
    L1RoIPhi->SetXTitle("L1JetRoIPhi");
    L1RoIPhi->SetNdivisions(505);
    L1RoIPhi->Draw();

    // L2Jet
    c->cd(4);
    TH1F* L2JetEt = static_cast<TH1F*>(jVar[i].getHistL2JetEt());
    TH1F* L2JetEt_pass = static_cast<TH1F*>(jVar_pass[i].getHistL2JetEt());
    L2JetEt->SetLabelSize(0.065, "XY");
    L2JetEt->SetXTitle("L2JetEt [GeV]");
    L2JetEt->SetNdivisions(505);
    L2JetEt->Draw();
    L2JetEt_pass->SetFillColor(2);
    L2JetEt_pass->Draw("same");
    c->cd(5);
    TH1F* L2JetEta = static_cast<TH1F*>(jVar[i].getHistL2JetEta());
    TH1F* L2JetEta_pass = static_cast<TH1F*>(jVar_pass[i].getHistL2JetEta());
    L2JetEta->SetLabelSize(0.065, "XY");
    L2JetEta->SetXTitle("L2JetEta");
    L2JetEta->SetNdivisions(505);
    L2JetEta->Draw();
    L2JetEta_pass->SetFillColor(2);
    L2JetEta_pass->Draw("same");
    c->cd(6);
    TH1F* L2JetPhi = static_cast<TH1F*>(jVar[i].getHistL2JetPhi());
    TH1F* L2JetPhi_pass = static_cast<TH1F*>(jVar_pass[i].getHistL2JetPhi());
    L2JetPhi->SetLabelSize(0.065, "XY");
    L2JetPhi->SetXTitle("L2JetPhi");
    L2JetPhi->SetNdivisions(505);
    L2JetPhi->Draw();
    L2JetPhi_pass->SetFillColor(2);
    L2JetPhi_pass->Draw("same");

    // L1RoI vs L2Jet
    c->cd(7);
    TH2F* L1RoI8x8L2JetEtEt = static_cast<TH2F*>(jVar_pass[i].getHistColL1RoI8x8EtL2JetEt());
    L1RoI8x8L2JetEtEt->SetLabelSize(0.065, "XY");
    L1RoI8x8L2JetEtEt->SetXTitle("L1RoI8x8Et [GeV]");
    L1RoI8x8L2JetEtEt->SetYTitle("L2JetEt [GeV]");
    L1RoI8x8L2JetEtEt->SetNdivisions(505);
    L1RoI8x8L2JetEtEt->Draw("colz");
    c->cd(8);
    TH2F* L1RoIL2JetEtaEta = static_cast<TH2F*>(jVar_pass[i].getHistColL1RoIEtaL2JetEta());
    L1RoIL2JetEtaEta->SetLabelSize(0.065, "XY");
    L1RoIL2JetEtaEta->SetXTitle("L1RoIEta");
    L1RoIL2JetEtaEta->SetYTitle("L2JetEta");
    L1RoIL2JetEtaEta->SetNdivisions(505);
    L1RoIL2JetEtaEta->Draw("colz");
    c->cd(9);
    TH2F* L1RoIL2JetPhiPhi = static_cast<TH2F*>(jVar_pass[i].getHistColL1RoIPhiL2JetPhi());
    L1RoIL2JetPhiPhi->SetLabelSize(0.065, "XY");
    L1RoIL2JetPhiPhi->SetXTitle("L1RoIPhi");
    L1RoIL2JetPhiPhi->SetYTitle("L2JetPhi");
    L1RoIL2JetPhiPhi->SetNdivisions(505);
    L1RoIL2JetPhiPhi->Draw("colz");


    string plotFile = www_dir + jChain[i] + file_type;
    c->Print(plotFile.c_str());
    delete c;
  }// Loop for Jet Chain


  CreateMain(www_dir);
  CreateTop(TopText, www_dir);
  CreateMenu(eChain, muChain, jChain, www_dir);
  img2html(eChain, "600", "1200", file_type, www_dir);
  img2html(muChain, "600", "1200", file_type, www_dir);
  img2html(jChain, "600", "600", file_type, www_dir);
}




void ReadFileToVector(const string& filename, vector<string>& vec){
  ifstream ifs( filename.c_str() );
  if( !ifs ){
    std::cout << "file open error: " << filename
	      << " does not exist or it's might be empty!" << std::endl;
  }
  string buf;
  while(ifs >> buf){
    vec.push_back(buf);
  }
}


TChain* CreateChain(std::string ListFile, std::string TreeName) {
  TChain* t = new TChain( TreeName.c_str() );
  ifstream ifs( ListFile.c_str() );
  if( !ifs ) {
    std::cout << "file open error: " << ListFile
	      << " does not exist or it's might be empty!" << std::endl;
  }

  std::string rootfile;
  while( ifs >> rootfile ){
    std::string fileToAdd = rootfile + TreeName;
    t->AddFile( fileToAdd.c_str() );
  }

  return t;
}
