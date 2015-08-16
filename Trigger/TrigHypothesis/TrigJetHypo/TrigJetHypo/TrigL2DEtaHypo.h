/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2DETAHYPO_H
#define TRIGL2DETAHYPO_H

#include "TrigJetHypo/TrigDEtaHypoBase.h"

class TriggerElement;
class ISvcLocator;

class TrigL2DEtaHypo : public TrigDEtaHypoBase {
  
public:
  
  TrigL2DEtaHypo(const std::string &name, ISvcLocator *pSvcLocator) : 
  TrigDEtaHypoBase(name, pSvcLocator){};
  
  ~TrigL2DEtaHypo(){};
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *outputTE, std::vector<const INavigable4Momentum*> &jets){
    
    std::vector<const TrigT2Jet*> l2Jets;
    HLT::ErrorCode status = getFeatures(outputTE, l2Jets);
    if(status != HLT::OK) return status;
    
    for(std::vector<const TrigT2Jet*>::const_iterator jet = l2Jets.begin();
        jet != l2Jets.end(); ++jet){
      jets.push_back(dynamic_cast<const INavigable4Momentum*> (*jet));
    }
    return HLT::OK;
  }
  
};


#endif
