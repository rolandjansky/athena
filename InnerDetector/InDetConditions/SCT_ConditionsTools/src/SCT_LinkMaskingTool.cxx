/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingTool.h"

// Athena includes
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_LinkMaskingTool::SCT_LinkMaskingTool(const std::string &type, const std::string &name, const IInterface *parent) : 
  base_class(type, name, parent),
  m_sctHelper{nullptr},
  m_mutex{},
  m_cache{},
  m_condData{}
{ 
}

// Initialize
StatusCode SCT_LinkMaskingTool::initialize() {
  ATH_MSG_INFO("Initializing configuration");

  // Retrieve SCT ID helper
  if (detStore()->retrieve(m_sctHelper, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle
  CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_LinkMaskingTool::finalize() {
  ATH_MSG_INFO("Configuration finalize");
  return StatusCode::SUCCESS;
}

// What level of element can this service report about
bool SCT_LinkMaskingTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_SIDE or h==InDetConditions::DEFAULT); 
}

// Is an element with this Identifier and hierachy good?
bool SCT_LinkMaskingTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;

  const SCT_ModuleVetoCondData* condData{getCondData(ctx)};
  // If database cannot be retrieved, all wafer IDs are good.
  if (condData==nullptr) return true;

  // Return the result of database
  return (not condData->isBadWaferId(elementId));
}

bool SCT_LinkMaskingTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

// Is a wafer with this IdentifierHash good?
bool SCT_LinkMaskingTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier elementId{m_sctHelper->wafer_id(hashId)};
  return isGood(elementId, ctx);
}

bool SCT_LinkMaskingTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

const SCT_ModuleVetoCondData*
SCT_LinkMaskingTool::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cache.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_ModuleVetoCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
