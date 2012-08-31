/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInElectronChain_cxx__
#define __TrigVarInElectronChain_cxx__
/*
  TrigVarInElectronChain.cxx
*/

#include "TrigVarInElectronChain.h"

TrigVarInElectronChain::TrigVarInElectronChain(std::string chain_name,
					       bool ActiveOnly) :
  TrigVarInHlt(chain_name, ActiveOnly),
  mHistL1RoIEt(),  mHistL1RoIEta(),  mHistL1RoIPhi(),
  mHistL2ClusterEt(),  mHistL2ClusterEta(),  mHistL2ClusterPhi(),
  mL1RoIEt(0),  mL1RoIEta(0),  mL1RoIPhi(0),
  mL2ClusterEt(0),  mL2ClusterEta(0),  mL2ClusterPhi(0){
}


TrigVarInElectronChain::~TrigVarInElectronChain(){
}



void TrigVarInElectronChain::define(){
  const std::string hist_prefix=std::string("h_")+mChainName;
  std::string hname;
  std::string hist_postfix("");
  if (mActiveOnly) hist_postfix="_pass";

  // L1 EM_RoI //
  hname = hist_prefix + "_l1cluster_et" + hist_postfix;
  mHistL1RoIEt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_l1cluster_eta" + hist_postfix;
  mHistL1RoIEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l1cluster_phi" + hist_postfix;
  mHistL1RoIPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // L2 TrigEMCluster //
  hname = hist_prefix + "_l2cluster_et" + hist_postfix;
  mHistL2ClusterEt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_l2cluster_eta" + hist_postfix;
  mHistL2ClusterEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l2cluster_phi" + hist_postfix;
  mHistL2ClusterPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // L2 SiTrack //
  hname = hist_prefix + "_l2sitrack_pt" + hist_postfix;
  mHistL2SiTrackPt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_l2sitrack_eta" + hist_postfix;
  mHistL2SiTrackEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l2sitrack_phi" + hist_postfix;
  mHistL2SiTrackPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // correlation between L1RoI && L2Cluster //
  hname = hist_prefix + "_col_l1clusterl2cluster_EtEt" + hist_postfix;
  mHistColL1RoIEtL2ClusterEt = new TH2F(hname.c_str(), "", 100,0,100, 100,0,100);
  hname = hist_prefix + "_col_l1clusterl2cluster_EtaEta" + hist_postfix;
  mHistColL1RoIEtaL2ClusterEta = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);
  hname = hist_prefix + "_col_l1clusterl2cluster_PhiPhi" + hist_postfix;
  mHistColL1RoIPhiL2ClusterPhi = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);

  // correlation between L1RoI && L2SiTrack //
  hname = hist_prefix + "_col_l1clusterl2sitrack_EtPt" + hist_postfix;
  mHistColL1RoIEtL2SiTrackPt = new TH2F(hname.c_str(), "", 100,0,100, 100,0,100);
  hname = hist_prefix + "_col_l1clusterl2sitrack_EtaEta" + hist_postfix;
  mHistColL1RoIEtaL2SiTrackEta = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);
  hname = hist_prefix + "_col_l1clusterl2sitracj_PhiPhi" + hist_postfix;
  mHistColL1RoIPhiL2SiTrackPhi = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);

  // correlation between L1RoI && L2SiTrack //
  hname = hist_prefix + "_col_l2clusterl2sitrack_EtPt" + hist_postfix;
  mHistColL2ClusterEtL2SiTrackPt = new TH2F(hname.c_str(), "", 100,0,100, 100,0,100);
  hname = hist_prefix + "_col_l2clusterl2sitrack_EtaEta" + hist_postfix;
  mHistColL2ClusterEtaL2SiTrackEta = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);
  hname = hist_prefix + "_col_l2clusterl2sitracj_PhiPhi" + hist_postfix;
  mHistColL2ClusterPhiL2SiTrackPhi = new TH2F(hname.c_str(), "", 64,-3.2,3.2, 64,-3.2,3.2);
}


void TrigVarInElectronChain::fill(std::vector<ChainEntry>& chains,
				  RoILinks& roi,
				  const L1_Cluster& L1RoI,
				  const L2_Cluster& L2Clus,
				  const L2_Track& L2track) {
  // filling vector
  fillVar(mL1RoIEt, chains, roi, "EmTau_ROI", L1RoI.L1ROI_EMet);
  fillVar(mL1RoIEta, chains, roi, "EmTau_ROI", L1RoI.L1ROI_eta);
  fillVar(mL1RoIPhi, chains, roi, "EmTau_ROI", L1RoI.L1ROI_phi);
  fillVar(mL2ClusterEt, chains, roi, "TrigEMCluster", L2Clus.TrigEMCluster_et);
  fillVar(mL2ClusterEta, chains, roi, "TrigEMCluster", L2Clus.TrigEMCluster_eta);
  fillVar(mL2ClusterPhi, chains, roi, "TrigEMCluster", L2Clus.TrigEMCluster_phi);
  fillVar(mL2SiTrackPt, chains, roi, "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2track.TrigInDetTrack_pT);
  fillVar(mL2SiTrackEta, chains, roi, "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2track.TrigInDetTrack_eta);
  fillVar(mL2SiTrackPhi, chains, roi, "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2track.TrigInDetTrack_phi0);

  fillVarPair(mColL1RoIEtL2ClusterEt, chains, roi, "EmTau_ROI", "TrigEMCluster", L1RoI.L1ROI_EMet, L2Clus.TrigEMCluster_et);
  fillVarPair(mColL1RoIEtaL2ClusterEta, chains, roi, "EmTau_ROI", "TrigEMCluster", L1RoI.L1ROI_eta, L2Clus.TrigEMCluster_eta);
  fillVarPair(mColL1RoIPhiL2ClusterPhi, chains, roi, "EmTau_ROI", "TrigEMCluster", L1RoI.L1ROI_phi, L2Clus.TrigEMCluster_phi);
  fillVarPair(mColL1RoIEtL2SiTrackPt, chains, roi, "EmTau_ROI",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L1RoI.L1ROI_EMet, L2track.TrigInDetTrack_pT);
  fillVarPair(mColL1RoIEtaL2SiTrackEta, chains, roi, "EmTau_ROI",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L1RoI.L1ROI_eta, L2track.TrigInDetTrack_eta);
  fillVarPair(mColL1RoIPhiL2SiTrackPhi, chains, roi, "EmTau_ROI",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L1RoI.L1ROI_phi, L2track.TrigInDetTrack_phi0);
  fillVarPair(mColL2ClusterEtL2SiTrackPt, chains, roi, "TrigEMCluster",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2Clus.TrigEMCluster_et, L2track.TrigInDetTrack_pT);
  fillVarPair(mColL2ClusterEtaL2SiTrackEta, chains, roi, "TrigEMCluster",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2Clus.TrigEMCluster_eta, L2track.TrigInDetTrack_eta);
  fillVarPair(mColL2ClusterPhiL2SiTrackPhi, chains, roi, "TrigEMCluster",
	      "TrigInDetTrackCollection_TrigSiTrack_eGamma", L2Clus.TrigEMCluster_phi, L2track.TrigInDetTrack_phi0);

 
  // filling histograms
  fillTH1( mHistL1RoIEt, mL1RoIEt, 0.001 );
  fillTH1( mHistL1RoIEta, mL1RoIEta );
  fillTH1( mHistL1RoIPhi, mL1RoIPhi );
  fillTH1( mHistL2ClusterEt, mL2ClusterEt, 0.001 );
  fillTH1( mHistL2ClusterEta, mL2ClusterEta );
  fillTH1( mHistL2ClusterPhi, mL2ClusterPhi );
  fillTH1abs( mHistL2SiTrackPt, mL2SiTrackPt, 0.001 );
  fillTH1( mHistL2SiTrackEta, mL2SiTrackEta );
  fillTH1( mHistL2SiTrackPhi, mL2SiTrackPhi );
  fillTH2( mHistColL1RoIEtL2ClusterEt, mColL1RoIEtL2ClusterEt, 0.001, 0.001);
  fillTH2( mHistColL1RoIEtaL2ClusterEta, mColL1RoIEtaL2ClusterEta);
  fillTH2( mHistColL1RoIPhiL2ClusterPhi, mColL1RoIPhiL2ClusterPhi);
  fillTH2abs( mHistColL1RoIEtL2SiTrackPt, mColL1RoIEtL2SiTrackPt, 0.001, 0.001);
  fillTH2( mHistColL1RoIEtaL2SiTrackEta, mColL1RoIEtaL2SiTrackEta);
  fillTH2( mHistColL1RoIPhiL2SiTrackPhi, mColL1RoIPhiL2SiTrackPhi);
  fillTH2abs( mHistColL2ClusterEtL2SiTrackPt, mColL2ClusterEtL2SiTrackPt, 0.001, 0.001);
  fillTH2( mHistColL2ClusterEtaL2SiTrackEta, mColL2ClusterEtaL2SiTrackEta);
  fillTH2( mHistColL2ClusterPhiL2SiTrackPhi, mColL2ClusterPhiL2SiTrackPhi);
}

#endif // __TrigVarInElectronChain_cxx__
