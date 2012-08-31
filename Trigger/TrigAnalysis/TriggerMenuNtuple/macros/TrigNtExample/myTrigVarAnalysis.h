/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __myTrigVarAnalysis_h__
#define __myTrigVarAnalysis_h__
/*
  myTrigVarAnalysis.h
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"
#include <iostream>
#include <vector>
#include "TrigMuon.h"

class myTrigVarAnalysis : public TrigVarInHlt {
public:
  myTrigVarAnalysis(std::string chain_name="", bool ActiveOnly=false);
  ~myTrigVarAnalysis();

  void define();
  void fill(std::vector<ChainEntry>& chains,
	    RoILinks roi,
	    const TrigMuon& muon);

  // Method to get Histograms
  TH1* getHistL1RoIPt() {return mHistL1RoIPt;}
  TH1* getHistL1RoIEta() {return mHistL1RoIEta;}
  TH1* getHistL1RoIPhi() {return mHistL1RoIPhi;}
  TH1* getHistMuFastPt() {return mHistMuFastPt;}
  TH1* getHistMuFastEta() {return mHistMuFastEta;}
  TH1* getHistMuFastPhi() {return mHistMuFastPhi;}
  TH2* getHistColMuFastPtMuCombPt() {return mHistColMuFastPtMuCombPt;}
  TH2* getHistColMuFastEtaMuCombEta() {return mHistColMuFastEtaMuCombEta;}
  TH2* getHistColMuFastPhiMuCombPhi() {return mHistColMuFastPhiMuCombPhi;}
private:
  // Histograms
  TH1* mHistL1RoIPt;
  TH1* mHistL1RoIEta;
  TH1* mHistL1RoIPhi;
  TH1* mHistMuFastPt;
  TH1* mHistMuFastEta;
  TH1* mHistMuFastPhi;
  TH2* mHistColMuFastPtMuCombPt;
  TH2* mHistColMuFastEtaMuCombEta;
  TH2* mHistColMuFastPhiMuCombPhi;
  // Vectors to hold variables
  std::vector<Float_t> mL1RoIPt;
  std::vector<Float_t> mL1RoIEta;
  std::vector<Float_t> mL1RoIPhi;
  std::vector<Float_t> mMuFastPt;
  std::vector<Float_t> mMuFastEta;
  std::vector<Float_t> mMuFastPhi;
  // Maps to hold variables
  std::multimap<Float_t, Float_t> mColMuFastPtMuCombPt;
  std::multimap<Float_t, Float_t> mColMuFastEtaMuCombEta;
  std::multimap<Float_t, Float_t> mColMuFastPhiMuCombPhi;
};



#endif // __myTrigVarAnalysis_h__
