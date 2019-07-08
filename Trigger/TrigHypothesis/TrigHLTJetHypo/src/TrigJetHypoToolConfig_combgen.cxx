/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_combgen.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_combgen.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_combgen::TrigJetHypoToolConfig_combgen(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_combgen::~TrigJetHypoToolConfig_combgen(){
}

StatusCode TrigJetHypoToolConfig_combgen::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}




ConditionsMT TrigJetHypoToolConfig_combgen::getConditions() const {
  auto conditions = conditionsFactoryEtaEtMT(m_etaMins,
                                             m_etaMaxs,
                                             m_EtThresholds,
                                             m_asymmetricEtas);

  return conditions;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_combgen::getJetGrouper() const {
  return std::make_unique<CombinationsGrouper>(m_size);
}

StatusCode TrigJetHypoToolConfig_combgen::checkVals() const {
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
TrigJetHypoToolConfig_combgen::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_combgen::getMatcher () const {
  // method needed for interface. Combgen nneds no matcher
  return std::unique_ptr<IGroupsMatcherMT> (nullptr);
}
