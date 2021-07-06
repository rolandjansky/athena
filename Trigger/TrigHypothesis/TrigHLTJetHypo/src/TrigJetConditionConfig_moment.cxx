/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_moment.h"
#include "GaudiKernel/StatusCode.h"
#include "./MomentCondition.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_moment::TrigJetConditionConfig_moment(const std::string& type,
							     const std::string& name,
							     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_moment::initialize() {
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_moment::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<MomentCondition>(a2d(m_min),
					     a2d(m_max),
					     m_moment
					     );
}
				     

StatusCode TrigJetConditionConfig_moment::checkVals() const {
  return StatusCode::SUCCESS;
}

