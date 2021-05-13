/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for PT Condition
 */
#include "TrigJetConditionConfig_pt.h"
#include "GaudiKernel/StatusCode.h"
#include "./PtCondition.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_pt::TrigJetConditionConfig_pt(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_pt::initialize() {
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_pt::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<PtCondition>(a2d(m_min));
}
				     

StatusCode TrigJetConditionConfig_pt::checkVals() const {
  return StatusCode::SUCCESS;
}
