/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInMuonChain_h__
#define __TrigVarInMuonChain_h__
/*
  TrigVarInMuonChain.h
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"
#include <iostream>
#include <vector>
#include "TrigMuon.h"

class TrigVarInMuonChain : public TrigVarInHlt {
public:
  TrigVarInMuonChain(std::string chain_name="", bool ActiveOnly=false);
  ~TrigVarInMuonChain();

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
  TH1* getHistMuCombPt() {return mHistMuCombPt;}
  TH1* getHistMuCombEta() {return mHistMuCombEta;}
  TH1* getHistMuCombPhi() {return mHistMuCombPhi;}
  TH2* getHistColL1RoIPtMuFastPt() {return mHistColL1RoIPtMuFastPt;}
  TH2* getHistColL1RoIEtaMuFastEta() {return mHistColL1RoIEtaMuFastEta;}
  TH2* getHistColL1RoIPhiMuFastPhi() {return mHistColL1RoIPhiMuFastPhi;}
  TH2* getHistColL1RoIPtMuCombPt() {return  mHistColL1RoIPtMuCombPt;}
  TH2* getHistColL1RoIEtaMuCombEta() {return mHistColL1RoIEtaMuCombEta;}
  TH2* getHistColL1RoIPhiMuCombPhi() {return mHistColL1RoIPhiMuCombPhi;}
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
  TH1* mHistMuCombPt;
  TH1* mHistMuCombEta;
  TH1* mHistMuCombPhi;
  TH2* mHistColL1RoIPtMuFastPt;
  TH2* mHistColL1RoIEtaMuFastEta;
  TH2* mHistColL1RoIPhiMuFastPhi;
  TH2* mHistColL1RoIPtMuCombPt;
  TH2* mHistColL1RoIEtaMuCombEta;
  TH2* mHistColL1RoIPhiMuCombPhi;
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
  std::vector<Float_t> mMuCombPt;
  std::vector<Float_t> mMuCombEta;
  std::vector<Float_t> mMuCombPhi;
  // Maps to hold variables
  std::multimap<Float_t, Float_t> mColL1RoIPtMuFastPt;
  std::multimap<Float_t, Float_t> mColL1RoIEtaMuFastEta;
  std::multimap<Float_t, Float_t> mColL1RoIPhiMuFastPhi;
  std::multimap<Float_t, Float_t> mColL1RoIPtMuCombPt;
  std::multimap<Float_t, Float_t> mColL1RoIEtaMuCombEta;
  std::multimap<Float_t, Float_t> mColL1RoIPhiMuCombPhi;
  std::multimap<Float_t, Float_t> mColMuFastPtMuCombPt;
  std::multimap<Float_t, Float_t> mColMuFastEtaMuCombEta;
  std::multimap<Float_t, Float_t> mColMuFastPhiMuCombPhi;
};



#endif // __TrigVarInMuonChain_h__
