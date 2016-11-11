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

// Note, up till Friday 23rd Oct the conditions update was too frequent (every 10 sec).
// Shouldn't use for data before this.

// Static folder names 
static const std::string coolMajorityFolderName("/SCT/DCS/MAJ");

// Constructor
SCT_MajorityConditionsSvc::SCT_MajorityConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_filled(false),
  m_detStore("DetectorStore", name),
  m_IOVSvc("IOVSvc", name),
  m_overall(false),
  m_majorityFraction(0.9){ 
    declareProperty("UseOverall", m_overall);
    declareProperty("MajorityFraction", m_majorityFraction);
}

// Initialize
StatusCode SCT_MajorityConditionsSvc::initialize(){
  msg(MSG:: INFO)<< "Initializing SCT_MajorityConditionsSvc" << endmsg;

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure()) return msg(MSG:: FATAL)<< "Detector service  not found !" << endmsg, StatusCode::FAILURE;

  // Retrieve IOV service
  if (m_IOVSvc.retrieve().isFailure()) return msg(MSG:: ERROR)<< "Failed to retrieve IOVSvc " << endmsg, StatusCode::FAILURE;

  // Register callbacks for folders 
  if (m_detStore->regFcn(&SCT_MajorityConditionsSvc::fillData,this, m_dataMajority, coolMajorityFolderName).isFailure())
    return msg(MSG:: ERROR)<< "Failed to register callback" << endmsg, StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_MajorityConditionsSvc::finalize(){
  msg(MSG:: INFO)<< "Finalizing SCT_MajorityConditionsSvc" << endmsg;

  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_MajorityConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

  if ( ISCT_DetectorLevelConditionsSvc::interfaceID().versionMatch(riid) ) {
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
    return (m_majorityState[OVERALL] && m_hvFraction[OVERALL] > m_majorityFraction);
  } else {
    return ((m_majorityState[BARREL] && m_majorityState[ECA] && m_majorityState[ECC]) && 
	    m_hvFraction[BARREL] > m_majorityFraction && m_hvFraction[ECA] > m_majorityFraction && m_hvFraction[ECC] > m_majorityFraction);
  }
}

// Is a barrel/endcap good?
bool SCT_MajorityConditionsSvc::isGood(int bec) {
  bool result(true);

  // Check numbering

  if (bec == 0) {
    result = m_majorityState[BARREL] && m_hvFraction[BARREL]  > m_majorityFraction;
  } else if (bec == -2) { 
    result = m_majorityState[ECC]    && m_hvFraction[ECC]     > m_majorityFraction;
  } else if (bec == 2) {
    result = m_majorityState[ECA]    && m_hvFraction[ECA] > m_majorityFraction;
  } else {
    msg(MSG::WARNING) << "Unrecognised BEC " << bec << endmsg;
  }

  return result;
}


// Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_MajorityConditionsSvc::fillData(int& /*i*/ , std::list<std::string>& /*l*/){  

  int numFilled(0);

  // Get Majority folder
  if (retrieveFolder(m_dataMajority, coolMajorityFolderName).isFailure()) {
    return msg(MSG:: ERROR)<< "Could not fill majority data" << endmsg, StatusCode::FAILURE;
  } else {
    msg(MSG:: INFO)<< "fillMajorityData: IOV callback resulted in a Chip CondAttrListCollection of size " << m_dataMajority->size() << endmsg;
  }

  CondAttrListCollection::const_iterator majItr(m_dataMajority->begin());
  CondAttrListCollection::const_iterator majEnd(m_dataMajority->end());
   
  for (;majItr != majEnd; ++majItr) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum  channelNumber  = (*majItr).first;
    CondAttrListCollection::AttributeList   payload = (*majItr).second;   
    
    // Possible components
    if (channelNumber == OVERALL || channelNumber == BARREL || channelNumber == ECA || channelNumber == ECC) {      
      // Reset default to true at callback
      m_majorityState[channelNumber] = true;
      m_hvFraction[channelNumber]    = 1.;

      // Majority state
      if (!payload[3].isNull()) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Majority state for " << channelNumber << " = " << payload[3].data<int>() << endmsg;
	m_majorityState[channelNumber] = (payload[3].data<int>() == HighAndLowVoltageOK);
      }

      // HV fraction in majority state (>50% by definition) IF majority state is HV and LV on
      if (m_majorityState[channelNumber] && !payload[1].isNull()) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Majority HV fraction for " << channelNumber << " = " << payload[1].data<float>() << endmsg;
	m_hvFraction[channelNumber] = payload[1].data<float>();
	numFilled++;
      }

    } else {
      msg(MSG::WARNING) << "Unexpected channel number " << channelNumber << endmsg;
    }
  }

  // Has data been filled
  m_filled = (numFilled==4 ? true : false);

  return StatusCode::SUCCESS;
}

// Is the information filled?
bool SCT_MajorityConditionsSvc::filled() const{
  return m_filled;
}

// Get a DB folder
StatusCode SCT_MajorityConditionsSvc::retrieveFolder(const DataHandle<CondAttrListCollection> &pDataVec, const std::string & folderName){
  if (not m_detStore) return (msg(MSG:: FATAL) << "The detector store pointer is NULL" << endmsg), StatusCode::FAILURE;
  if (m_detStore->retrieve(pDataVec, folderName).isFailure()) 
    return (msg(MSG:: FATAL) << "Could not retrieve AttrListCollection for " << folderName << endmsg), StatusCode::FAILURE;
  if (0 == pDataVec->size()) return (msg(MSG:: FATAL) << "This folder's data set appears to be empty: " << folderName << endmsg), StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

