/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigMuonHypo/TrigL2DimuonMassHypo.h
 **
 **   Description: Hypothesis algorithm to be run at Level 2 after 
 **                muon finding algorithms
 **
 **   Author: C.Schiavi <Carlo.Schiavi@ge.infn.it>
 **
 **   Created:   Jul 27 2006
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2DimuonMassHypo_H 
#define TRIG_TrigL2DimuonMassHypo_H


#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;
class CombinedMuonFeature;

class TrigL2DimuonMassHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2DimuonMassHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DimuonMassHypo();
    
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    

  private:
    double invariantMass(const CombinedMuonFeature* mu1,const CombinedMuonFeature* mu2);

    StoreGateSvc* m_storeGate;
    

    //PROPERTIES:

    // Mass window cuts
    bool m_oppositeCharge;
    float m_lowerMassCut;
    float m_upperMassCut;

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;
};

#endif
