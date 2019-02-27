/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMULATIONBASE
#include "PixelConditionsSummaryTool.h"

#define UNUSED_VARIABLE(x) (void)(x)

PixelConditionsSummaryTool::PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent)
  :AthAlgTool(type, name, parent),
  m_pixelID(0),
  m_pixelBSErrorsSvc("PixelByteStreamErrorsSvc", name),
  m_useDCSState(false),
  m_useByteStream(false),
  m_useTDAQ(false),
  m_useDeadMap(true)
{
  m_isActiveStatus.push_back("OK");
  m_isActiveStates.push_back("READY");

  declareProperty("IsActiveStatus", m_isActiveStatus);
  declareProperty("IsActiveStates", m_isActiveStates);
  declareProperty("UseDCSState", m_useDCSState, "Switch for usage of DCS");
  declareProperty("UseByteStream", m_useByteStream, "Switch for usage of the ByteStream error service");
  declareProperty("UseTDAQ", m_useTDAQ, "Switch for usage of TDAQ");
  declareProperty("UseDeadMap", m_useDeadMap, "Switch for usage of dead map");
}

PixelConditionsSummaryTool::~PixelConditionsSummaryTool(){}

StatusCode PixelConditionsSummaryTool::initialize(){
  ATH_MSG_DEBUG("PixelConditionsSummaryTool::initialize()");

  ATH_CHECK(setProperties());

  if (m_useDCSState) {
    ATH_CHECK(m_condDCSStateKey.initialize());
    ATH_CHECK(m_condDCSStatusKey.initialize());
  }

  if (m_useByteStream) { ATH_CHECK(m_pixelBSErrorsSvc.retrieve()); }

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  if (m_useTDAQ) { ATH_CHECK(m_condTDAQKey.initialize()); }

  if (m_useDeadMap) { ATH_CHECK(m_condDeadMapKey.initialize()); }

  for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
    if      (m_isActiveStates[istate]=="READY")      { m_activeState.push_back(PixelModuleData::DCSModuleState::READY); }
    else if (m_isActiveStates[istate]=="ON")         { m_activeState.push_back(PixelModuleData::DCSModuleState::ON); }
    else if (m_isActiveStates[istate]=="UNKNOWN")    { m_activeState.push_back(PixelModuleData::DCSModuleState::UNKNOWN); }
    else if (m_isActiveStates[istate]=="TRANSITION") { m_activeState.push_back(PixelModuleData::DCSModuleState::TRANSITION); }
    else if (m_isActiveStates[istate]=="UNDEFINED")  { m_activeState.push_back(PixelModuleData::DCSModuleState::UNDEFINED); }
    else if (m_isActiveStates[istate]=="NOSTATE")    { m_activeState.push_back(PixelModuleData::DCSModuleState::NOSTATE); }
    else {
      ATH_MSG_ERROR("No matching DCS state " << m_isActiveStates[istate] << " in DCSModuleState");
      return StatusCode::FAILURE;
    }
  }

  for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
    if      (m_isActiveStatus[istatus]=="OK")       { m_activeStatus.push_back(PixelModuleData::DCSModuleStatus::OK); }
    else if (m_isActiveStatus[istatus]=="WARNING")  { m_activeStatus.push_back(PixelModuleData::DCSModuleStatus::WARNING); }
    else if (m_isActiveStatus[istatus]=="ERROR")    { m_activeStatus.push_back(PixelModuleData::DCSModuleStatus::ERROR); }
    else if (m_isActiveStatus[istatus]=="FATAL")    { m_activeStatus.push_back(PixelModuleData::DCSModuleStatus::FATAL); }
    else if (m_isActiveStatus[istatus]=="NOSTATUS") { m_activeStatus.push_back(PixelModuleData::DCSModuleStatus::NOSTATUS); }
    else {
      ATH_MSG_ERROR("No matching DCS status " << m_isActiveStatus[istatus] << " in DCSModuleStatus");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

bool PixelConditionsSummaryTool::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h) const {
 
  UNUSED_VARIABLE(elementId);
  UNUSED_VARIABLE(h);

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash & moduleHash) const {

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return false; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash & moduleHash, const Identifier & elementId) const{

  UNUSED_VARIABLE(elementId);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return false; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

double PixelConditionsSummaryTool::activeFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {

  UNUSED_VARIABLE(moduleHash);
  UNUSED_VARIABLE(idStart);
  UNUSED_VARIABLE(idEnd);

  return 1.0;
}

bool PixelConditionsSummaryTool::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h)const{

  UNUSED_VARIABLE(h);

  Identifier moduleID       = m_pixelID->wafer_id(elementId);
  IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }

    SG::ReadCondHandle<PixelModuleData> dcsstatus_data(m_condDCSStatusKey);
    bool isDCSGood = false;
    for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
      if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
    }
    if (!isDCSGood) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash) const {

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }

    SG::ReadCondHandle<PixelModuleData> dcsstatus_data(m_condDCSStatusKey);
    bool isDCSGood = false;
    for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
      if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
    }
    if (!isDCSGood) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash, const Identifier & elementId) const {

  UNUSED_VARIABLE(elementId);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }

    SG::ReadCondHandle<PixelModuleData> dcsstatus_data(m_condDCSStatusKey);
    bool isDCSGood = false;
    for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
      if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
    }
    if (!isDCSGood) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

double PixelConditionsSummaryTool::goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {

  UNUSED_VARIABLE(idStart);
  UNUSED_VARIABLE(idEnd);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return 0.; }

  if (m_useDCSState) {
    SG::ReadCondHandle<PixelModuleData> dcsstate_data(m_condDCSStateKey);
    bool isDCSActive = false;
    for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
      if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
    }
    if (!isDCSActive) { return false; }

    SG::ReadCondHandle<PixelModuleData> dcsstatus_data(m_condDCSStatusKey);
    bool isDCSGood = false;
    for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
      if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
    }
    if (!isDCSGood) { return 0.0; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condTDAQKey)->getModuleStatus(moduleHash)) { return 1.0; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getModuleStatus(moduleHash)) { return 1.0; }

  // TODO!!!  Calculate active fraction from dead map.

  return 1.0;
}

#endif // not SIMULATIONBASE
