/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for a Repeated Condition
 */
#include "./TrigJetConditionConfig_repeated.h"
#include "RepeatedCondition.h"
#include "CompoundCondition.h"

#include "GaudiKernel/StatusCode.h"
#include <vector>


TrigJetConditionConfig_repeated::TrigJetConditionConfig_repeated(const std::string& type,
								 const std::string& name,
								 const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_repeated::initialize() {
  ATH_MSG_INFO("initialising " << name());
  return StatusCode::SUCCESS;
}

std::unique_ptr<ICondition>
TrigJetConditionConfig_repeated::getCompoundCondition() const {
  std::vector<Condition> elements;
  for(const auto& el : m_elementConditions){
    
    auto cond = el->getCondition();
    if (cond != nullptr) {
      elements.push_back(std::move(cond));
    }
  }
  
  return std::make_unique<CompoundCondition>(elements);
}

ConditionPtr
TrigJetConditionConfig_repeated::getRepeatedCondition() const {

  if (m_elementConditions.empty()) {return ConditionPtr(nullptr);}

  return
    std::make_unique<RepeatedCondition>(getCompoundCondition(),
					m_multiplicity,
					m_clique,
					m_chainPartInd,
					m_invert);
}


StatusCode TrigJetConditionConfig_repeated::checkVals() const {

  if (m_multiplicity < 1u) {
    ATH_MSG_ERROR("m_multiplicity = " + std::to_string(m_multiplicity) +
		  "expected > 0");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
