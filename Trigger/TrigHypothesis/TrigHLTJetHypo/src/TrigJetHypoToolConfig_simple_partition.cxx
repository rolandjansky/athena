/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_simple_partition.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_simple_partition.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_simple_partition::TrigJetHypoToolConfig_simple_partition(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}



StatusCode TrigJetHypoToolConfig_simple_partition::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


ConditionsMT TrigJetHypoToolConfig_simple_partition::getConditions() const {
  auto conditions = conditionsFactoryEtaEtMT(m_etaMins,
                                             m_etaMaxs,
                                             m_EtThresholds,
                                             m_asymmetricEtas);

  return conditions;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_simple_partition::getJetGrouper() const {

  // set up theP PartitionsGrouper according to the number of jets
  // required by each condition
  auto conditions = getConditions();
  std::vector<size_t> mults;
  for(const auto& c : conditions){mults.push_back(c.capacity());}
   
  return std::make_unique<PartitionsGrouper>(mults);
}


std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_simple_partition::getMatcher() const {
  return groupsMatcherFactoryMT_Partitions(getConditions());
}


StatusCode TrigJetHypoToolConfig_simple_partition::checkVals() const {
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
TrigJetHypoToolConfig_simple_partition::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


