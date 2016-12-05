/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec_Jet: Provide the xAOD::Jet specific method
// needed by TruggerHLTJetRec_Base to for m jets from jets.

#include "TrigHLTJetRec/TrigHLTJetRecFromJet.h"


TrigHLTJetRecFromJet::TrigHLTJetRecFromJet(const std::string& name, 
                                           ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::JetContainer>(name, pSvcLocator) {}

TrigHLTJetRecFromJet::~TrigHLTJetRecFromJet(){}

const xAOD::JetContainer* TrigHLTJetRecFromJet::build() const{
  return TrigHLTJetRecBase<xAOD::JetContainer>::defaultBuild();
}
