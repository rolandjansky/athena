/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUGIRLSTAUHYPO_H 
#define TRIG_TRIGMUGIRLSTAUHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuGirlStauHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuGirlStauHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuGirlStauHypo();

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
  //  std::vector<float> m_fex_pt;
  //  std::vector<float> m_fex_eta;
  //  std::vector<float> m_fex_phi;
};

#endif
