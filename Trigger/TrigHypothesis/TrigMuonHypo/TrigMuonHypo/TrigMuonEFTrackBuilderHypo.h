/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFTRACKBUILDERHYPO_H 
#define TRIG_TRIGMUONEFTRACKBUILDERHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFTrackBuilderHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFTrackBuilderHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFTrackBuilderHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

 private:

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds;
  BooleanProperty m_acceptAll;

  // Other members:   
  StoreGateSvc* m_storeGate;

  std::vector<float>::size_type m_bins;
  std::vector<float> m_fex_pt;
  std::vector<float> m_fex_eta;
  std::vector<float> m_fex_phi;
};

#endif
