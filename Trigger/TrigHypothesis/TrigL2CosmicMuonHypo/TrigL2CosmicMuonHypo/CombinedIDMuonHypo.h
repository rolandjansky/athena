/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDMuonHypo_h__
#define __CombinedIDMuonHypo_h__
/*
  CombinedIDMuonHypo.h
*/
#include "TH1.h"
#include "TrigInterfaces/HypoAlgo.h"

class CombinedIDMuonHypo : public HLT::HypoAlgo {
public:
  CombinedIDMuonHypo(const std::string& name, ISvcLocator* svcloc);
  ~CombinedIDMuonHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

private:
  //------------------------------------------------------------
  // Configurable properties
  //------------------------------------------------------------
  
  /** Number of combined muon/track objects needed to pass hypo */
  int mCombIDMuNtracks;

  /** DeltaPhi cut between combined ID track and Muon */
  float mCombIDMu_DeltaPhi0Cut;

  /** Delta Eta cut between combined ID track and Muon */
  float mCombIDMu_DeltaEtaCut;

  /** Delta Theta cut between combined ID track and Muon */
  float mCombIDMu_DeltaThetaCut;

  // HLT Services
  /** storegate pointer */
  StoreGateSvc* mStoreGate;
  
  //------------------------------------------------------------

  // Monitoring histograms
  /** histogram of deltaPhi after hypo cuts */
  std::vector <float> mHistdPhi_Cut;

  /** histogram of deltaEta after hypo cuts */
  std::vector <float> mHistdEta_Cut;

  /** histogram of deltaTheta after hypo cuts */
  std::vector <float> mHistdTheta_Cut;
};

#endif // __CombinedIDMuonHypo_h__


