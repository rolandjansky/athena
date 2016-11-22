/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGL2DETAFEX_H
#define TRIG_TRIGL2DETAFEX_H

#include "TrigJetHypo/TrigDEtaFexBase.h"
#include "TrigCaloEvent/TrigT2Jet.h"
//using HLT::ErrorCode;
//using HLT::TEConstVec;

class ITrigTimerSvc;

class TrigL2DEtaFex : public TrigDEtaFexBase {
  
public:
  
  TrigL2DEtaFex(const std::string &name, ISvcLocator* pSvcLocator) : TrigDEtaFexBase(name, pSvcLocator){};
  ~TrigL2DEtaFex(){};
  
  
private:
  
  virtual HLT::ErrorCode getJets(const HLT::TriggerElement *te, 
                                 std::vector<const INavigable4Momentum*> &jets) {
   
    std::vector<const TrigT2Jet*> l2Jets;
    HLT::ErrorCode status = this->HLT::Algo::getFeatures<TrigT2Jet>(te, l2Jets);
    if(status != HLT::OK) return status;
    for(std::vector<const TrigT2Jet*>::const_iterator jet = l2Jets.begin();
        jet != l2Jets.end(); ++jet){
      jets.push_back( dynamic_cast<const INavigable4Momentum*> (*jet));
    }
    return status;
  }
  
  virtual HLT::ErrorCode saveJets(HLT::TriggerElement *output, 
                                  std::vector<const INavigable4Momentum*> jets, 
                                         std::string label) {
    
    for(std::vector<const INavigable4Momentum*>::const_iterator jet = jets.begin();
        jet != jets.end(); ++jet){
      
      HLT::ErrorCode status = AllTEAlgo::reAttachFeature(output, dynamic_cast<const TrigT2Jet*>(*jet), label);
      if(status != HLT::OK) return status;
    }
    return HLT::OK;
  }
  
};
#endif
