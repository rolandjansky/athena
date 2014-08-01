/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUCOMBHYPO_H 
#define TRIG_MUCOMBHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class MucombHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    MucombHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MucombHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();


  private:

    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;

    BooleanProperty m_acceptAll;

    // pi/K rejections flags
    BooleanProperty m_pik_cuts;
    DoubleProperty  m_maxPtToApply_pik;
    DoubleProperty  m_chi2MaxID;

    // Apply strategy dependent pT cuts (B/T/S = 0 fields, L1 seeded, ...)
    BooleanProperty     m_strategydependent;
    DoubleArrayProperty m_strategyDependentPtCuts;

    // Other members:   
    StoreGateSvc* m_storeGate;

    std::vector<float>::size_type m_bins;
    float m_fex_pt;
    float m_id_eta;
    float m_id_phi;
    float m_id_Z0;
    float m_id_A0;
    float m_ptFL;
    int   m_Strategy;
};

#endif
