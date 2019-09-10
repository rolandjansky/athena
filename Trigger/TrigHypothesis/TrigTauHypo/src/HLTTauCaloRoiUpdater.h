/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_HLTTAUCALOROIUPDATER_H
#define TRIGTAUHYPO_HLTTAUCALOROIUPDATER_H

#include "TrigInterfaces/FexAlgo.h"

class HLTTauCaloRoiUpdater : public HLT::FexAlgo 
{ 
    
 public:   

  HLTTauCaloRoiUpdater(const std::string &name, ISvcLocator *pSvcLocator); 
  ~HLTTauCaloRoiUpdater();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* outputTE);    
  HLT::ErrorCode hltFinalize();

 private:


  float m_dRForCenter;
  
};
#endif
