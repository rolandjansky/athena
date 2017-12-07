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

// Constructor
SCT_LinkMaskingSvc::SCT_LinkMaskingSvc(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthService(name, pSvcLocator),
  m_detStore{"DetectorStore", name},
  m_sctHelper{nullptr},
  m_mutex{},
  m_cache{},
  m_condData{},
  m_condKey{"SCT_LinkMaskingCondData"}
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

  // Read Cond Handle
  CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_LinkMaskingSvc::finalize() {
  ATH_MSG_INFO("Configuration finalize");
  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_LinkMaskingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
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

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ModuleVetoCondData* condData{getCondData(ctx)};
  // If database cannot be retrieved, all wafer IDs are good.
  if (condData==nullptr) return true;

  // Return the result of database
  return (not condData->isBadWaferId(elementId));
}

// Is a wafer with this IdentifierHash good?
bool SCT_LinkMaskingSvc::isGood(const IdentifierHash& hashId) {
  Identifier elementId{m_sctHelper->wafer_id(hashId)};
  return isGood(elementId);
}

// Is the information filled?
bool SCT_LinkMaskingSvc::filled() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ModuleVetoCondData* condData{getCondData(ctx)};
  return (condData!=nullptr);
}

const SCT_ModuleVetoCondData*
SCT_LinkMaskingSvc::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_ModuleVetoCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
