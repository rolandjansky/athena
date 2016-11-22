/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGEFDETAFEX_H
#define TRIG_TRIGEFDETAFEX_H

#include "TrigJetHypo/TrigDEtaFexBase.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

//using HLT::ErrorCode;
//using HLT::TEConstVec;

class TrigEFDEtaFex : public TrigDEtaFexBase{
  
public:
  TrigEFDEtaFex(const std::string &name, ISvcLocator* pSvcLocator) : TrigDEtaFexBase(name, pSvcLocator){};
  ~TrigEFDEtaFex(){};
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *te, 
                                 std::vector<const INavigable4Momentum*> &jets) {
    
    const JetCollection *efJets(0);
    HLT::ErrorCode status = this->HLT::Algo::getFeature(te, efJets);
    if(status != HLT::OK) return status;
    for(JetCollection::const_iterator jet = efJets->begin();
        jet != efJets->end(); ++jet){
      jets.push_back(dynamic_cast<const INavigable4Momentum*> (*jet));
    }
    return status;
  }
  
  virtual HLT::ErrorCode saveJets(HLT::TriggerElement *output, 
                                  std::vector<const INavigable4Momentum*> jets, 
                                  std::string label) {
    JetCollection *jetCol = new JetCollection(SG::VIEW_ELEMENTS);
    for(std::vector<const INavigable4Momentum*>::const_iterator jet = jets.begin();
        jet != jets.end(); ++jet){
      jetCol->push_back(const_cast<Jet*>(dynamic_cast<const Jet*>(*jet)));
    }
    return AllTEAlgo::attachFeature(output, jetCol, label);
  }
};
#endif
