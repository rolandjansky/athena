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


std::optional<ConditionsMT>
TrigJetHypoToolConfig_simple_partition::getConditions() const {
  
  return
    std::make_optional<ConditionsMT>(conditionsFactoryEtaEtMT(m_etaMins,
							      m_etaMaxs,
							      m_EtThresholds,
							      m_asymmetricEtas));
}

std::size_t
TrigJetHypoToolConfig_simple_partition::requiresNJets() const {
  std::size_t result{0};
  auto opt_conds = getConditions();
  if(!opt_conds.has_value()){return result;}

  for(const auto& c : *opt_conds){result += c->capacity();}

  return result;
}
 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_simple_partition::getJetGrouper() const {

  // set up theP PartitionsGrouper according to the number of jets
  // required by each condition
  auto opt_conds = getConditions();
  if(!opt_conds.has_value()){
    ATH_MSG_ERROR("Error obtaining Conditions");
    return std::unique_ptr<IJetGrouper>(nullptr);
  }

  std::vector<size_t> mults;
  for(const auto& c : *opt_conds){mults.push_back(c->capacity());}
   
  return std::make_unique<PartitionsGrouper>(mults);
}


std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_simple_partition::getMatcher() const {
  
  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IGroupsMatcherMT>(nullptr);
  }
  
  return groupsMatcherFactoryMT_Partitions(std::move(*opt_conds));
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

  if (m_EtThresholds.empty()){
    ATH_MSG_ERROR(name() + " No conditions");
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_simple_partition::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


