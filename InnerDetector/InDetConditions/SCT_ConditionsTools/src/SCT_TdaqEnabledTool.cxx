/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTool.cxx
 * implementation file for service allowing one to declare rods as bad, or read the bad rods from the Tdaq entries in COOL db
 * @author shaun.roe@cern.ch
**/

#include "SCT_TdaqEnabledTool.h"

#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// Constructor
SCT_TdaqEnabledTool::SCT_TdaqEnabledTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_pHelper{nullptr},
  m_useDatabase{true}
{
}

//Initialize
StatusCode 
SCT_TdaqEnabledTool::initialize() {
  const std::string databaseUseString{m_useDatabase?"":"not "};
  ATH_MSG_INFO(" Database will "<<databaseUseString<<"be used.");

  ATH_CHECK(detStore()->retrieve(m_pHelper,"SCT_ID"));
  // Read Cond Handle Key
  if (m_useDatabase) {
    ATH_CHECK(m_condKey.initialize());
  }
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_TdaqEnabledTool::finalize() {
  return StatusCode::SUCCESS;
}

bool 
SCT_TdaqEnabledTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::DEFAULT or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_MODULE); 
}

bool 
SCT_TdaqEnabledTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;
  //turn to hash, given the identifier
  const IdentifierHash hashId{m_pHelper->wafer_hash(elementId)};
  return isGood(hashId, ctx);
}

bool 
SCT_TdaqEnabledTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

bool
SCT_TdaqEnabledTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  const SCT_TdaqEnabledCondData* condData{getCondData(ctx)};
  if (!condData) return false;
  return condData->isGood(hashId);
}

bool
SCT_TdaqEnabledTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

const SCT_TdaqEnabledCondData*
SCT_TdaqEnabledTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_TdaqEnabledCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}
