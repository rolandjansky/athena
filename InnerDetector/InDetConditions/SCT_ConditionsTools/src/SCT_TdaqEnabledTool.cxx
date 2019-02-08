/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTool.cxx
 * implementation file for service allowing one to declare rods as bad, or read the bad rods from the Tdaq entries in COOL db
 * @author shaun.roe@cern.ch
**/

#include "SCT_TdaqEnabledTool.h"

//Use Event info to determine whether folder is expetd to have valid data
#include "EventInfo/EventID.h"

// Read (Cond) Handle
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_TdaqEnabledTool::SCT_TdaqEnabledTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_mutex{},
  m_cache{},
  m_condData{},
  m_pHelper{nullptr},
  m_useDatabase{true}
{
  declareProperty("EventInfoKey", m_eventInfoKey=std::string{"ByteStreamEventInfo"});
}

//Initialize
StatusCode 
SCT_TdaqEnabledTool::initialize() {
  const std::string databaseUseString{m_useDatabase?"":"not "};
  ATH_MSG_INFO(" Database will "<<databaseUseString<<"be used.");

  ATH_CHECK(detStore()->retrieve(m_pHelper,"SCT_ID"));
  // Read (Cond) Handle Key
  ATH_CHECK(m_eventInfoKey.initialize());
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
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cache.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_TdaqEnabledCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
