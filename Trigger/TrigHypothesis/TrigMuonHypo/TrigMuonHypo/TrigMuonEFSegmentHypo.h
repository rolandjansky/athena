/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFSEGMENTHYPO_H 
#define TRIG_TRIGMUONEFSEGMENTHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class TriggerElement;

class TrigMuonEFSegmentHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFSegmentHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFSegmentHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

 private:

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds;
  BooleanProperty m_acceptAll;
  BooleanProperty m_rejectCBmuons;

  // Other members:   
  std::vector<float>::size_type m_bins;
  std::vector<float> m_fex_pt;
  std::vector<float> m_fex_eta;
  std::vector<float> m_fex_phi;
};

#endif
