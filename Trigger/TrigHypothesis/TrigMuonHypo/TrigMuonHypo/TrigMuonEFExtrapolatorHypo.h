/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFEXTRAPOLATORHYPO_H 
#define TRIG_TRIGMUONEFEXTRAPOLATORHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFExtrapolatorHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFExtrapolatorHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFExtrapolatorHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

 private:

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds; 
  BooleanProperty m_acceptAll;
  BooleanProperty m_msonlyCut;

  // Other members:   
  StoreGateSvc* m_storeGate;
  std::vector<float>::size_type m_bins;
  std::vector<float> m_fex_pt;
  std::vector<float> m_fex_eta;
  std::vector<float> m_fex_phi;

};

#endif
