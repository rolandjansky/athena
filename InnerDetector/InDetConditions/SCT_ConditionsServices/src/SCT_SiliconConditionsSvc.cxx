/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SiliconConditionsSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h"


// Constructor
SCT_SiliconConditionsSvc::SCT_SiliconConditionsSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_defaultTemperature(10.),
  m_defaultBiasVoltage(150.),
  m_defaultDepletionVoltage(70.),
  m_useDB(true),
  m_checkGeoModel(true),
  m_forceUseGeoModel(false),
  m_detStore("DetectorStore", name),
  m_sctDCSSvc("InDetSCT_DCSConditionsSvc", name),
  m_geoModelSvc("GeoModelSvc", name),
  m_rdbSvc("RDBAccessSvc", name),
  m_useGeoModel(false)
{
  declareProperty("Temperature",      m_defaultTemperature      ); 
  declareProperty("BiasVoltage",      m_defaultBiasVoltage      ); 
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage ); 
  declareProperty("UseDB",            m_useDB                   ); 
  declareProperty("CheckGeoModel",    m_checkGeoModel           );
  declareProperty("ForceUseGeoModel", m_forceUseGeoModel);
  declareProperty("GeoModelSvc",      m_geoModelSvc             );
  declareProperty("RDBAccessSvc",     m_rdbSvc                  );
  declareProperty("DCSConditionsSvc", m_sctDCSSvc               );

  // These will get overwritten if used but give them some initial value anyway.
  m_geoModelTemperature      =   m_defaultTemperature;
  m_geoModelBiasVoltage      =   m_defaultBiasVoltage;
  m_geoModelDepletionVoltage =   m_defaultDepletionVoltage;
}

// Destructor
SCT_SiliconConditionsSvc::~SCT_SiliconConditionsSvc(){}

// Initialize
StatusCode SCT_SiliconConditionsSvc::initialize(){
  ATH_MSG_DEBUG( "SCT_SiliconConditionsSvc::initialize()" );

  if (m_checkGeoModel or m_forceUseGeoModel) {
    m_useGeoModel = setConditionsFromGeoModel();
    if (m_useGeoModel) {
      ATH_MSG_INFO( "Default conditions come from GeoModel." );
    } else {
      ATH_MSG_INFO( "GeoModel requests to use Conditions DB." );
    }
  } 
  if (!m_useGeoModel) {
    ATH_MSG_INFO( "Will use temperature and voltages from this service (not from GeoModel)." );
   
    // Get from Conditions database. Register callback, etc.
    if(m_useDB){
      ATH_CHECK(m_sctDCSSvc.retrieve()) ;
      ATH_MSG_INFO( "SCTDCSSvc retrieved" );
      ATH_MSG_INFO( "Registering callback." );
      ATH_CHECK( m_detStore->regFcn(&ISCT_ConditionsSvc::fillData,  dynamic_cast<ISCT_ConditionsSvc*>(&*m_sctDCSSvc),
                                    &ISiliconConditionsSvc::callBack, dynamic_cast<ISiliconConditionsSvc *>(this), true));
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
StatusCode SCT_SiliconConditionsSvc::finalize(){
  ATH_MSG_DEBUG( "SCT_SiliconConditionsSvc::finalize()" ); 
  return StatusCode::SUCCESS; 
} 

// QueryInterface
StatusCode SCT_SiliconConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIf){
  if (riid == interfaceID()){
    *ppvIf = dynamic_cast<ISiliconConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );
}

// Silicon temperature (by Identifier)
float SCT_SiliconConditionsSvc::temperature(const Identifier& elementId){
  if(m_useDB && !m_useGeoModel) {
    float temperature = m_sctDCSSvc->sensorTemperature(elementId);
    if (temperature <= -30.){
      ATH_MSG_DEBUG( "Sensor temperature: "<< temperature <<" <  -30 " );
      return m_defaultTemperature; 
    }
    return temperature;
  }
  return m_defaultTemperature; 
}

// Silicon bias voltage (by Identifier)
float SCT_SiliconConditionsSvc::biasVoltage(const Identifier& elementId){
  if(m_useDB && !m_useGeoModel) {
    float hv = m_sctDCSSvc->modHV(elementId); 
    if (hv < 0.){
      ATH_MSG_DEBUG( "HV: "<< hv <<" <  0 " );
      return m_defaultBiasVoltage;
    }
    return hv;
  }
  return m_defaultBiasVoltage;
}

// Silicon depletion voltage (by Identifier)
float SCT_SiliconConditionsSvc::depletionVoltage(const Identifier& /*elementId*/){
  return m_defaultDepletionVoltage;
}

// Silicon temperature (by IdentifierHash)
float SCT_SiliconConditionsSvc::temperature(const IdentifierHash& elementHash){
  if(m_useDB && !m_useGeoModel) {
    float temperature = m_sctDCSSvc->sensorTemperature(elementHash);
    if (temperature <= -30.){
      ATH_MSG_DEBUG( "Sensor temperature: "<< temperature <<" <  -30 ");
      return m_defaultTemperature; 
    }
    return m_sctDCSSvc->sensorTemperature(elementHash);
  }
  return m_defaultTemperature;
}

// Silicon bias voltage (by IdentifierHash)
float SCT_SiliconConditionsSvc::biasVoltage(const IdentifierHash& elementHash){

  if(m_useDB && !m_useGeoModel) {
    float hv = m_sctDCSSvc->modHV(elementHash);
    if (hv < 0.){
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "HV: "<< hv <<" <  0 " << endmsg;
       return m_defaultBiasVoltage;
    }
    return hv;
  }
  return m_defaultBiasVoltage;
}

// Silicon deplition voltage (by IdentifierHash)
float SCT_SiliconConditionsSvc::depletionVoltage(const IdentifierHash& /*elementHash*/){
  return m_defaultDepletionVoltage;
}

// DB Callback
StatusCode SCT_SiliconConditionsSvc::callBack(int&, std::list<std::string>&){
  // Nothing needs to be done
  return StatusCode::SUCCESS;
}

// Has a callback been registered
bool SCT_SiliconConditionsSvc::hasCallBack(){
  // No use of database yet so we always return false.
  return (!m_useGeoModel && m_useDB);
  //return false;
}

bool 
SCT_SiliconConditionsSvc::setConditionsFromGeoModel()
{
  bool conditionsPresent = false;
  bool useCondDB = false;
   
  if (m_rdbSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Could not locate RDBAccessSvc" << endmsg;
    return false;
  }

  if (m_geoModelSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endmsg;
    return false;
  }
  DecodeVersionKey versionKey(&*m_geoModelSvc, "SCT");
  ATH_MSG_DEBUG( "Checking GeoModel Version Tag: "<<  versionKey.tag() << " at Node: " << versionKey.node() );

  IRDBRecordset_ptr  sctConditionsSet = m_rdbSvc->getRecordsetPtr("SctConditions",  versionKey.tag(), versionKey.node());
  if (sctConditionsSet->size()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Default conditions available from GeoModel."  << endmsg;
    const IRDBRecord * defaultConditions = (*sctConditionsSet)[0];
    m_geoModelTemperature = defaultConditions->getDouble("TEMPERATURE");
    m_geoModelBiasVoltage = defaultConditions->getDouble("BIASVOLT");
    m_geoModelDepletionVoltage = defaultConditions->getDouble("DEPLETIONVOLT");
    conditionsPresent = true;
    // If m_forceUseGeoModel set then we ignore the USECONDDB column and
    // keep useCondDB = false
    if (!m_forceUseGeoModel && !defaultConditions->isFieldNull("USECONDDB")) { // USECONDDB Not present in old SCT tables 
      useCondDB = (defaultConditions->getInt("USECONDDB"));
    }
  } else {
    ATH_MSG_WARNING( "Default conditions NOT available in GeoModel database. Using old GeoModel defaults" );
    // These are pre DC3 geometries. Probably never will be used.
    m_geoModelTemperature = -7;
    m_geoModelBiasVoltage = 100;
    m_geoModelDepletionVoltage = 20; 
    conditionsPresent = true; 
  }

  return (!useCondDB && conditionsPresent);

}
