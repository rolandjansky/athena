/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_simple.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_simple.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_simple::TrigJetHypoToolConfig_simple(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_simple::~TrigJetHypoToolConfig_simple(){
}

StatusCode TrigJetHypoToolConfig_simple::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}




ConditionsMT TrigJetHypoToolConfig_simple::getConditions() const {
  auto conditions = conditionsFactoryEtaEtMT(m_etaMins,
                                             m_etaMaxs,
                                             m_EtThresholds,
                                             m_asymmetricEtas);

  return conditions;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_simple::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_simple::getMatcher () const {
  return groupsMatcherFactoryMT_MaxBipartite(getConditions());
}

StatusCode TrigJetHypoToolConfig_simple::checkVals() const {
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
TrigJetHypoToolConfig_simple::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


