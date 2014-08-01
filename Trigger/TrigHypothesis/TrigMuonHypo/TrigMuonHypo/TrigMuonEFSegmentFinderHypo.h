/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFSEGMENTFINDERHYPO_H 
#define TRIG_TRIGMUONEFSEGMENTFINDERHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFSegmentFinderHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFSegmentFinderHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFSegmentFinderHypo();
  
  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();
  
 private:
  BooleanProperty m_acceptAll;

  // Other members:   
  StoreGateSvc* m_storeGate;

  unsigned short int m_nseg;
};

#endif
