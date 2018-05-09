/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondTool.h"

#include "GaudiKernel/ThreadLocalContext.h"

//const Identifier SCT_DCSConditionsTool::s_invalidId;
const float PixelDCSCondTool::s_defaultHV(150.0); 
const float PixelDCSCondTool::s_defaultTemperature(-20.0);

PixelDCSCondTool::PixelDCSCondTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_readAllDBFolders(true),
  m_returnHVTemp(true),
  m_mutex(),
  m_cacheState(),
  m_cacheHV(),
  m_cacheTemp0(),
  m_pBadModules(),
  m_pModulesHV(),
  m_pModulesTemp0(),
  m_condKeyState("PixelDCSStatCondData"),
  m_condKeyHV("PixelDCSHVCondData"),
  m_condKeyTemp0("PixelDCSTemp0CondData"),
  m_pHelper{nullptr}
{ 
  declareProperty("ReadAllDBFolders", m_readAllDBFolders);
  declareProperty("ReturnHVTemp", m_returnHVTemp);
}

StatusCode PixelDCSCondTool::initialize() {
  ATH_MSG_INFO("PixelDCSCondTool::initialize()");

  CHECK(detStore()->retrieve(m_pHelper,"PixelID"));

  // Read Cond Handle Keys
  if ((m_readAllDBFolders and m_returnHVTemp) or m_returnHVTemp) {
    ATH_CHECK(m_condKeyHV.initialize());
    ATH_CHECK(m_condKeyTemp0.initialize());
  }
  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    ATH_CHECK(m_condKeyState.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondTool::finalize() { 
  ATH_MSG_INFO("PixelDCSCondTool::finalize()");
  return StatusCode::SUCCESS;
}

//Can report about the module as a whole or the wafer
bool PixelDCSCondTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::PIXEL_MODULE or h==InDetConditions::PIXEL_CHIP);
}   

//Returns if element Id is good or bad
bool PixelDCSCondTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  Identifier moduleId = m_pHelper->wafer_id(elementId);
  if (not moduleId.is_valid()) { return true; }

  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    const EventContext& ctx(Gaudi::Hive::currentContext());
    const PixelDCSCondData* condDataState(getCondDataState(ctx));
    if (!condDataState) { 
      return false; // no cond data
    }
    else if (condDataState->output((unsigned int)moduleId.get_identifier32().get_compact())==0) {
      return true;  //No params are listed as bad
    }
    else { 
      return false; 
    }
  } 
  else {
    return true;
  }
}

//Does the same for hashIds
bool PixelDCSCondTool::isGood(const IdentifierHash& hashId) const {
  Identifier waferId = m_pHelper->wafer_id(hashId);
  return isGood(waferId, InDetConditions::PIXEL_MODULE);
}

// returns HV (s_defaultHV(-30) if there is no information)
float PixelDCSCondTool::getHV(const Identifier& elementId) const {
  Identifier moduleId = m_pHelper->wafer_id(elementId);
  if (not moduleId.is_valid()) { return s_defaultHV; }

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const PixelDCSCondData* condDataHV = getCondDataHV(ctx);
  if (!condDataHV) { return s_defaultHV; }

  float hvval = s_defaultHV;
  if (condDataHV->getValue((unsigned int)moduleId.get_identifier32().get_compact(), hvval)) {
    return hvval;
  }
  return s_defaultHV; //didn't find the module, return default
}

//Does the same for hashIds
float PixelDCSCondTool::getHV(const IdentifierHash& hashId) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const PixelDCSCondData* condDataHV = getCondDataHV(ctx);
  if (!condDataHV) { return s_defaultHV; }

  float hvval = s_defaultHV;
  if (condDataHV->getValue((unsigned int)hashId,hvval)) {
    return hvval;
  }
  return s_defaultHV;
} 

//Returns temp0 (s_defaultTemperature(-40) if there is no information)
float PixelDCSCondTool::getTemperature(const Identifier& elementId) const {
  Identifier moduleId = m_pHelper->wafer_id(elementId);
  if (not moduleId.is_valid()) { return s_defaultTemperature; } // not canreportabout

  const EventContext& ctx(Gaudi::Hive::currentContext());
  const PixelDCSCondData* condDataTemp0(getCondDataTemp0(ctx));
  if (!condDataTemp0) { return s_defaultTemperature; } // no cond data

  float temperature = s_defaultTemperature;
  if (condDataTemp0->getValue((unsigned int)moduleId.get_identifier32().get_compact(), temperature)) {
    return temperature;
  }
  return s_defaultTemperature;  //didn't find the module, return default
} 

//Does the same for hashIds
float PixelDCSCondTool::getTemperature(const IdentifierHash& hashId) const {
  Identifier waferId = m_pHelper->wafer_id(hashId);
  return getTemperature(waferId);
}

const PixelDCSCondData* PixelDCSCondTool::getCondDataState(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue(EventContext::INVALID_CONTEXT_EVT);
  EventContext::ContextID_t slot(ctx.slot());
  EventContext::ContextEvt_t evt(ctx.evt());
  std::lock_guard<std::mutex> lock(m_mutex);
  if (slot>=m_cacheState.size()) {
    m_cacheState.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheState[slot]!=evt) {
    SG::ReadCondHandle<PixelDCSCondData> condData(m_condKeyState);
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyState.key());
    }
    m_pBadModules.set(*condData);
    m_cacheState[slot] = evt;
  }
  return m_pBadModules.get();
}

const PixelDCSCondData* PixelDCSCondTool::getCondDataHV(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue(EventContext::INVALID_CONTEXT_EVT);
  EventContext::ContextID_t slot(ctx.slot());
  EventContext::ContextEvt_t evt(ctx.evt());
  std::lock_guard<std::mutex> lock(m_mutex);
  if (slot>=m_cacheHV.size()) {
    m_cacheHV.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheHV[slot]!=evt) {
    SG::ReadCondHandle<PixelDCSCondData> condData(m_condKeyHV);
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    }
    m_pModulesHV.set(*condData);
    m_cacheHV[slot] = evt;
  }
  return m_pModulesHV.get();
}

const PixelDCSCondData* PixelDCSCondTool::getCondDataTemp0(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue(EventContext::INVALID_CONTEXT_EVT);
  EventContext::ContextID_t slot(ctx.slot());
  EventContext::ContextEvt_t evt(ctx.evt());
  std::lock_guard<std::mutex> lock(m_mutex);
  if (slot>=m_cacheTemp0.size()) {
    m_cacheTemp0.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cacheTemp0[slot]!=evt) {
    SG::ReadCondHandle<PixelDCSCondData> condData{m_condKeyTemp0};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKeyTemp0.key());
    }
    m_pModulesTemp0.set(*condData);
    m_cacheTemp0[slot] = evt;
  }
  return m_pModulesTemp0.get();
}
