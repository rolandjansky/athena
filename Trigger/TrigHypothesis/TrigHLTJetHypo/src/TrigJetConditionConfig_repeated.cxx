/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_repeated.h"
#include "RepeatedCondition.h"
#include "ConditionInverter.h"
#include "CompoundConditionMT.h"

#include "GaudiKernel/StatusCode.h"
#include <vector>


TrigJetConditionConfig_repeated::TrigJetConditionConfig_repeated(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_repeated::initialize() {
  return StatusCode::SUCCESS;
}

std::unique_ptr<IConditionMT>
TrigJetConditionConfig_repeated::getCompoundCondition() const {
  std::vector<ConditionMT> elements;
  for(const auto& el : m_elementConditions){
    elements.push_back(el->getCondition());
  }
  
  return std::make_unique<CompoundConditionMT>(elements);
}

ConditionPtr
TrigJetConditionConfig_repeated::getRepeatedCondition() const {

  if (m_elementConditions.empty()) {return ConditionPtr(nullptr);}

  return
    std::make_unique<RepeatedCondition>(getCompoundCondition(),
					       m_multiplicity,
					       m_chainPartInd);
}

ConditionPtr
TrigJetConditionConfig_repeated::getRepeatedAntiCondition() const {
  auto acc = std::make_unique<ConditionInverterMT>(getCompoundCondition());
  return std::make_unique<RepeatedCondition>(std::move(acc),
						    m_multiplicity,
						    m_chainPartInd);
}
  


StatusCode TrigJetConditionConfig_repeated::checkVals() const {

  if (m_multiplicity < 1u) {
    ATH_MSG_ERROR("m_multiplicity = " + std::to_string(m_multiplicity) +
		  "expected > 0");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_repeated::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_repeated::capacity() const {
  return getRepeatedCondition()->capacity();
}

