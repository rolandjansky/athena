/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_MajorityConditionsSvc.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

using namespace SCT_ConditionsServices;

// Static folder name
// const std::string SCT_MajorityConditionsSvc::s_coolMajorityFolderName{"/SCT/DCS/MAJ"};

// Constructor
SCT_MajorityConditionsSvc::SCT_MajorityConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_overall{false},
  m_majorityFraction{0.9},
  m_condData{nullptr},
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
  if(not getCondData()) return false;

  if (m_overall) {
    return (m_condData->getMajorityState(OVERALL) and (m_condData->getHVFraction(OVERALL) > m_majorityFraction));
  } else {
    return ((m_condData->getMajorityState(BARREL) and 
             m_condData->getMajorityState(ECA)    and 
             m_condData->getMajorityState(ECC))                      and
	    (m_condData->getHVFraction(BARREL) > m_majorityFraction) and
            (m_condData->getHVFraction(ECA)    > m_majorityFraction) and
            (m_condData->getHVFraction(ECC)    > m_majorityFraction));
  }
}

// Is a barrel/endcap good?
bool SCT_MajorityConditionsSvc::isGood(int bec) {
  if(not getCondData()) return false;

  bool result{true};

  // Check numbering

  if (bec == bec_BARREL) {
    result = (m_condData->getMajorityState(BARREL) and (m_condData->getHVFraction(BARREL) > m_majorityFraction));
  } else if (bec == bec_ECC) { 
    result = (m_condData->getMajorityState(ECC)    and (m_condData->getHVFraction(ECC)    > m_majorityFraction));
  } else if (bec == bec_ECA) {
    result = (m_condData->getMajorityState(ECA)    and (m_condData->getHVFraction(ECA)    > m_majorityFraction));
  } else {
    ATH_MSG_WARNING("Unrecognised BEC " << bec);
  }

  return result;
}

// Is the information filled?
bool SCT_MajorityConditionsSvc::filled() const {
  if(not getCondData()) return false;
  return m_condData->isFilled();
}

bool SCT_MajorityConditionsSvc::getCondData() const {
  if(!m_condData) {
    SG::ReadCondHandle<SCT_MajorityCondData> condData{m_condKey};
    if((not condData.isValid()) or !(*condData)) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
      return false;
    }
    m_condData = *condData;
  }
  return true;
}
