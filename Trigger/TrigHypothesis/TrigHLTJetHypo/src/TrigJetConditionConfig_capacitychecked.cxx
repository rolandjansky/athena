/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_capacitychecked.h"
#include "CapacityCheckedCondition.h"
#include "GaudiKernel/StatusCode.h"
#include "./CompoundConditionMT.h"
#include <vector>


TrigJetConditionConfig_capacitychecked::TrigJetConditionConfig_capacitychecked(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_capacitychecked::initialize() {
  return StatusCode::SUCCESS;
}


ConditionPtr
TrigJetConditionConfig_capacitychecked::getCapacityCheckedCondition() const {
  std::vector<ConditionMT> elements;
  for(const auto& el : m_elementConditions){
    elements.push_back(el->getCondition());
  }
  
  auto cc =  std::make_unique<CompoundConditionMT>(elements);

  return std::make_unique<CapacityCheckedCondition>(std::move(cc),
						    m_multiplicity,

						    m_chainPartInd);
}
				     

StatusCode TrigJetConditionConfig_capacitychecked::checkVals() const {

  if (m_multiplicity < 1u) {
    ATH_MSG_ERROR("m_multiplicity = " + std::to_string(m_multiplicity) +
		  "expected > 0");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_capacitychecked::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_capacitychecked::capacity() const {
  return getCapacityCheckedCondition()->capacity();
}

