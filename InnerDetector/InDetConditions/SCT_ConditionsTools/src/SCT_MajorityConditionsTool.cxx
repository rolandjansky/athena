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
  base_class(type, name, parent),
  m_overall{false},
  m_majorityFraction{0.9},
  m_mutex{},
  m_cache{},
  m_condData{}
 {
  declareProperty("UseOverall", m_overall);
  declareProperty("MajorityFraction", m_majorityFraction);
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
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cache.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_MajorityCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
