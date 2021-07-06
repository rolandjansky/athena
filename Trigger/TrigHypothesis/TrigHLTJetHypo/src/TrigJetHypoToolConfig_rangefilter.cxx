/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Configurer for a HypoJetVector ConditionFilter
*/

#include "TrigJetHypoToolConfig_rangefilter.h"
#include "RangeFilter.h"

#include "GaudiKernel/StatusCode.h"
#include <vector>


TrigJetHypoToolConfig_rangefilter::TrigJetHypoToolConfig_rangefilter(const std::string& type,
								     const std::string& name,
								     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetHypoToolConfig_rangefilter::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}

FilterPtr
TrigJetHypoToolConfig_rangefilter::getHypoJetVectorFilter() const {
  /* create and return a RangeFilter with the configure range limits.*/

  FilterPtr fp = std::unique_ptr<IHypoJetVectorFilter>(nullptr);
  fp.reset(new RangeFilter(m_begin, m_end));

  return fp;
}


StatusCode TrigJetHypoToolConfig_rangefilter::checkVals() const {

  if (m_begin > m_end) {ATH_MSG_ERROR("RangeFilter begin > end");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



