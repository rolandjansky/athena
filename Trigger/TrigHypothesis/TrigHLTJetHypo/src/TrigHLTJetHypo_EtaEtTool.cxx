/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_EtaEtTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "TrigHLTJetHypo_EtaEtTool.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"


TrigHLTJetHypo_EtaEtTool::TrigHLTJetHypo_EtaEtTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent):
  TrigHLTJetHypoTool(type, name, parent) {

  declareProperty("EtThresholds",   m_EtThresholds ); // Default: 40 GeV
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
  declareProperty("asymmetricEtas",   m_asymmetricEtas);
}


TrigHLTJetHypo_EtaEtTool::~TrigHLTJetHypo_EtaEtTool(){
}


Conditions TrigHLTJetHypo_EtaEtTool::getConditions() const {
  auto conditions = conditionsFactoryEtaEt(m_etaMins,
                                           m_etaMaxs,
                                           m_EtThresholds,
                                           m_asymmetricEtas);
  std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
  
  return conditions;
 }

 
std::unique_ptr<IJetGrouper> TrigHLTJetHypo_EtaEtTool::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

StatusCode TrigHLTJetHypo_EtaEtTool::checkVals() const {
  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_asymmetricEtas.size() != m_etaMaxs.size()){
    
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and eta min, max boundaries or asymmetric eta flags: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_asymmetricEtas.size() << " "
                  );
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigHLTJetHypo_EtaEtTool::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
} 
