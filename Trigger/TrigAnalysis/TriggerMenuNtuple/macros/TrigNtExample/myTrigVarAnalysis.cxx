/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __myTrigVarAnalysis_cxx__
#define __myTrigVarAnalysis_cxx__
/*
  myTrigVarAnalysis.cxx
*/

#include "myTrigVarAnalysis.h"

myTrigVarAnalysis::myTrigVarAnalysis(std::string chain_name,
				       bool ActiveOnly) :
  TrigVarInHlt(chain_name, ActiveOnly),
  mHistL1RoIPt(), mHistL1RoIEta(), mHistL1RoIPhi(),
  mL1RoIPt(0), mL1RoIEta(0), mL1RoIPhi(0) {
}


myTrigVarAnalysis::~myTrigVarAnalysis(){
}



void myTrigVarAnalysis::define(){
  const std::string hist_prefix=std::string("h_")+mChainName;
  std::string hname;
  std::string hist_postfix("");
  if (mActiveOnly) hist_postfix="_pass";

  // L1 Muon_RoI //
  hname = hist_prefix + "_l1roi_pt" + hist_postfix;
  mHistL1RoIPt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_l1roi_eta" + hist_postfix;
  mHistL1RoIEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l1roi_phi" + hist_postfix;
  mHistL1RoIPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // MuFast //
  hname = hist_prefix + "_MuFast_pt" + hist_postfix;
  mHistMuFastPt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_MuFast_eta" + hist_postfix;
  mHistMuFastEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_MuFast_phi" + hist_postfix;
  mHistMuFastPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // correlation between MuFast && MuComb
  hname = hist_prefix + "_col_MuFastMuComb_PtPt" + hist_postfix;
  mHistColMuFastPtMuCombPt = new TH2F(hname.c_str(), "", 200, -100, 100, 200, -100, 100);
  hname = hist_prefix + "_col_MuFastMuComb_EtaEta" + hist_postfix;
  mHistColMuFastEtaMuCombEta = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  hname = hist_prefix + "_col_MuFastMuComb_PhiPhi" + hist_postfix;
  mHistColMuFastPhiMuCombPhi = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
}

void myTrigVarAnalysis::fill(std::vector<ChainEntry>& chains,
				  RoILinks roi,
				  const TrigMuon& muon) {
  // filling vector
  fillVar(mL1RoIPt, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_pt, muon.L1MuonRoI_N);
  fillVar(mL1RoIEta, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_eta, muon.L1MuonRoI_N);
  fillVar(mL1RoIPhi, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_phi, muon.L1MuonRoI_N);
  fillVar(mMuFastPt, chains, roi, "MuonFeature", muon.Mufast_MuFast_pt, muon.Mufast_N);
  fillVar(mMuFastEta, chains, roi, "MuonFeature", muon.Mufast_MuFast_eta, muon.Mufast_N);
  fillVar(mMuFastPhi, chains, roi, "MuonFeature", muon.Mufast_MuFast_phi, muon.Mufast_N);

  fillVarPair(mColMuFastPtMuCombPt, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_pt,muon.Mufast_N, muon.Mucomb_MuComb_pt, muon.Mucomb_N);
  fillVarPair(mColMuFastEtaMuCombEta, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_eta,muon.Mufast_N, muon.Mucomb_MuComb_eta, muon.Mucomb_N);
  fillVarPair(mColMuFastPhiMuCombPhi, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_phi,muon.Mufast_N, muon.Mucomb_MuComb_phi, muon.Mucomb_N);

  // filling histograms
  fillTH1( mHistL1RoIPt, mL1RoIPt, 0.001 );
  fillTH1( mHistL1RoIEta, mL1RoIEta );
  fillTH1( mHistL1RoIPhi, mL1RoIPhi );
  fillTH1abs( mHistMuFastPt, mMuFastPt );
  fillTH1( mHistMuFastEta, mMuFastEta );
  fillTH1( mHistMuFastPhi, mMuFastPhi );
  fillTH2( mHistColMuFastPtMuCombPt, mColMuFastPtMuCombPt, 1.0, 0.001);
  fillTH2( mHistColMuFastEtaMuCombEta, mColMuFastEtaMuCombEta );
  fillTH2( mHistColMuFastPhiMuCombPhi, mColMuFastPhiMuCombPhi );
}

#endif // __myTrigVarAnalysis_cxx__
