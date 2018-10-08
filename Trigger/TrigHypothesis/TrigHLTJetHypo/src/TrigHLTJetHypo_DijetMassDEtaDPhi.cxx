/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_DijetMassDEtaDPhi.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_DijetMassDEtaDPhi.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"


TrigHLTJetHypo_DijetMassDEtaDPhi::TrigHLTJetHypo_DijetMassDEtaDPhi(
const std::string& name, ISvcLocator* pSvcLocator):
TrigHLTJetHypo_DijetMassDEta(name, pSvcLocator) {

  //Dimass dEta
  declareProperty("dPhi_mins", m_dPhiMins);
  declareProperty("dPhi_maxs", m_dPhiMaxs);
}

TrigHLTJetHypo_DijetMassDEtaDPhi::~TrigHLTJetHypo_DijetMassDEtaDPhi(){
}


 Conditions TrigHLTJetHypo_DijetMassDEtaDPhi::getConditions() const {

   auto conditions = TrigHLTJetHypo_DijetMassDEta::getConditions();
   auto n_conditions = conditionsFactoryDijetDPhi(m_dPhiMins, m_dPhiMaxs);

   conditions.insert(conditions.end(), 
                     n_conditions.begin(), 
                     n_conditions.end());
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
   return conditions;
 }
 

bool TrigHLTJetHypo_DijetMassDEtaDPhi::checkVals() const {

  if(!TrigHLTJetHypo_DijetMassDEta::checkVals()){return false;}

  auto ndphis = m_dPhiMaxs.size();
  bool dPhiOK = ndphis == 1;

  if (not dPhiOK){
    ATH_MSG_ERROR(name() << ": phi limit size error, expected 1, found "
                  << ndphis);
    ATH_MSG_ERROR(name() << " dPhi_mins " <<  m_dPhiMins.size());
    ATH_MSG_ERROR(name() << " dPhi_maxs " <<  m_dPhiMaxs.size());
  }
  
  return dPhiOK;
}

