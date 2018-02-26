/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_FlaggedConditionRHSvc.h"

// Athena
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_FlaggedConditionRHSvc::SCT_FlaggedConditionRHSvc(const std::string& name, ISvcLocator* pSvcLocator):
  AthService(name, pSvcLocator),
  m_filled{false},
  m_badIds{},
  m_detStore{"DetectorStore", name},
  m_sctID{nullptr}
{/* Do nothing */}

// Initialize
StatusCode SCT_FlaggedConditionRHSvc::initialize() {
  ATH_MSG_INFO("SCT_FlaggedConditionRHSvc::initialize()");

  // Retrieve detector store
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Detector service  not found !");
    return StatusCode::FAILURE;
  }

  // Retrieve incident service and add 'BeginEvent' listener
  IIncidentSvc* incsvc;
  if (service("IncidentSvc", incsvc).isSuccess()) {
    int priority(100);
    incsvc->addListener( this, "BeginEvent", priority);
  }

  // Retrieve SCT helper
  if (m_detStore->retrieve(m_sctID, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_FlaggedConditionRHSvc::finalize() {
  ATH_MSG_INFO("SCT_FlaggedConditionRHSvc::finalize()");
  return StatusCode::SUCCESS;
}

// QueryInterface
StatusCode SCT_FlaggedConditionRHSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

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
void SCT_FlaggedConditionRHSvc::resetBadIds() {
  m_badIds.clear();
}

// Handle BeginEvent incident and call reset function
void SCT_FlaggedConditionRHSvc::handle(const Incident&) {
  this->resetBadIds();
}

// Detector elements that this service can report about
bool SCT_FlaggedConditionRHSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT);
}

// Is this element good (by Identifier)?
bool SCT_FlaggedConditionRHSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  const IdentifierHash hashId = m_sctID->wafer_hash(elementId);
  return isGood(hashId);
}

// Is this element good (by IdentifierHash)?
bool SCT_FlaggedConditionRHSvc::isGood(const IdentifierHash& hashId){
  return (m_badIds.find(hashId) == m_badIds.end());
}

// Flag a wafer as bad (by IdentifierHash) with a reason
bool SCT_FlaggedConditionRHSvc::flagAsBad(const IdentifierHash& hashId, const std::string& source) {
  return m_badIds.insert(make_pair(hashId, source)).second;
}

// Flag a wafer as bad (by Identifier) with a reason
bool SCT_FlaggedConditionRHSvc::flagAsBad(const Identifier& Id, const std::string& source) {
  const IdentifierHash hashId = m_sctID->wafer_hash(Id);
  return flagAsBad(hashId, source);
}

// Retrieve the reason why the wafer is flagged as bad (by IdentifierHash)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionRHSvc::details(const IdentifierHash& hashId) const {
  std::map<IdentifierHash, std::string>::const_iterator itr(m_badIds.find(hashId)); 
  static const std::string nullString;
  return ((itr != m_badIds.end()) ? (*itr).second : nullString);
}

// Retrieve the reason why the wafer is flagged as bad (by Identifier)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionRHSvc::details(const Identifier& Id) const {
  const IdentifierHash hashId = m_sctID->wafer_hash(Id);
  return details(hashId);
}
