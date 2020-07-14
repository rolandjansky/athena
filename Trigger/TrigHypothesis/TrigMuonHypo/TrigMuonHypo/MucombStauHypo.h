/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUCOMBSTAUHYPO_H 
#define TRIG_MUCOMBSTAUHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"

class StoreGateSvc;
class TriggerElement;

class MucombStauHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    MucombStauHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MucombStauHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();


  private:

    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;

    BooleanProperty m_pik_cuts;
    BooleanProperty m_acceptAll;
    FloatProperty m_betaMax;
    FloatProperty m_mMin;

    // Other members:   
    StoreGateSvc* m_storeGate;
    
    ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolator;
    
    std::vector<float>::size_type m_bins;
    float m_fex_pt;
    float m_id_eta;
    float m_id_phi;
    float m_id_Z0;
    float m_id_A0;
    float m_fex_mass;
    float m_id_mass;
    float m_fex_beta;
};

#endif
