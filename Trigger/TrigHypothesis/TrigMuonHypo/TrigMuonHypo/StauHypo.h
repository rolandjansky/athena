/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_STAUHYPO_H 
#define TRIG_STAUHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class StauHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    StauHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~StauHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:

    // Properties:
    FloatProperty m_ptThreshold;
    FloatProperty m_betaMax;
    FloatProperty m_mMin;
    BooleanProperty m_acceptAll;

    // Other members:   
    StoreGateSvc* m_storeGate;
};

#endif

