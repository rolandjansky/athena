/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInJetChain_h__
#define __TrigVarInJetChain_h__
/*
  TrigVarInJetChain.h
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"
#include <iostream>
#include <vector>
#include "TrigMenu.h"

class TrigVarInJetChain : public TrigVarInHlt {
public:
  TrigVarInJetChain(std::string chain_name="", bool ActiveOnly=false);
  ~TrigVarInJetChain();

  void define();


  void fill(std::vector<ChainEntry>& chains,
	    RoILinks roi,
	    const TrigMenu& menu);

  // Method to get Histograms
  TH1* getHistL1RoI4x4Et() {return mHistL1RoI4x4Et;}
  TH1* getHistL1RoI6x6Et() {return mHistL1RoI6x6Et;}
  TH1* getHistL1RoI8x8Et() {return mHistL1RoI8x8Et;}
  TH1* getHistL1RoIEta() {return mHistL1RoIEta;}
  TH1* getHistL1RoIPhi() {return mHistL1RoIPhi;}
  TH1* getHistL2JetEt() {return mHistL2JetEt;}
  TH1* getHistL2JetEta() {return mHistL2JetEta;}
  TH1* getHistL2JetPhi() {return mHistL2JetPhi;}
  TH2* getHistColL1RoI8x8EtL2JetEt() {return mHistColL1RoI8x8EtL2JetEt;}
  TH2* getHistColL1RoIEtaL2JetEta() {return mHistColL1RoIEtaL2JetEta;}
  TH2* getHistColL1RoIPhiL2JetPhi() {return mHistColL1RoIPhiL2JetPhi;}

private:
  // Histograms
  TH1* mHistL1RoI4x4Et;
  TH1* mHistL1RoI6x6Et;
  TH1* mHistL1RoI8x8Et;
  TH1* mHistL1RoIEta;
  TH1* mHistL1RoIPhi;
  TH1* mHistL2JetEt;
  TH1* mHistL2JetEta;
  TH1* mHistL2JetPhi;
  TH2* mHistColL1RoI8x8EtL2JetEt;
  TH2* mHistColL1RoIEtaL2JetEta;
  TH2* mHistColL1RoIPhiL2JetPhi;

  // Vectors to hold variables
  std::vector<float> mL1RoI4x4Et;
  std::vector<float> mL1RoI6x6Et;
  std::vector<float> mL1RoI8x8Et;
  std::vector<float> mL1RoIEta;
  std::vector<float> mL1RoIPhi;
  std::vector<float> mL2JetEt;
  std::vector<float> mL2JetEta;
  std::vector<float> mL2JetPhi;
  // Maps to hold variables
  std::multimap<float, float> mColL1RoI8x8EtL2JetEt;
  std::multimap<float, float> mColL1RoIEtaL2JetEta;
  std::multimap<float, float> mColL1RoIPhiL2JetPhi;
};


#endif // __TrigVarInJetChain_h__
