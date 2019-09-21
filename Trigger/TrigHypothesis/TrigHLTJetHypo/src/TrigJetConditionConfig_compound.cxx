/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_compound.h"
#include "GaudiKernel/StatusCode.h"
#include "./CompoundConditionMT.h"
#include <vector>


TrigJetConditionConfig_compound::TrigJetConditionConfig_compound(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_compound::initialize() {
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_compound::getCondition() const {
  std::vector<ConditionMT> elements;
  for(const auto& el : m_elementConditions){
    elements.push_back(el->getCondition());
  }
  
  return std::make_unique<CompoundConditionMT>(elements);
}
				     

StatusCode TrigJetConditionConfig_compound::checkVals() const {
  return StatusCode::SUCCESS;
}

