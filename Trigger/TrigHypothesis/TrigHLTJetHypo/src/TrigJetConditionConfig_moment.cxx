/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_moment.h"
#include "GaudiKernel/StatusCode.h"
#include "./MomentConditionMT.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_moment::TrigJetConditionConfig_moment(const std::string& type,
							     const std::string& name,
							     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_moment::initialize() {
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_moment::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<MomentConditionMT>(a2d(m_min),
					     a2d(m_max),
					     m_moment
					     );
}
				     

StatusCode TrigJetConditionConfig_moment::checkVals() const {
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_moment::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_moment::capacity() const {
  return getCondition()->capacity();
}
