/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_MajorityConditionsSvc.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

using namespace SCT_ConditionsServices;

// Static folder name
// const std::string SCT_MajorityConditionsSvc::s_coolMajorityFolderName{"/SCT/DCS/MAJ"};

// Constructor
SCT_MajorityConditionsSvc::SCT_MajorityConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_overall{false},
  m_majorityFraction{0.9},
  m_mutex{},
  m_cache{},
  m_condData{},
  m_condKey{std::string{"SCT_MajorityCondData"}}
 {
  declareProperty("UseOverall", m_overall);
  declareProperty("MajorityFraction", m_majorityFraction);
}

// Initialize
StatusCode SCT_MajorityConditionsSvc::initialize() {
  ATH_MSG_INFO("Initializing SCT_MajorityConditionsSvc");

  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_MajorityConditionsSvc::finalize() {
  ATH_MSG_INFO("Finalizing SCT_MajorityConditionsSvc");

  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_MajorityConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_DetectorLevelConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();

  return StatusCode::SUCCESS;
}

// Is the detector good?
bool SCT_MajorityConditionsSvc::isGood() {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_MajorityCondData* condData{getCondData(ctx)};
  if (!condData) return false;

  if (m_overall) {
    return (condData->getMajorityState(OVERALL) and (condData->getHVFraction(OVERALL) > m_majorityFraction));
  } else {
    return ((condData->getMajorityState(BARREL) and 
             condData->getMajorityState(ECA)    and 
             condData->getMajorityState(ECC))                      and
	    (condData->getHVFraction(BARREL) > m_majorityFraction) and
            (condData->getHVFraction(ECA)    > m_majorityFraction) and
            (condData->getHVFraction(ECC)    > m_majorityFraction));
  }
}

// Is a barrel/endcap good?
bool SCT_MajorityConditionsSvc::isGood(int bec) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_MajorityCondData* condData{getCondData(ctx)};
  if (!condData) return false;

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

// Is the information filled?
bool SCT_MajorityConditionsSvc::filled() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_MajorityCondData* condData{getCondData(ctx)};
  if (!condData) return false;
  return condData->isFilled();
}

const SCT_MajorityCondData* SCT_MajorityConditionsSvc::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_MajorityCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
