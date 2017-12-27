/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTOTRHYPO_H 
#define TRIG_MUFASTOTRHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigMuonRoITools/ITrigMuonRoITool.h"

class StoreGateSvc;
class TriggerElement;

class MufastOTRHypo: public HLT::HypoAlgo {
  public:
    MufastOTRHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MufastOTRHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:
  
    std::pair<unsigned int, int> getRoIFeature(uint32_t);
  
    ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool;
    
    // Properties:
    BooleanProperty m_acceptAll;
    IntegerProperty m_segments;
    
    // Other members:   
    StoreGateSvc* m_storeGate;
    
    std::vector<int> m_RpcOutOfTimeIn;
    std::vector<int> m_TgcOutOfTimeIn;
    std::vector<int> m_RpcOutOfTimeOut;
    std::vector<int> m_TgcOutOfTimeOut;
    
    std::vector<float> m_fex_pt;
    std::vector<float> m_fex_eta;
    std::vector<float> m_fex_phi;    
};

#endif
