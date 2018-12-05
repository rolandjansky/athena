/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

  if (m_useDCSState)   { ATH_CHECK(m_DCSConditionsTool.retrieve()); }

  if (m_useByteStream) { ATH_CHECK(m_pixelBSErrorsSvc.retrieve()); }

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  if (m_useTDAQ || m_useDeadMap) { ATH_CHECK(m_condKey.initialize()); }

  return StatusCode::SUCCESS;
}

bool PixelConditionsSummaryTool::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h) const {

  UNUSED_VARIABLE(h);

  IdentifierHash moduleHash = m_pixelID->wafer_hash(elementId);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    if (!isDCSActive) { return isDCSActive; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash & moduleHash) const {

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    if (!isDCSActive) { return isDCSActive; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash & moduleHash, const Identifier & elementId) const{

  UNUSED_VARIABLE(elementId);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    if (!isDCSActive) { return isDCSActive; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

double PixelConditionsSummaryTool::activeFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {

  UNUSED_VARIABLE(idStart);
  UNUSED_VARIABLE(idEnd);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isActive(moduleHash)) { return 0.; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    if (!isDCSActive) { return 0.; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return 0.0; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return 0.0; }

  // TODO!!!  Calculate active fraction from dead map.
  
  return 1.;
}

bool PixelConditionsSummaryTool::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h)const{

  UNUSED_VARIABLE(h);

  Identifier moduleID       = m_pixelID->wafer_id(elementId);
  IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_DCSConditionsTool->PixelFSMStatus(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
      if (m_isActiveStatus[istatus]==dcsStatus) { isDCSGood=true; }
    }
    if (!(isDCSActive && isDCSGood)) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash) const {

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_DCSConditionsTool->PixelFSMStatus(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
      if (m_isActiveStatus[istatus]==dcsStatus) { isDCSGood=true; }
    }
    if (!(isDCSActive && isDCSGood)) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash, const Identifier & elementId) const {

  UNUSED_VARIABLE(elementId);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return false; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_DCSConditionsTool->PixelFSMStatus(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
      if (m_isActiveStatus[istatus]==dcsStatus) { isDCSGood=true; }
    }
    if (!(isDCSActive && isDCSGood)) { return false; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return false; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

double PixelConditionsSummaryTool::goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {

  UNUSED_VARIABLE(idStart);
  UNUSED_VARIABLE(idEnd);

  if (m_useByteStream && !m_pixelBSErrorsSvc->isGood(moduleHash)) { return 0.; }

  if (m_useDCSState) {
    bool isDCSActive = false;
    std::string dcsState = m_DCSConditionsTool->PixelFSMState(moduleHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_DCSConditionsTool->PixelFSMStatus(moduleHash);

    for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
      if (m_isActiveStates[istate]==dcsState) { isDCSActive=true; }
    }
    for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
      if (m_isActiveStatus[istatus]==dcsStatus) { isDCSGood=true; }
    }
    if (!(isDCSActive && isDCSGood)) { return 0.; }
  }

  if (m_useTDAQ && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getTDAQModuleStatus(moduleHash)) { return 0.0; }

  if (m_useDeadMap && SG::ReadCondHandle<PixelModuleData>(m_condKey)->getModuleStatus(moduleHash)) { return 0.0; }

  // TODO!!!  Calculate active fraction from dead map.

  return 1.0;
}

#endif // not SIMULATIONBASE
