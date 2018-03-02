/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SiliconConditionsSvc.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_SiliconConditionsSvc::SCT_SiliconConditionsSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_defaultTemperature{10.},
  m_defaultBiasVoltage{150.},
  m_defaultDepletionVoltage{70.},
  m_useDB{true},
  m_checkGeoModel{true},
  m_forceUseGeoModel{false},
  m_detStore{"DetectorStore", name},
  m_geoModelSvc{"GeoModelSvc", name},
  m_rdbSvc{"RDBAccessSvc", name},
  m_useGeoModel{false},
  m_condKeyHV{"SCT_SiliconBiasVoltCondData"},
  m_condKeyTemp{"SCT_SiliconTempCondData"},
  m_sct_id{nullptr}
{
  declareProperty("Temperature",      m_defaultTemperature     );
  declareProperty("BiasVoltage",      m_defaultBiasVoltage     );
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage);
  declareProperty("UseDB",            m_useDB                  );
  declareProperty("CheckGeoModel",    m_checkGeoModel          );
  declareProperty("ForceUseGeoModel", m_forceUseGeoModel       );
  declareProperty("GeoModelSvc",      m_geoModelSvc            );
  declareProperty("RDBAccessSvc",     m_rdbSvc                 );

  // These will get overwritten if used but give them some initial value anyway.
  m_geoModelTemperature      = m_defaultTemperature;
  m_geoModelBiasVoltage      = m_defaultBiasVoltage;
  m_geoModelDepletionVoltage = m_defaultDepletionVoltage;
}

// Initialize
StatusCode SCT_SiliconConditionsSvc::initialize() {
  ATH_MSG_DEBUG("SCT_SiliconConditionsSvc::initialize()");

  if (m_checkGeoModel or m_forceUseGeoModel) {
    m_useGeoModel = setConditionsFromGeoModel();
    if (m_useGeoModel) {
      ATH_MSG_INFO("Default conditions come from GeoModel.");
    } else {
      ATH_MSG_INFO("GeoModel requests to use Conditions DB.");
    }
  } 
  if (not m_useGeoModel) {
    ATH_MSG_INFO("Will use temperature and voltages from this service (not from GeoModel).");
   
    // Get from Conditions database. Register callback, etc.
    if (m_useDB) {
      ATH_CHECK(m_condKeyHV.initialize());
      ATH_CHECK(m_condKeyTemp.initialize());
      ATH_CHECK(m_detStore->retrieve(m_sct_id, "SCT_ID"));

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
StatusCode SCT_SiliconConditionsSvc::finalize() {
  ATH_MSG_DEBUG("SCT_SiliconConditionsSvc::finalize()");
  return StatusCode::SUCCESS; 
} 

// QueryInterface
StatusCode SCT_SiliconConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if (riid == interfaceID()) {
    *ppvIf = dynamic_cast<ISiliconConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface(riid, ppvIf);
}

// Silicon temperature (by Identifier)
float SCT_SiliconConditionsSvc::temperature(const Identifier& elementId) {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return temperature(elementHash);
}

// Silicon bias voltage (by Identifier)
float SCT_SiliconConditionsSvc::biasVoltage(const Identifier& elementId) {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return biasVoltage(elementHash);
}

// Silicon depletion voltage (by Identifier)
float SCT_SiliconConditionsSvc::depletionVoltage(const Identifier& /*elementId*/) {
  return m_defaultDepletionVoltage;
}

// Silicon temperature (by IdentifierHash)
float SCT_SiliconConditionsSvc::temperature(const IdentifierHash& elementHash) {
  if (m_useDB and not (m_useGeoModel)) {
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
float SCT_SiliconConditionsSvc::biasVoltage(const IdentifierHash& elementHash) {

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
float SCT_SiliconConditionsSvc::depletionVoltage(const IdentifierHash& /*elementHash*/) {
  return m_defaultDepletionVoltage;
}

// DB Callback
StatusCode SCT_SiliconConditionsSvc::callBack(int&, std::list<std::string>&) {
  // Nothing needs to be done
  ATH_MSG_FATAL("SCT_SiliconConditionsSvc::callBack should not be used anymore!");
  return StatusCode::FAILURE;
}

// Has a callback been registered
bool SCT_SiliconConditionsSvc::hasCallBack() {
  // No use of database yet so we always return false.
  return (m_useDB and (not m_useGeoModel));
  //return false;
}

bool 
SCT_SiliconConditionsSvc::setConditionsFromGeoModel()
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

const SCT_DCSFloatCondData* SCT_SiliconConditionsSvc::getCondDataHV() const {
  SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyHV};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    return nullptr;
  }
  return *condData;
}

const SCT_DCSFloatCondData* SCT_SiliconConditionsSvc::getCondDataTemp() const {
  SG::ReadCondHandle<SCT_DCSFloatCondData> condData{m_condKeyTemp};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyTemp.key());
    return nullptr;
  }
  return *condData;
}
