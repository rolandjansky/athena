/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingTool.h"
#include "SCT_DetectorElementStatus.h"

// Athena includes
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_LinkMaskingTool::SCT_LinkMaskingTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent)
{ 
}

// Initialize
StatusCode SCT_LinkMaskingTool::initialize() {
  ATH_MSG_DEBUG("Initializing configuration");

  // Retrieve SCT ID helper
  if (detStore()->retrieve(m_sctHelper, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle
  ATH_CHECK(m_condKey.initialize());

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
  return isGood(elementId, ctx, InDetConditions::SCT_SIDE);
}

bool SCT_LinkMaskingTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

void SCT_LinkMaskingTool::getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status,
                                                   SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const {
  SG::ReadCondHandle<SCT_ModuleVetoCondData> condDataHandle{m_condKey, ctx};
  if (not condDataHandle.isValid()) {
     ATH_MSG_ERROR("Failed to get " << m_condKey.key());
     return;
  }
  const SCT_ModuleVetoCondData* condData{condDataHandle.cptr()};
  if (whandle) {
    whandle->addDependency (condDataHandle);
  }
  if (condData) {
     std::vector<bool> &status = element_status.getElementStatus();
     if (status.empty()) {
        status.resize(m_sctHelper->wafer_hash_max(),true);
     }
     for (const Identifier &wafer_id: condData->badWaferIds()) {
        status.at( m_sctHelper->wafer_hash(wafer_id) ) = false;
     }
  }
}

const SCT_ModuleVetoCondData*
SCT_LinkMaskingTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_ModuleVetoCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}
