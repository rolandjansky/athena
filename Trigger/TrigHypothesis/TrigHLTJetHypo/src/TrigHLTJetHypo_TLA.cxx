/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_TLA.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_TLA.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IndexedJetsGrouper.h"


TrigHLTJetHypo_TLA::TrigHLTJetHypo_TLA(const std::string& name,
			       ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {

  //TLA style jet indices
  declareProperty("ystar_mins", m_ystarMins);
  declareProperty("ystar_maxs", m_ystarMaxs);
  declareProperty("eta_mins", m_etaMins);
  declareProperty("eta_maxs", m_etaMaxs);
  declareProperty("mass_mins", m_massMins);
  declareProperty("mass_maxs", m_massMaxs);
  declareProperty("jetvec_indices",   m_jetvec_indices);
}

TrigHLTJetHypo_TLA::~TrigHLTJetHypo_TLA(){
}


Conditions TrigHLTJetHypo_TLA::getConditions() const {
   

   auto conditions = conditionsFactoryTLA(m_etaMins, m_etaMaxs,
                                          m_ystarMins, m_ystarMaxs,
                                          m_massMins, m_massMaxs);
   
   std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
   return conditions;
 }

 
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_TLA::getJetGrouper() const {
  return std::make_shared<IndexedJetsGrouper>(m_jetvec_indices);
}


bool TrigHLTJetHypo_TLA::checkVals() const {
  
  if (1 != m_etaMins.size() or
      1 != m_etaMaxs.size() or
      1 != m_ystarMins.size() or
      1 != m_ystarMaxs.size() or
      1 != m_massMins.size() or
      1 != m_massMaxs.size()){
      
      
    ATH_MSG_ERROR(name()
                  << ": size != 1: "
                  << "eta_mins" <<  m_etaMins.size() << " "
                  << "eta_maxs" <<  m_etaMaxs.size() << " "
                  << "ystar_mins" <<  m_ystarMins.size() << " "
                  << "ystar_maxs" <<  m_ystarMaxs.size() << " "
                  << "mass_mins" <<  m_massMins.size() << " "
                  << "mass_maxs" <<  m_massMaxs.size());
    return false;
  }
  return true;
}

std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_TLA::getCleaners () const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}
