/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_et.h"
#include "GaudiKernel/StatusCode.h"
#include "./EtCondition.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_et::TrigJetConditionConfig_et(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_et::initialize() {
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_et::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<EtCondition>(a2d(m_min));
}
				     

StatusCode TrigJetConditionConfig_et::checkVals() const {
  return StatusCode::SUCCESS;
}
