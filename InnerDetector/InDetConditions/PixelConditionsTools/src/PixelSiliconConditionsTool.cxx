/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsTool.h"

PixelSiliconConditionsTool::PixelSiliconConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_pixid(0),
  m_useDB(true),
  m_defaultTemperature(-5.0),
  m_defaultBiasVoltage(150.0),
  m_defaultDepletionVoltage(80.0),
  m_condKeyHV("PixelDCSHVCondData"),
  m_condKeyTemp("PixelDCSTemp0CondData")
{
  declareProperty("UseDB",       m_useDB); 
  declareProperty("Temperature", m_defaultTemperature, "Default temperature in Celcius."); 
  declareProperty("BiasVoltage", m_defaultBiasVoltage, "Default bias voltage in Volt." ); 
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage, "Default depletion voltage in Volt."); 
}

StatusCode PixelSiliconConditionsTool::initialize() {
  ATH_MSG_INFO("PixelSiliconConditionsTool::initialize()");

  if (m_useDB) {
    ATH_CHECK(m_condKeyHV.initialize());
    ATH_CHECK(m_condKeyTemp.initialize());
    ATH_CHECK(detStore()->retrieve(m_pixid,"PixelID"));
  }

  return StatusCode::SUCCESS; 
}

StatusCode PixelSiliconConditionsTool::finalize() {
  ATH_MSG_INFO("PixelSiliconConditionsTool::finalize()");
  return StatusCode::SUCCESS; 
} 

float PixelSiliconConditionsTool::temperature(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return temperature(elementHash);
}

float PixelSiliconConditionsTool::biasVoltage(const Identifier& elementId) const {
  const IdentifierHash elementHash = m_pixid->wafer_hash(elementId);
  return biasVoltage(elementHash);
}

float PixelSiliconConditionsTool::depletionVoltage(const Identifier& /*elementId*/) const {
  return m_defaultDepletionVoltage;
}

float PixelSiliconConditionsTool::temperature(const IdentifierHash& elementHash) const {
  if (m_useDB) {
    const PixelDCSCondData* data(getCondDataTemp());
    if (data==nullptr) { return m_defaultTemperature; }
    float temperature = m_defaultTemperature;
    if (not data->getValue(elementHash, temperature)) { return m_defaultTemperature; }
    return temperature;
  }
  return m_defaultTemperature;
}

float PixelSiliconConditionsTool::biasVoltage(const IdentifierHash& elementHash) const {
  if (m_useDB) {
    const PixelDCSCondData* data(getCondDataHV());
    if (data==nullptr) { return m_defaultBiasVoltage; }
    float hv = m_defaultBiasVoltage;
    if (not data->getValue(elementHash, hv)) { return m_defaultBiasVoltage; }
    return hv;
  }
  return m_defaultBiasVoltage;
}

float PixelSiliconConditionsTool::depletionVoltage(const IdentifierHash& /*elementHash*/) const {
  return m_defaultDepletionVoltage;
}

const PixelDCSCondData* PixelSiliconConditionsTool::getCondDataHV() const {
  SG::ReadCondHandle<PixelDCSCondData> condData(m_condKeyHV);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    return nullptr;
  }
  return *condData;
}

const PixelDCSCondData* PixelSiliconConditionsTool::getCondDataTemp() const {
  SG::ReadCondHandle<PixelDCSCondData> condData(m_condKeyTemp);
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyTemp.key());
    return nullptr;
  }
  return *condData;
}
