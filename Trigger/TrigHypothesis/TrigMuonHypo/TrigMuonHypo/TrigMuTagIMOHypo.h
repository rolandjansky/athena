/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigMuTagIMOHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigMuonHypo
//
// AUTHOR:  Andrea Coccaro (AT ge DOT infn DOT it)
// 
// ************************************************

#ifndef TRIGMUTAGIMOHYPO_H
#define TRIGMUTAGIMOHYPO_H
 
#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuTagIMOHypo : public HLT::HypoAlgo {

 public:

  TrigMuTagIMOHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuTagIMOHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

 private:

  /** @brief Cut counter. */
  int m_cutCounter;

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds;
  std::vector<float>::size_type m_bins;

  bool m_acceptAll;
  float m_pt;
  float m_eta;
  float m_phi;

  //StoreGateSvc* m_storeGate;

};

#endif
