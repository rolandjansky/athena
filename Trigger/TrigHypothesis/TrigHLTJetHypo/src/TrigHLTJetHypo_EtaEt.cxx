/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_EtaEt.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_EtaEt.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"


TrigHLTJetHypo_EtaEt::TrigHLTJetHypo_EtaEt(const std::string& name,
			       ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {

  declareProperty("EtThresholds",   m_EtThresholds ); // Default: 40 GeV
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
  declareProperty("asymmetricEtas",   m_asymmetricEtas);
}


TrigHLTJetHypo_EtaEt::~TrigHLTJetHypo_EtaEt(){
}


Conditions TrigHLTJetHypo_EtaEt::getConditions() const {
   auto conditions = conditionsFactoryEtaEt(m_etaMins,
                                            m_etaMaxs,
                                            m_EtThresholds,
                                            m_asymmetricEtas);
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
   
   return conditions;
 }

 
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_EtaEt::getJetGrouper() const {
  return std::make_shared<SingleJetGrouper>();
}

bool TrigHLTJetHypo_EtaEt::checkVals() const {
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
    
    return false;
  }
  return true;
}

std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_EtaEt::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
} 
