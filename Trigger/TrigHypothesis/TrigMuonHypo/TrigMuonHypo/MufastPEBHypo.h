/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTPEBHYPO_H 
#define TRIG_MUFASTPEBHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class MufastPEBHypo: public HLT::HypoAlgo {
  public:
    MufastPEBHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MufastPEBHypo();
    
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
