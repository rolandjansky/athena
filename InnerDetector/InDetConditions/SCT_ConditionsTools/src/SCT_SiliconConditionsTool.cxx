/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SiliconConditionsTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_SiliconConditionsTool::SCT_SiliconConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent)
{
  // These will get overwritten if used but give them some initial value anyway.
  m_geoModelTemperature      = m_defaultTemperature;
  m_geoModelBiasVoltage      = m_defaultBiasVoltage;
  m_geoModelDepletionVoltage = m_defaultDepletionVoltage;
}

// Initialize
StatusCode SCT_SiliconConditionsTool::initialize() {
  ATH_MSG_DEBUG("SCT_SiliconConditionsTool::initialize()");

  if (m_checkGeoModel or m_forceUseGeoModel) {
    m_useGeoModel = setConditionsFromGeoModel();
    if (m_useGeoModel) {
      ATH_MSG_INFO("Default conditions come from GeoModel.");
    } else {
      ATH_MSG_INFO("GeoModel requests to use Conditions DB.");
    }
  } 

  ATH_CHECK(m_condKeyHV.initialize((not m_useGeoModel) and m_useDB));
  ATH_CHECK(m_condKeyTemp.initialize((not m_useGeoModel) and m_useDB));

  if (not m_useGeoModel) {
    ATH_MSG_INFO("Will use temperature and voltages from this service (not from GeoModel).");
   
    // Get from Conditions database. Register callback, etc.
    if (m_useDB) {
      ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));

      ATH_MSG_INFO("SCTDCSSvc retrieved");
    }
  } else {
    // Otherwise we use the GeoModel values
    m_defaultTemperature      = m_geoModelTemperature;
    m_defaultBiasVoltage      = m_geoModelBiasVoltage;
    m_defaultDepletionVoltage = m_geoModelDepletionVoltage;
  } 
  return StatusCode::SUCCESS; 
}

// Finalize
StatusCode SCT_SiliconConditionsTool::finalize() {
  ATH_MSG_DEBUG("SCT_SiliconConditionsTool::finalize()");
  return StatusCode::SUCCESS; 
} 

// Silicon temperature (by Identifier)
float SCT_SiliconConditionsTool::temperature(const Identifier& elementId) const {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return temperature(elementHash);
}

// Silicon bias voltage (by Identifier)
float SCT_SiliconConditionsTool::biasVoltage(const Identifier& elementId) const {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return biasVoltage(elementHash);
}

// Silicon depletion voltage (by Identifier)
float SCT_SiliconConditionsTool::depletionVoltage(const Identifier& /*elementId*/) const {
  return m_defaultDepletionVoltage;
}

// Silicon temperature (by IdentifierHash)
float SCT_SiliconConditionsTool::temperature(const IdentifierHash& elementHash) const {
  if (m_useDB and (not m_useGeoModel)) {
    const SCT_DCSFloatCondData* data{getCondDataTemp()};
    if (data==nullptr) return m_defaultTemperature;
    float temperature{m_defaultTemperature};
    if (not data->getValue(elementHash, temperature)) return m_defaultTemperature;
    if (temperature <= -30.){
      ATH_MSG_DEBUG( "Sensor temperature: "<< temperature <<" <  -30 ");
      return m_defaultTemperature; 
    }
    return temperature;
  }
  return m_defaultTemperature;
}

// Silicon bias voltage (by IdentifierHash)
float SCT_SiliconConditionsTool::biasVoltage(const IdentifierHash& elementHash) const {

  if (m_useDB and (not m_useGeoModel)) {
    const SCT_DCSFloatCondData* data{getCondDataHV()};
    if (data==nullptr) return m_defaultBiasVoltage;
    float hv{m_defaultBiasVoltage};
    if (not data->getValue(elementHash, hv)) return m_defaultBiasVoltage;
    if (hv < 0.) {
      ATH_MSG_DEBUG("HV: "<< hv <<" <  0 ");
      return m_defaultBiasVoltage;
    }
    return hv;
  }
  return m_defaultBiasVoltage;
}

// Silicon deplition voltage (by IdentifierHash)
float SCT_SiliconConditionsTool::depletionVoltage(const IdentifierHash& /*elementHash*/) const {
  return m_defaultDepletionVoltage;
}

bool 
SCT_SiliconConditionsTool::setConditionsFromGeoModel()
{
  bool conditionsPresent{false};
  bool useCondDB{false};
   
  if (m_rdbSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not locate RDBAccessSvc");
    return false;
  }

  if (m_geoModelSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not locate GeoModelSvc");
    return false;
  }
  DecodeVersionKey versionKey{&*m_geoModelSvc, "SCT"};
  ATH_MSG_DEBUG("Checking GeoModel Version Tag: "<<  versionKey.tag() << " at Node: " << versionKey.node());

  IRDBRecordset_ptr sctConditionsSet{m_rdbSvc->getRecordsetPtr("SctConditions", versionKey.tag(), versionKey.node())};
  if (sctConditionsSet->size()) {
    ATH_MSG_DEBUG("Default conditions available from GeoModel.");
    const IRDBRecord* defaultConditions{(*sctConditionsSet)[0]};
    m_geoModelTemperature = defaultConditions->getDouble("TEMPERATURE");
    m_geoModelBiasVoltage = defaultConditions->getDouble("BIASVOLT");
    m_geoModelDepletionVoltage = defaultConditions->getDouble("DEPLETIONVOLT");
    conditionsPresent = true;
    // If m_forceUseGeoModel set then we ignore the USECONDDB column and
    // keep useCondDB = false
    if ((not m_forceUseGeoModel) and (not defaultConditions->isFieldNull("USECONDDB"))) { // USECONDDB Not present in old SCT tables
      useCondDB = (defaultConditions->getInt("USECONDDB"));
    }
  } else {
    ATH_MSG_WARNING("Default conditions NOT available in GeoModel database. Using old GeoModel defaults");
    // These are pre DC3 geometries. Probably never will be used.
    m_geoModelTemperature = -7;
    m_geoModelBiasVoltage = 100;
    m_geoModelDepletionVoltage = 20; 
    conditionsPresent = true; 
  }

  return ((not useCondDB) and conditionsPresent);

}

const SCT_DCSFloatCondData* SCT_SiliconConditionsTool::getCondDataHV() const {
  SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyHV};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    return nullptr;
  }
  return *condData;
}

const SCT_DCSFloatCondData* SCT_SiliconConditionsTool::getCondDataTemp() const {
  SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyTemp};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyTemp.key());
    return nullptr;
  }
  return *condData;
}
