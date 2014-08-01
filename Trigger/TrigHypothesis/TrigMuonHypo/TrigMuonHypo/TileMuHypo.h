/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TileMuHYPO_H 
#define TRIG_TileMuHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TileMuHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    TileMuHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TileMuHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:

    // Properties:
    BooleanProperty m_acceptAll;
    IntegerProperty m_IOptIDScan;
    //BooleanProperty m_UseIDScan;
    //FloatProperty   m_DelPhi_Cut;
    //FloatProperty   m_DelEta_Cut;
    FloatProperty   m_Pt_Cut;

    // Other members:   
    StoreGateSvc* m_storeGate;

    float m_hypoEta;
    float m_hypoPhi;
    float m_hypoEffwTrk;
    float m_hypoEtaTrk;
    float m_hypoPhiTrk;
    float m_hypoPtTrk;
    float m_hypo_absPtTrk;
    float m_hypoDelEtaTrk;
    float m_hypoDelEtaTrkTR;
    float m_hypoDelPhiTrk;
    float m_hypoDelPhiTrkTR;
    float m_hypo_absPtTrk_Pre;

};

#endif
