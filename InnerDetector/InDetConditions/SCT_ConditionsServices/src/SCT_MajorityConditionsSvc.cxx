/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_MajorityConditionsSvc.h"

// STL includes
#include <algorithm>
#include <iostream>

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

using namespace SCT_ConditionsServices;

// Static folder name
const std::string SCT_MajorityConditionsSvc::s_coolMajorityFolderName{"/SCT/DCS/MAJ"};

// Constructor
SCT_MajorityConditionsSvc::SCT_MajorityConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_filled{false},
  m_detStore{"DetectorStore", name},
  m_IOVSvc{"IOVSvc", name},
  m_overall{false},
  m_majorityFraction{0.9} {
  declareProperty("UseOverall", m_overall);
  declareProperty("MajorityFraction", m_majorityFraction);
}

// Initialize
StatusCode SCT_MajorityConditionsSvc::initialize() {
  ATH_MSG_INFO("Initializing SCT_MajorityConditionsSvc");

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Detector service  not found !");
    return StatusCode::FAILURE;
  }

  // Retrieve IOV service
  if (m_IOVSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve IOVSvc");
    return StatusCode::FAILURE;
  }

  // Register callbacks for folders 
  if (m_detStore->regFcn(&SCT_MajorityConditionsSvc::fillData, this, m_dataMajority, s_coolMajorityFolderName).isFailure()) {
    ATH_MSG_ERROR("Failed to register callback");
    return StatusCode::FAILURE;
  }

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
  if (m_overall) {
    return (m_majorityState[OVERALL] and (m_hvFraction[OVERALL] > m_majorityFraction));
  } else {
    return ((m_majorityState[BARREL] and m_majorityState[ECA] and m_majorityState[ECC]) and
	    (m_hvFraction[BARREL] > m_majorityFraction) and
            (m_hvFraction[ECA]    > m_majorityFraction) and
            (m_hvFraction[ECC]    > m_majorityFraction));
  }
}

// Is a barrel/endcap good?
bool SCT_MajorityConditionsSvc::isGood(int bec) {
  bool result{true};

  // Check numbering

  if (bec == 0) {
    result = (m_majorityState[BARREL] and (m_hvFraction[BARREL] > m_majorityFraction));
  } else if (bec == -2) { 
    result = (m_majorityState[ECC]    and (m_hvFraction[ECC]    > m_majorityFraction));
  } else if (bec == +2) {
    result = (m_majorityState[ECA]    and (m_hvFraction[ECA]    > m_majorityFraction));
  } else {
    ATH_MSG_WARNING("Unrecognised BEC " << bec);
  }

  return result;
}


// Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_MajorityConditionsSvc::fillData(int& /*i*/, std::list<std::string>& /*l*/) {
  int numFilled{0};

  // Get Majority folder
  if (retrieveFolder(m_dataMajority, s_coolMajorityFolderName).isFailure()) {
    ATH_MSG_FATAL("Could not fill majority data");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("fillMajorityData: IOV callback resulted in a Chip CondAttrListCollection of size " << m_dataMajority->size());
  }

  CondAttrListCollection::const_iterator majItr{m_dataMajority->begin()};
  CondAttrListCollection::const_iterator majEnd{m_dataMajority->end()};
   
  for (;majItr != majEnd; ++majItr) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{(*majItr).first};
    CondAttrListCollection::AttributeList payload{(*majItr).second};
    
    // Possible components
    if ((channelNumber == OVERALL) or (channelNumber == BARREL) or (channelNumber == ECA) or (channelNumber == ECC)) {
      // Reset default to true at callback
      m_majorityState[channelNumber] = true;
      m_hvFraction[channelNumber]    = 1.;

      // Majority state
      if (not payload[INDEX_MajorityState].isNull()) {
	ATH_MSG_DEBUG("Majority state for " << channelNumber << " = " << payload[INDEX_MajorityState].data<int>());
	m_majorityState[channelNumber] = (payload[INDEX_MajorityState].data<int>() == HighAndLowVoltageOK);
      }

      // HV fraction in majority state (>50% by definition) IF majority state is HV and LV on
      if (m_majorityState[channelNumber] and (not payload[INDEX_HVfraction].isNull())) {
	ATH_MSG_DEBUG("Majority HV fraction for " << channelNumber << " = " << payload[INDEX_HVfraction].data<float>());
	m_hvFraction[channelNumber] = payload[INDEX_HVfraction].data<float>();
	numFilled++;
      }

    } else {
      ATH_MSG_WARNING("Unexpected channel number " << channelNumber);
    }
  }

  // Has data been filled
  m_filled = (numFilled==N_REGIONS ? true : false);

  return StatusCode::SUCCESS;
}

// Is the information filled?
bool SCT_MajorityConditionsSvc::filled() const {
  return m_filled;
}

// Get a DB folder
StatusCode SCT_MajorityConditionsSvc::retrieveFolder(const DataHandle<CondAttrListCollection>& pDataVec, const std::string& folderName) {
  if (not m_detStore) {
    ATH_MSG_FATAL("The detector store pointer is NULL");
    return StatusCode::FAILURE;
  }
  if (m_detStore->retrieve(pDataVec, folderName).isFailure()) {
    ATH_MSG_FATAL("Could not retrieve AttrListCollection for " << folderName);
    return StatusCode::FAILURE;
  }
  if (0 == pDataVec->size()) {
    ATH_MSG_FATAL("This folder's data set appears to be empty: " << folderName);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
