/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRecFromTriggerTower: Provide the xAOD::TriggerTower specific method
// needed by TruggerHLTJetRec_Base to for m jets from clusters.

#include "TrigHLTJetRec/TrigHLTJetRecFromTriggerTower.h"

TrigHLTJetRecFromTriggerTower::TrigHLTJetRecFromTriggerTower(const std::string& name, 
                                             ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::TriggerTowerContainer>(name, pSvcLocator) {}

TrigHLTJetRecFromTriggerTower::~TrigHLTJetRecFromTriggerTower(){}

HLT::ErrorCode TrigHLTJetRecFromTriggerTower::build(fastjet::ClusterSequence*& cs,
                                                    xAOD::JetContainer*& jc) const{
  return TrigHLTJetRecBase<xAOD::TriggerTowerContainer>::defaultBuild(cs, jc);
}
