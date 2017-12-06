/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingSvc.h"

// STL includes

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

// Static folder name
static const std::string coolLinkFolderName{"/purple/pants"};
// This folder can be created by SCT_ConditionsServices/python/createLinkMaskingSQLiteFile.py 

// Constructor
SCT_LinkMaskingSvc::SCT_LinkMaskingSvc(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthService(name, pSvcLocator),
  m_filled{false},
  m_detStore{"DetectorStore", name},
  m_sctHelper{nullptr}
{ 
  // nop
}

// Initialize
StatusCode SCT_LinkMaskingSvc::initialize() {
  ATH_MSG_INFO("Initializing configuration");

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Detector service  not found !");
    return StatusCode::FAILURE;
  }
   
  // Retrieve SCT ID helper
  if (m_detStore->retrieve(m_sctHelper, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper");
    return StatusCode::FAILURE;
  }

  // Register callbacks for folders 
  if (m_detStore->regFcn(&SCT_LinkMaskingSvc::fillData, this, m_dataLink, coolLinkFolderName).isFailure()) {
    ATH_MSG_FATAL("Failed to register callback");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_LinkMaskingSvc::finalize() {
  ATH_MSG_INFO("Configuration finalize");
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
bool SCT_LinkMaskingSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::SCT_SIDE or h==InDetConditions::DEFAULT); 
}

// Is an element with this Identifier and hierachy good?
bool SCT_LinkMaskingSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return true;

  bool result{true};
  result = (m_maskedLinkIds.find(elementId) == m_maskedLinkIds.end());

  return result;
}

// Is a wafer with this IdentifierHash good?
bool SCT_LinkMaskingSvc::isGood(const IdentifierHash& hashId) {
  Identifier elementId{m_sctHelper->wafer_id(hashId)};
  return isGood(elementId);
}

// Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_LinkMaskingSvc::fillData(int& /*i*/, std::list<std::string>& /*l*/) {
  // Clear previous information at callback
  m_maskedLinkIds.clear();

  // Get link folder
  if (retrieveFolder(m_dataLink, coolLinkFolderName).isFailure()) {
    ATH_MSG_FATAL("Could not fill masked link data");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("fillChannelData: IOV callback resulted in a link CondAttrListCollection of size " << m_dataLink->size());
  }

  CondAttrListCollection::const_iterator linkItr{m_dataLink->begin()};
  CondAttrListCollection::const_iterator linkEnd{m_dataLink->end()};
  for (;linkItr != linkEnd; ++linkItr) {
    //A CondAttrListCollection is a map of ChanNum and AttributeList
    Identifier waferId{(*linkItr).first};
    CondAttrListCollection::AttributeList payload{(*linkItr).second};
    bool lastProbedState{payload[0].data<bool>()};
    if (not lastProbedState) m_maskedLinkIds.insert(waferId);
    ATH_MSG_INFO("LINK " << waferId << ": " << lastProbedState);
  }

  m_filled = (m_maskedLinkIds.size() != 0);

  const long unsigned int totalMasked{m_maskedLinkIds.size()};
  ATH_MSG_INFO("Total number of masked links is " << totalMasked);

  return StatusCode::SUCCESS;
}

// Is the information filled?
bool SCT_LinkMaskingSvc::filled() const {
  return m_filled;
}

// Get a DB folder
StatusCode SCT_LinkMaskingSvc::retrieveFolder(const DataHandle<CondAttrListCollection>& pDataVec, const std::string& folderName) {
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
