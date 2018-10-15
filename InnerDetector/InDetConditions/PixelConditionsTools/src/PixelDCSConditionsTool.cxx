/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMULATIONBASE
#include "PixelDCSConditionsTool.h"

PixelDCSConditionsTool::PixelDCSConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_pixid(nullptr),
  m_useDB(true),
  m_isDATA(true),
  m_defaultTemperature(-7.0),
  m_defaultBiasVoltage(150.0),
  m_defaultDepletionVoltage(40.0)
{
  declareProperty("UseDB",       m_useDB); 
  declareProperty("IsDATA",      m_isDATA); 
  declareProperty("Temperature", m_defaultTemperature, "Default temperature in Celcius."); 
  declareProperty("BiasVoltage", m_defaultBiasVoltage, "Default bias voltage in Volt." ); 
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage, "Default depletion voltage in Volt."); 
}

StatusCode PixelDCSConditionsTool::initialize() {
  ATH_MSG_DEBUG("PixelDCSConditionsTool::initialize()");

  if (m_useDB) {
    ATH_CHECK(m_condKeyHV.initialize());
    ATH_CHECK(m_condKeyTemp.initialize());
    if (m_isDATA) {
      ATH_CHECK(m_condKeyState.initialize());
      ATH_CHECK(m_condKeyStatus.initialize());
    }
  }
  ATH_CHECK(detStore()->retrieve(m_pixid,"PixelID"));

  return StatusCode::SUCCESS; 
}

StatusCode PixelDCSConditionsTool::finalize() {
  ATH_MSG_DEBUG("PixelDCSConditionsTool::finalize()");
  return StatusCode::SUCCESS; 
} 

float PixelDCSConditionsTool::temperature(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return temperature(elementHash);
}

float PixelDCSConditionsTool::biasVoltage(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return biasVoltage(elementHash);
}

float PixelDCSConditionsTool::depletionVoltage(const Identifier& /*elementId*/) const {
  return m_defaultDepletionVoltage;
}

std::string PixelDCSConditionsTool::PixelFSMState(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return PixelFSMState(elementHash);
}

std::string PixelDCSConditionsTool::PixelFSMStatus(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return PixelFSMStatus(elementHash);
}

float PixelDCSConditionsTool::temperature(const IdentifierHash& elementHash) const {
  if (m_useDB) {
    const PixelDCSConditionsData* data(getCondDataTemp());
    if (data==nullptr) { return m_defaultTemperature; }
    float temperature = m_defaultTemperature;
    if (not data->getValue(elementHash, temperature)) { return m_defaultTemperature; }
    if (temperature>100.0 or temperature<-80.0) { temperature = m_defaultTemperature; }
    return temperature;
  }
  return m_defaultTemperature;
}

float PixelDCSConditionsTool::biasVoltage(const IdentifierHash& elementHash) const {
  if (m_useDB) {
    const PixelDCSConditionsData* data(getCondDataHV());
    if (data==nullptr) { return m_defaultBiasVoltage; }
    float hv = m_defaultBiasVoltage;
    if (not data->getValue(elementHash, hv)) { return m_defaultBiasVoltage; }
    if (hv>1000.0 or hv<-1000.0) { hv = m_defaultBiasVoltage; }
    return hv;
  }
  return m_defaultBiasVoltage;
}

float PixelDCSConditionsTool::depletionVoltage(const IdentifierHash& /*elementHash*/) const {
  return m_defaultDepletionVoltage;
}

std::string PixelDCSConditionsTool::PixelFSMState(const IdentifierHash& elementHash) const {
  std::string defaultState = "READY";
  if (!m_isDATA) { return defaultState; }
  if (m_useDB) {
    const PixelDCSConditionsData* data(getCondDataState());
    if (data==nullptr) { return defaultState; }
    std::string fsmstate = defaultState;
    if (not data->getValue(elementHash, fsmstate)) { return defaultState; }
    return fsmstate;
  }
  return defaultState;
}

std::string PixelDCSConditionsTool::PixelFSMStatus(const IdentifierHash& elementHash) const {
  std::string defaultStatus = "OK";
  if (!m_isDATA) { return defaultStatus; }
  if (m_useDB) {
    const PixelDCSConditionsData* data(getCondDataStatus());
    if (data==nullptr) { return defaultStatus; }
    std::string fsmstatus = defaultStatus;
    if (not data->getValue(elementHash, fsmstatus)) { return defaultStatus; }
    return fsmstatus;
  }
  return defaultStatus;
}

const PixelDCSConditionsData* PixelDCSConditionsTool::getCondDataHV() const {
  SG::ReadCondHandle<PixelDCSConditionsData> condData(m_condKeyHV);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    return nullptr;
  }
  return *condData;
}

const PixelDCSConditionsData* PixelDCSConditionsTool::getCondDataTemp() const {
  SG::ReadCondHandle<PixelDCSConditionsData> condData(m_condKeyTemp);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyTemp.key());
    return nullptr;
  }
  return *condData;
}

const PixelDCSConditionsData* PixelDCSConditionsTool::getCondDataState() const {
  SG::ReadCondHandle<PixelDCSConditionsData> condData(m_condKeyState);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyState.key());
    return nullptr;
  }
  return *condData;
}

const PixelDCSConditionsData* PixelDCSConditionsTool::getCondDataStatus() const {
  SG::ReadCondHandle<PixelDCSConditionsData> condData(m_condKeyStatus);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyStatus.key());
    return nullptr;
  }
  return *condData;
}

#endif // not SIMULATIONBASE
