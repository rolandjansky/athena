/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledSvc.cxx
 * implementation file for service allowing one to declare rods as bad, or read the bad rods from the Tdaq entries in COOL db
 * @author shaun.roe@cern.ch
**/

#include "SCT_TdaqEnabledSvc.h"

//STL includes
#include <iterator>
#include <iostream>

//Use Event info to determine whether folder is expetd to have valid data
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "SCT_Cabling/SCT_OnlineId.h"

// Read (Cond) Handle
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_TdaqEnabledSvc::SCT_TdaqEnabledSvc(const std::string& name, ISvcLocator* pSvcLocator) : AthService(name, pSvcLocator),
m_mutex{},
m_cache{},
m_condData{},
m_pHelper{nullptr},
m_useDatabase{true},
m_detStore{"DetectorStore", name},
m_condKey{std::string{"SCT_TdaqEnabledCondData"}}
{
  declareProperty("EventInfoKey", m_eventInfoKey=std::string{"ByteStreamEventInfo"});
}

//Initialize
StatusCode 
SCT_TdaqEnabledSvc::initialize() {
  const std::string databaseUseString{m_useDatabase?"":"not "};
  ATH_MSG_INFO(" Database will "<<databaseUseString<<"be used.");

  ATH_CHECK(m_detStore->retrieve(m_pHelper,"SCT_ID"));
  // Read (Cond) Handle Key
  ATH_CHECK(m_eventInfoKey.initialize());
  if (m_useDatabase) {
    ATH_CHECK(m_condKey.initialize());
  }
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_TdaqEnabledSvc::finalize() {
  return StatusCode::SUCCESS;
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
SCT_TdaqEnabledSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool 
SCT_TdaqEnabledSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::DEFAULT or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_MODULE); 
}

bool 
SCT_TdaqEnabledSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return true;
  //turn to hash, given the identifier
  const IdentifierHash hashId{m_pHelper->wafer_hash(elementId)};
  return isGood(hashId);
}

bool 
SCT_TdaqEnabledSvc::isGood(const IdentifierHash& hashId) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_TdaqEnabledCondData* condData{getCondData(ctx)};
  if (!condData) return false;
  return condData->isGood(hashId);
}

bool 
SCT_TdaqEnabledSvc::canFillDuringInitialize() {
  return (not m_useDatabase);// can only fill during intialize if we don't use the database
}

bool
SCT_TdaqEnabledSvc::filled() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_TdaqEnabledCondData* condData{getCondData(ctx)};
  if (!condData) return false;
  return condData->isFilled();
}

const SCT_TdaqEnabledCondData*
SCT_TdaqEnabledSvc::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_TdaqEnabledCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
