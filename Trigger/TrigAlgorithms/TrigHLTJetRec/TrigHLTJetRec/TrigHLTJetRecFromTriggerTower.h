/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTJETRECFROMTRIGGERTOWER_H
#define TRIGHLTJETREC_TRIGHLTJETRECFROMTRIGGERTOWER_H

#include "TrigHLTJetRec/TrigHLTJetRecBase.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class TrigHLTJetRecFromTriggerTower:
public TrigHLTJetRecBase<xAOD::TriggerTowerContainer>{

 public:
  TrigHLTJetRecFromTriggerTower(const std::string & name,
                                ISvcLocator* pSvcLocator);
  ~TrigHLTJetRecFromTriggerTower();


  const xAOD::JetContainer* build() const override;

}; 
#endif
