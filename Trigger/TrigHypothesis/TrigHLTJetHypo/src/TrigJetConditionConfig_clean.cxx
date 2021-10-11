/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_clean.h"
#include "GaudiKernel/StatusCode.h"
#include "./CleanCondition.h"


TrigJetConditionConfig_clean::TrigJetConditionConfig_clean(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_clean::initialize() {
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_clean::getCondition() const {
  return std::make_unique<CleanCondition>(m_cName);
}
				     

StatusCode TrigJetConditionConfig_clean::checkVals() const {
  return StatusCode::SUCCESS;
}

