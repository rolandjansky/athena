/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_MajorityConditionsTool.h"

#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ThreadLocalContext.h"

using namespace SCT_ConditionsData;

// Constructor
SCT_MajorityConditionsTool::SCT_MajorityConditionsTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
 {
  declareProperty("UseOverall", m_overall=false);
  declareProperty("MajorityFraction", m_majorityFraction=0.9);
}

// Initialize
StatusCode SCT_MajorityConditionsTool::initialize() {
  ATH_MSG_INFO("Initializing SCT_MajorityConditionsTool");

  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_MajorityConditionsTool::finalize() {
  ATH_MSG_INFO("Finalizing SCT_MajorityConditionsTool");

  return StatusCode::SUCCESS;
}

// Is the detector good?
bool SCT_MajorityConditionsTool::isGood(const EventContext& ctx) const {
  const SCT_MajorityCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return false;

  if (m_overall) {
    return (condData->getMajorityState(OVERALL) and (condData->getHVFraction(OVERALL) > m_majorityFraction));
  } else {
    return ((condData->getMajorityState(BARREL) and condData->getMajorityState(ECA) and condData->getMajorityState(ECC)) and
            (condData->getHVFraction(BARREL) > m_majorityFraction) and
            (condData->getHVFraction(ECA)    > m_majorityFraction) and
            (condData->getHVFraction(ECC)    > m_majorityFraction));
  }
}

bool SCT_MajorityConditionsTool::isGood() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(ctx);
}

// Is a barrel/endcap good?
bool SCT_MajorityConditionsTool::isGood(int bec, const EventContext& ctx) const {
  const SCT_MajorityCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return false;

  bool result{true};

  // Check numbering

  if (bec == bec_BARREL) {
    result = (condData->getMajorityState(BARREL) and (condData->getHVFraction(BARREL) > m_majorityFraction));
  } else if (bec == bec_ECC) { 
    result = (condData->getMajorityState(ECC)    and (condData->getHVFraction(ECC)    > m_majorityFraction));
  } else if (bec == bec_ECA) {
    result = (condData->getMajorityState(ECA)    and (condData->getHVFraction(ECA)    > m_majorityFraction));
  } else {
    ATH_MSG_WARNING("Unrecognised BEC " << bec);
  }

  return result;
}

bool SCT_MajorityConditionsTool::isGood(int bec) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(bec, ctx);
}

const SCT_MajorityCondData* SCT_MajorityConditionsTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_MajorityCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}
