/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInJetChain_cxx__
#define __TrigVarInJetChain_cxx__
/*
  TrigVarInJetChain.cxx
*/

#include "TrigVarInJetChain.h"

TrigVarInJetChain::TrigVarInJetChain(std::string chain_name,
				     bool ActiveOnly) :
  TrigVarInHlt(chain_name, ActiveOnly) {
}


TrigVarInJetChain::~TrigVarInJetChain(){
}



void TrigVarInJetChain::define(){
  const std::string hist_prefix=std::string("h_")+mChainName;
  std::string hname;
  std::string hist_postfix("");
  if (mActiveOnly) hist_postfix="_pass";

  // L1 Jet_RoI //
  hname = hist_prefix + "_l2jet_4x4et" + hist_postfix;
  mHistL1RoI4x4Et = new TH1F(hname.c_str(), "", 100, 0, 400);
  hname = hist_prefix + "_l2jet_6x6et" + hist_postfix;
  mHistL1RoI6x6Et = new TH1F(hname.c_str(), "", 100, 0, 400);
  hname = hist_prefix + "_l2jet_8x8et" + hist_postfix;
  mHistL1RoI8x8Et = new TH1F(hname.c_str(), "", 100, 0, 400);
  hname = hist_prefix + "_l2jet_eta" + hist_postfix;
  mHistL1RoIEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l2jet_phi" + hist_postfix;
  mHistL1RoIPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // L2 Jet //
  hname = hist_prefix + "_l2jet_et" + hist_postfix;
  mHistL2JetEt = new TH1F(hname.c_str(), "", 100, 0, 400);
  hname = hist_prefix + "_l2jet_eta" + hist_postfix;
  mHistL2JetEta = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);
  hname = hist_prefix + "_l2jet_phi" + hist_postfix;
  mHistL2JetPhi = new TH1F(hname.c_str(), "", 64, -3.2, 3.2);

  // correlation between L1RoI && L2Jet //
  hname = hist_prefix + "_col_l1roiL2Jet_8x8EtEt" + hist_postfix;
  mHistColL1RoI8x8EtL2JetEt = new TH2F(hname.c_str(), "", 100, 0, 100, 400, 0, 100);
  hname = hist_prefix + "_col_l1roiL2Jet_EtaEta" + hist_postfix;
  mHistColL1RoIEtaL2JetEta = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
  hname = hist_prefix + "_col_l1roiL2Jet_PhiPhi" + hist_postfix;
  mHistColL1RoIPhiL2JetPhi = new TH2F(hname.c_str(), "", 64, -3.2, 3.2, 64, -3.2, 3.2);
}

void TrigVarInJetChain::fill(std::vector<ChainEntry>& chains,
			     RoILinks roi,
			     const TrigMenu& menu) {
  // filling vector
  fillVar(mL1RoI4x4Et, chains, roi, "Jet_ROI", menu.L1JetRoI_4x4);
  fillVar(mL1RoI6x6Et, chains, roi, "Jet_ROI", menu.L1JetRoI_6x6);
  fillVar(mL1RoI8x8Et, chains, roi, "Jet_ROI", menu.L1JetRoI_8x8);
  fillVar(mL1RoIEta, chains, roi, "Jet_ROI", menu.L1JetRoI_eta);
  fillVar(mL1RoIPhi, chains, roi, "Jet_ROI", menu.L1JetRoI_phi);
  fillVar(mL2JetEt, chains, roi, "TrigT2Jet", menu.TrigT2Jet_et);
  fillVar(mL2JetEta, chains, roi, "TrigT2Jet", menu.TrigT2Jet_eta);
  fillVar(mL2JetPhi, chains, roi, "TrigT2Jet", menu.TrigT2Jet_phi);
  fillVarPair(mColL1RoI8x8EtL2JetEt, chains, roi, "Jet_ROI", "TrigT2Jet", menu.L1JetRoI_8x8, menu.TrigT2Jet_et);
  fillVarPair(mColL1RoIEtaL2JetEta, chains, roi, "Jet_ROI", "TrigT2Jet", menu.L1JetRoI_eta, menu.TrigT2Jet_eta);
  fillVarPair(mColL1RoIPhiL2JetPhi, chains, roi, "Jet_ROI", "TrigT2Jet", menu.L1JetRoI_phi, menu.TrigT2Jet_phi);
  // filling histograms
  fillTH1(mHistL1RoI4x4Et, mL1RoI4x4Et, 0.001);
  fillTH1(mHistL1RoI6x6Et, mL1RoI6x6Et, 0.001);
  fillTH1(mHistL1RoI8x8Et, mL1RoI8x8Et, 0.001);
  fillTH1(mHistL1RoIEta, mL1RoIEta);
  fillTH1(mHistL1RoIPhi, mL1RoIPhi);
  fillTH1(mHistL2JetEt, mL2JetEt, 0.001);
  fillTH1(mHistL2JetEta, mL2JetEta);
  fillTH1(mHistL2JetPhi, mL2JetPhi);
  fillTH2(mHistColL1RoI8x8EtL2JetEt, mColL1RoI8x8EtL2JetEt, 0.001, 0.001);
  fillTH2(mHistColL1RoIEtaL2JetEta, mColL1RoIEtaL2JetEta);
  fillTH2(mHistColL1RoIPhiL2JetPhi, mColL1RoIPhiL2JetPhi);
}

#endif // __TrigVarInJetChain_cxx__
