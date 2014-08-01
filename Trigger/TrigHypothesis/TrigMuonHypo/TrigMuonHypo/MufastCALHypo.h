/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTCALHYPO_H 
#define TRIG_MUFASTCALHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class MufastCALHypo: public HLT::HypoAlgo {
  public:
    MufastCALHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MufastCALHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:
    
    // Properties:
    BooleanProperty m_acceptAll;
    
    // Other members:   
    StoreGateSvc* m_storeGate;
    
    float m_fex_pt;
    float m_fex_eta;
    float m_fex_phi;    
};

#endif
