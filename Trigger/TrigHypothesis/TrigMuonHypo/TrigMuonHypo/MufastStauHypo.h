/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTSTAUHYPO_H 
#define TRIG_MUFASTSTAUHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class MufastStauHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    MufastStauHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MufastStauHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:

    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;
    //FloatProperty m_ptThreshold;
    FloatProperty m_betaMax;
    FloatProperty m_mMin;
    BooleanProperty m_acceptAll;
    BooleanProperty m_etaCut;

    std::vector<float>::size_type m_bins;
    float m_fex_pt;
    float m_fex_eta;
    float m_fex_phi;
    float m_fex_beta;
    float m_fex_mass;
    
    // Other members:   
    StoreGateSvc* m_storeGate;
};

#endif

