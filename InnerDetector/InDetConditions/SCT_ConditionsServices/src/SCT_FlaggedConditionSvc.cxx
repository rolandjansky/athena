/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_FlaggedConditionSvc.h"

// Athena
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_FlaggedConditionSvc::SCT_FlaggedConditionSvc(const std::string& name, ISvcLocator* pSvcLocator):
  AthService(name, pSvcLocator),
  m_badIds{"SCT_FlaggedCondData"},
  m_detStore{"DetectorStore", name},
  m_sctID{nullptr}
{
  declareProperty("SCT_FlaggedCondData", m_badIds, "SCT flagged conditions data");
}

// Initialize
StatusCode SCT_FlaggedConditionSvc::initialize() {
  ATH_MSG_INFO("SCT_FlaggedConditionSvc::initialize()");

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Detector service  not found !");
    return StatusCode::FAILURE;
  }

  // Retrieve SCT helper
  if (m_detStore->retrieve(m_sctID, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_badIds.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_FlaggedConditionSvc::finalize() {
  ATH_MSG_INFO("SCT_FlaggedConditionSvc::finalize()");
  return StatusCode::SUCCESS;
}

// QueryInterface
StatusCode SCT_FlaggedConditionSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

  if ( ISCT_FlaggedConditionSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if ( ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// Clear list of bad IDs (called at begin event)
void SCT_FlaggedConditionSvc::resetBadIds() {
  ATH_MSG_ERROR("SCT_FlaggedConditionSvc::resetBadIds should not be used.");
}

// Detector elements that this service can report about
bool SCT_FlaggedConditionSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT);
}

// Is this element good (by Identifier)?
bool SCT_FlaggedConditionSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  const IdentifierHash hashId = m_sctID->wafer_hash(elementId);
  return isGood(hashId);
}

// Is this element good (by IdentifierHash)?
bool SCT_FlaggedConditionSvc::isGood(const IdentifierHash& hashId){
  const SCT_FlaggedCondData* badIds{getCondData()};
  if (badIds==nullptr) {
    ATH_MSG_ERROR("SCT_FlaggedCondData cannot be retrieved. (isGood)");
    return false;
  }

  return (badIds->find(hashId) == badIds->end());
}

// Flag a wafer as bad (by IdentifierHash) with a reason
bool SCT_FlaggedConditionSvc::flagAsBad(const IdentifierHash& /*hashId*/, const std::string& /*source*/) {
  ATH_MSG_ERROR("SCT_FlaggedConditionSvc::flagAsBad should not be used.");
  return false;
}

// Flag a wafer as bad (by Identifier) with a reason
bool SCT_FlaggedConditionSvc::flagAsBad(const Identifier& /*Id*/, const std::string& /*source*/) {
  ATH_MSG_ERROR("SCT_FlaggedConditionSvc::flagAsBad should not be used.");
  return false;
}

// Retrieve the reason why the wafer is flagged as bad (by IdentifierHash)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionSvc::details(const IdentifierHash& hashId) const {
  static const std::string nullString;

  const SCT_FlaggedCondData* badIds{getCondData()};
  if (badIds==nullptr) {
    ATH_MSG_ERROR("SCT_FlaggedCondData cannot be retrieved. (details)");
    return nullString;
  }

  std::map<IdentifierHash, std::string>::const_iterator itr(badIds->find(hashId));
  return ((itr != badIds->end()) ? (*itr).second : nullString);
}

// Retrieve the reason why the wafer is flagged as bad (by Identifier)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionSvc::details(const Identifier& Id) const {
  const IdentifierHash hashId = m_sctID->wafer_hash(Id);
  return details(hashId);
}

int SCT_FlaggedConditionSvc::numBadIds() const {
  const SCT_FlaggedCondData* badIds{getCondData()};
  if (badIds==nullptr) {
    ATH_MSG_ERROR("SCT_FlaggedCondData cannot be retrieved. (numBadIds)");
    return -1;
  }

  return badIds->size();
}

const SCT_FlaggedCondData* SCT_FlaggedConditionSvc::getBadIds() const {
  return getCondData();
}

const SCT_FlaggedCondData* SCT_FlaggedConditionSvc::getCondData() const {
  SG::ReadHandle<SCT_FlaggedCondData> condData{m_badIds};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_badIds.key());
    return nullptr;
  }
  return condData.cptr();
}
