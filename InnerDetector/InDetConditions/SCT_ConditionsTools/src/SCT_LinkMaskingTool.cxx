/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingTool.h"

// Athena includes
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_LinkMaskingTool::SCT_LinkMaskingTool(const std::string &type, const std::string &name, const IInterface *parent) : 
  base_class(type, name, parent),
  m_sctHelper{nullptr}
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
  SG::ReadCondHandle<SCT_ModuleVetoCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}
