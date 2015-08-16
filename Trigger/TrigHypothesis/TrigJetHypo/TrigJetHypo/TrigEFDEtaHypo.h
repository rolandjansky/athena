/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFDETAHYPO_H
#define TRIGEFDETAHYPO_H

#include "TrigJetHypo/TrigDEtaHypoBase.h"

class TriggerElement;
class ISvcLocator;

class TrigEFDEtaHypo : public TrigDEtaHypoBase {
  
public:
  
  TrigEFDEtaHypo(const std::string &name, ISvcLocator *pSvcLocator) : 
  TrigDEtaHypoBase(name, pSvcLocator){};
  
  ~TrigEFDEtaHypo(){};
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *outputTE, std::vector<const INavigable4Momentum*> &jets){
    
    const JetCollection *efJets(0);
    HLT::ErrorCode status = getFeature(outputTE, efJets);
    if(status != HLT::OK) return status;
    
    for(JetCollection::const_iterator jet = efJets->begin();
        jet != efJets->end(); ++jet){
      jets.push_back(dynamic_cast<const INavigable4Momentum*> (*jet));
    }
    return HLT::OK;
  }
  
};


#endif
