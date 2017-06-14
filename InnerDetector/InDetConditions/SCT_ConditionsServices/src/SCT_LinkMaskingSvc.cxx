/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingSvc.h"

// STL includes
#include <vector>
#include <iostream>

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Static folder names 
static const std::string coolLinkFolderName("/purple/pants");

// Constructor
SCT_LinkMaskingSvc::SCT_LinkMaskingSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_filled(false),
  m_detStore("DetectorStore", name),
  m_IOVSvc("IOVSvc", name),
  m_sctHelper{nullptr},
  m_cablingSvc("SCT_CablingSvc", name)
{ 
  // nop
}

// Initialize
StatusCode SCT_LinkMaskingSvc::initialize(){
  msg(MSG:: INFO)<< "Initializing configuration" << endmsg;

  // Retrieve cabling service
  if (m_cablingSvc.retrieve().isFailure())                     return msg(MSG:: ERROR)<< "Can't get the cabling service." << endmsg, StatusCode::FAILURE;

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure())                       return msg(MSG:: FATAL)<< "Detector service  not found !" << endmsg, StatusCode::FAILURE;
  
  // Retrieve SCT ID helper
  if (m_detStore->retrieve(m_sctHelper, "SCT_ID").isFailure()) return msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg, StatusCode::FAILURE;

  // Retrieve IOV service
  if (m_IOVSvc.retrieve().isFailure())                         return msg(MSG:: ERROR)<< "Failed to retrieve IOVSvc " << endmsg, StatusCode::FAILURE;

  // Register callbacks for folders 
  if (m_detStore->regFcn(&SCT_LinkMaskingSvc::fillData,this, m_dataLink,coolLinkFolderName).isFailure()) 
    return msg(MSG:: ERROR)<< "Failed to register callback" << endmsg, StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_LinkMaskingSvc::finalize(){
  msg(MSG:: INFO)<< "Configuration finalize" << endmsg;
  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_LinkMaskingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// What level of element can this service report about
bool SCT_LinkMaskingSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT); 
}

// Is an element with this Identifier and hierachy good?
bool SCT_LinkMaskingSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy  h){
  if (not canReportAbout(h)) return true;

  bool result(true);
  result = (m_maskedLinkIds.find(elementId) == m_maskedLinkIds.end());

  return result;
}

// Is a wafer with this IdentifierHash good?
bool SCT_LinkMaskingSvc::isGood(const IdentifierHash & hashId){
  Identifier elementId(m_sctHelper->wafer_id(hashId));
  return isGood(elementId);
}

  // Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_LinkMaskingSvc::fillData(int& /*i*/ , std::list<std::string>& /*l*/){  

  // Clear previous information at callback
  m_maskedLinkIds.clear();

  // Get link folder
  if (retrieveFolder(m_dataLink, coolLinkFolderName).isFailure()) {
    return msg(MSG:: ERROR)<< "Could not fill masked link data" << endmsg, StatusCode::FAILURE;
  } else {
    msg(MSG:: INFO)<< "fillChannelData: IOV callback resulted in a link CondAttrListCollection of size " << m_dataLink->size() << endmsg;
  }

  // 
  CondAttrListCollection::const_iterator linkItr(m_dataLink->begin());
  CondAttrListCollection::const_iterator linkEnd(m_dataLink->end());

  for (;linkItr != linkEnd; ++linkItr) {
    //A CondAttrListCollection is a map of ChanNum and AttributeList
    Identifier waferId((*linkItr).first);
    CondAttrListCollection::AttributeList   payload = (*linkItr).second;    
    bool lastProbedState = payload[0].data<bool>();
    if (!lastProbedState) m_maskedLinkIds.insert(waferId);
    //std::cout << "LINK " << waferId << ": " << lastProbedState << std::endl;    
  }

  m_filled = (m_maskedLinkIds.size() != 0);

  const unsigned int totalMasked(m_maskedLinkIds.size());
  msg(MSG:: INFO)<< "Total number of masked links is " << totalMasked << endmsg;
  return StatusCode::SUCCESS;
}

// Is the information filled?
bool SCT_LinkMaskingSvc::filled() const{
  return m_filled;
}

// Get a DB folder
StatusCode SCT_LinkMaskingSvc::retrieveFolder(const DataHandle<CondAttrListCollection> &pDataVec, const std::string & folderName){
  if (not m_detStore) return (msg(MSG:: FATAL) << "The detector store pointer is NULL" << endmsg), StatusCode::FAILURE;

  if (m_detStore->retrieve(pDataVec, folderName).isFailure()) 
    return (msg(MSG:: FATAL) << "Could not retrieve AttrListCollection for " << folderName << endmsg), StatusCode::FAILURE;

  if (0 == pDataVec->size()) return (msg(MSG:: FATAL) << "This folder's data set appears to be empty: " << folderName << endmsg), StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

