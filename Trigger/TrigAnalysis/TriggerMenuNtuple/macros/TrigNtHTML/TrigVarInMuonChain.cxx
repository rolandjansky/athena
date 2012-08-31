/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInMuonChain_cxx__
#define __TrigVarInMuonChain_cxx__
/*
  TrigVarInMuonChain.cxx
*/

#include "TrigVarInMuonChain.h"

TrigVarInMuonChain::TrigVarInMuonChain(std::string chain_name,
				       bool ActiveOnly) :
  TrigVarInHlt(chain_name, ActiveOnly),
  mHistL1RoIPt(), mHistL1RoIEta(), mHistL1RoIPhi(),
  mL1RoIPt(0), mL1RoIEta(0), mL1RoIPhi(0) {
}


TrigVarInMuonChain::~TrigVarInMuonChain(){
}



void TrigVarInMuonChain::define(){
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

  // MuComb //
  hname = hist_prefix + "_MuComb_pt" + hist_postfix;
  mHistMuCombPt = new TH1F(hname.c_str(), "", 100, 0, 100);
  hname = hist_prefix + "_MuComb_eta" + hist_postfix;
  mHistMuCombEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_MuComb_phi" + hist_postfix;
  mHistMuCombPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // correlation between L1RoI && MuFast
  hname = hist_prefix + "_col_l1roiMuFast_PtPt" + hist_postfix;
  mHistColL1RoIPtMuFastPt = new TH2F(hname.c_str(), "", 100, 0, 100, 100, 0, 100);
  hname = hist_prefix + "_col_l1roiMuFast_EtaEta" + hist_postfix;
  mHistColL1RoIEtaMuFastEta = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  hname = hist_prefix + "_col_l1roiMuFast_PhiPhi" + hist_postfix;
  mHistColL1RoIPhiMuFastPhi = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);

  // correlation between L1RoI && MuComb
  hname = hist_prefix + "_col_l1roiMuComb_PtPt" + hist_postfix;
  mHistColL1RoIPtMuCombPt = new TH2F(hname.c_str(), "", 100, 0, 100, 100, 0, 100);
  hname = hist_prefix + "_col_l1roiMuComb_EtaEta" + hist_postfix;
  mHistColL1RoIEtaMuCombEta = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  hname = hist_prefix + "_col_l1roiMuComb_PhiPhi" + hist_postfix;
  mHistColL1RoIPhiMuCombPhi = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);

  // correlation between MuFast && MuComb
  hname = hist_prefix + "_col_MuFastMuComb_PtPt" + hist_postfix;
  mHistColMuFastPtMuCombPt = new TH2F(hname.c_str(), "", 200, -100, 100, 200, -100, 100);
  hname = hist_prefix + "_col_MuFastMuComb_EtaEta" + hist_postfix;
  mHistColMuFastEtaMuCombEta = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  hname = hist_prefix + "_col_MuFastMuComb_PhiPhi" + hist_postfix;
  mHistColMuFastPhiMuCombPhi = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
}


void TrigVarInMuonChain::fill(std::vector<ChainEntry>& chains,
			      RoILinks roi,
			      const TrigMuon& muon) {
  // filling vector
  fillVar(mL1RoIPt, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_pt, (int)muon.L1MuonRoI_);
  fillVar(mL1RoIEta, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_eta, (int)muon.L1MuonRoI_);
  fillVar(mL1RoIPhi, chains, roi, "Muon_ROI", muon.L1MuonRoI_L1_phi, (int)muon.L1MuonRoI_);
  fillVar(mMuFastPt, chains, roi, "MuonFeature", muon.Mufast_MuFast_pt, (int)muon.Mufast_);
  fillVar(mMuFastEta, chains, roi, "MuonFeature", muon.Mufast_MuFast_eta, (int)muon.Mufast_);
  fillVar(mMuFastPhi, chains, roi, "MuonFeature", muon.Mufast_MuFast_phi, (int)muon.Mufast_);
  fillVar(mMuCombPt, chains, roi, "CombinedMuonFeature", muon.Mucomb_MuComb_pt, (int)muon.Mucomb_);
  fillVar(mMuCombEta, chains, roi, "CombinedMuonFeature", muon.Mucomb_MuComb_eta, (int)muon.Mucomb_);
  fillVar(mMuCombPhi, chains, roi, "CombinedMuonFeature", muon.Mucomb_MuComb_phi, (int)muon.Mucomb_);

  fillVarPair(mColL1RoIPtMuFastPt, chains, roi, "Muon_ROI", "MuonFeature", 
	      muon.L1MuonRoI_L1_pt,(int)muon.L1MuonRoI_, muon.Mufast_MuFast_pt, (int)muon.Mufast_);
  fillVarPair(mColL1RoIEtaMuFastEta, chains, roi, "Muon_ROI", "MuonFeature", 
	      muon.L1MuonRoI_L1_eta,(int)muon.L1MuonRoI_, muon.Mufast_MuFast_eta, (int)muon.Mufast_);
  fillVarPair(mColL1RoIPhiMuFastPhi, chains, roi, "Muon_ROI", "MuonFeature", 
	      muon.L1MuonRoI_L1_phi,(int)muon.L1MuonRoI_, muon.Mufast_MuFast_phi, (int)muon.Mufast_);

  fillVarPair(mColL1RoIPtMuCombPt, chains, roi, "Muon_ROI", "CombinedMuonFeature", 
	      muon.L1MuonRoI_L1_pt,(int)muon.L1MuonRoI_, muon.Mucomb_MuComb_pt, (int)muon.Mucomb_);
  fillVarPair(mColL1RoIEtaMuCombEta, chains, roi, "Muon_ROI", "CombinedMuonFeature", 
	      muon.L1MuonRoI_L1_eta,(int)muon.L1MuonRoI_, muon.Mucomb_MuComb_eta, (int)muon.Mucomb_);
  fillVarPair(mColL1RoIPhiMuCombPhi, chains, roi, "Muon_ROI", "CombinedMuonFeature", 
	      muon.L1MuonRoI_L1_phi,(int)muon.L1MuonRoI_, muon.Mucomb_MuComb_phi, (int)muon.Mucomb_);

  fillVarPair(mColMuFastPtMuCombPt, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_pt,(int)muon.Mufast_, muon.Mucomb_MuComb_pt, (int)muon.Mucomb_);
  fillVarPair(mColMuFastEtaMuCombEta, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_eta,(int)muon.Mufast_, muon.Mucomb_MuComb_eta, (int)muon.Mucomb_);
  fillVarPair(mColMuFastPhiMuCombPhi, chains, roi, "MuonFeature", "CombinedMuonFeature", 
	      muon.Mufast_MuFast_phi,(int)muon.Mufast_, muon.Mucomb_MuComb_phi, (int)muon.Mucomb_);



  
  // filling histograms
  fillTH1( mHistL1RoIPt, mL1RoIPt, 0.001 );
  fillTH1( mHistL1RoIEta, mL1RoIEta );
  fillTH1( mHistL1RoIPhi, mL1RoIPhi );
  fillTH1abs( mHistMuFastPt, mMuFastPt );
  fillTH1( mHistMuFastEta, mMuFastEta );
  fillTH1( mHistMuFastPhi, mMuFastPhi );
  fillTH1abs( mHistMuCombPt, mMuCombPt, 0.001 );
  fillTH1( mHistMuCombEta, mMuCombEta );
  fillTH1( mHistMuCombPhi, mMuCombPhi );
  fillTH2abs( mHistColL1RoIPtMuFastPt, mColL1RoIPtMuFastPt, 0.001, 1.0 );
  fillTH2( mHistColL1RoIEtaMuFastEta, mColL1RoIEtaMuFastEta );
  fillTH2( mHistColL1RoIPhiMuFastPhi, mColL1RoIPhiMuFastPhi );
  fillTH2abs( mHistColL1RoIPtMuCombPt, mColL1RoIPtMuCombPt, 0.001, 0.001);
  fillTH2( mHistColL1RoIEtaMuCombEta, mColL1RoIEtaMuCombEta );
  fillTH2( mHistColL1RoIPhiMuCombPhi, mColL1RoIPhiMuCombPhi );
  fillTH2( mHistColMuFastPtMuCombPt, mColMuFastPtMuCombPt, 1.0, 0.001);
  fillTH2( mHistColMuFastEtaMuCombEta, mColMuFastEtaMuCombEta );
  fillTH2( mHistColMuFastPhiMuCombPhi, mColMuFastPhiMuCombPhi );
}

#endif // __TrigVarInMuonChain_cxx__
