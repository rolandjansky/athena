/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsSvc.h"



#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsServices/IPixelDCSSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"


PixelSiliconConditionsSvc::PixelSiliconConditionsSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_pixelDCSSvc("PixelDCSSvc", name),
  m_geoModelSvc("GeoModelSvc", name),
  m_rdbSvc("RDBAccessSvc", name),
  m_pixman(0),
  m_pixid(0),
  m_useGeoModel(false),
  m_checkGeoModel(false),
  m_forceUseGeoModel(false)
{

  m_defaultTemperature = -5.;
  m_defaultBiasVoltage = 150.;
  m_defaultDepletionVoltage = 80.;
  m_useDBForHV = true;
  m_useDBForTemperature = true;

  declareProperty("Temperature", m_defaultTemperature, "Default temperature in Celcius."); 
  declareProperty("BiasVoltage", m_defaultBiasVoltage, "Default bias voltage in Volt." ); 
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage, "Default depletion voltage in Volt."); 
  declareProperty("UseDBForHV", m_useDBForHV); 
  declareProperty("UseDBForTemperature", m_useDBForTemperature); 
  declareProperty("CheckGeoModel", m_checkGeoModel);
  declareProperty("ForceUseGeoModel", m_forceUseGeoModel);
  declareProperty("PixelDCSSvc",m_pixelDCSSvc);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("RDBAccessSvc", m_rdbSvc);

  // These will get overwritten if used but give them some initial value anyway.
  m_geoModelTemperature      =   m_defaultTemperature;
  m_geoModelBiasVoltage      =   m_defaultBiasVoltage;
  m_geoModelDepletionVoltage =   m_defaultDepletionVoltage;

}

PixelSiliconConditionsSvc::~PixelSiliconConditionsSvc()
{
}


StatusCode PixelSiliconConditionsSvc::initialize()
{

   msg(MSG::INFO) << " Entering PixelSiliconConditionsSvc::initialize()" << endreq;


  //Retrieve Detector Store
  StatusCode sc = m_detStore.retrieve();
  if (!sc.isSuccess() || 0 == m_detStore)  {
    msg(MSG::FATAL) << "Unable to retrieve detector store" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Detector store retrieved" << endreq;

  // Get the geometry 
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 
  if(StatusCode::SUCCESS !=m_detStore->retrieve(m_pixman, "Pixel") || m_pixman==0){
    msg(MSG::FATAL) << "Could not find Pixel manager "<<endreq; 
    return StatusCode::FAILURE; 
  }
  else{ 
    itermin = m_pixman->getDetectorElementBegin(); 
    itermax = m_pixman->getDetectorElementEnd(); 

    if( StatusCode::SUCCESS != m_detStore->retrieve( m_pixid,"PixelID") ){
      ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
      return StatusCode::FAILURE;
    }
  } 


  if (m_checkGeoModel || m_forceUseGeoModel) {
    m_useGeoModel = setConditionsFromGeoModel();
    if (m_useGeoModel) {
      msg(MSG::INFO) << "Default conditions come from GeoModel." << endreq;
    } else {
      msg(MSG::INFO) << "GeoModel requests to use Conditions DB." << endreq;
    }
  } 
  if (!m_useGeoModel) {
    msg(MSG::INFO) << "Will use temperature and voltages from this service (not from GeoModel)." << endreq;
  
    if(m_useDBForHV || m_useDBForTemperature){
      if (StatusCode::SUCCESS!=m_pixelDCSSvc.retrieve()) {
	msg(MSG::FATAL) << "Unable to retrieve PixelDCSSvc" << endreq;
	return StatusCode::FAILURE;
      } 
      msg(MSG::INFO) << "PixelDCSSvc retrieved" << endreq;
      
      msg(MSG::INFO) << "Registering callback." << endreq;
      StatusCode sc = m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack,  dynamic_cast<IPixelDCSSvc*>(&*m_pixelDCSSvc),
					 &ISiliconConditionsSvc::callBack, dynamic_cast<ISiliconConditionsSvc *>(this), true);
      if (sc.isFailure()) {
	msg(MSG::ERROR)<< "Could not register callback." << endreq;
	return sc;
      }
    }
  } else {
    m_defaultTemperature      = m_geoModelTemperature;
    m_defaultBiasVoltage      = m_geoModelBiasVoltage;
    m_defaultDepletionVoltage = m_geoModelDepletionVoltage;
  }
  return StatusCode::SUCCESS; 

}


StatusCode PixelSiliconConditionsSvc::finalize(){
  msg(MSG::INFO) << "Entering PixelSiliconConditionsSvc::finalize()" << endreq; 
  return StatusCode::SUCCESS; 
} 



StatusCode PixelSiliconConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIf){
  if (riid == interfaceID()){
    *ppvIf = dynamic_cast<ISiliconConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}


float PixelSiliconConditionsSvc::temperature(const Identifier & detectorElement){

  if(m_useDBForTemperature && !m_useGeoModel) return m_pixelDCSSvc->getTemperature(detectorElement);

  return m_defaultTemperature;
}

float PixelSiliconConditionsSvc::biasVoltage(const Identifier & detectorElement){

  if(m_useDBForHV && !m_useGeoModel) return m_pixelDCSSvc->getHV(detectorElement);

  return m_defaultBiasVoltage;
}

float PixelSiliconConditionsSvc::depletionVoltage(const Identifier & /*detectorElement*/){

  return m_defaultDepletionVoltage;
}

float PixelSiliconConditionsSvc::temperature(const IdentifierHash & detectorElement){

  if(m_useDBForTemperature && !m_useGeoModel) return m_pixelDCSSvc->getTemperature(detectorElement);

  return m_defaultTemperature;
}

float PixelSiliconConditionsSvc::biasVoltage(const IdentifierHash & detectorElement){

  if(m_useDBForHV && !m_useGeoModel) return m_pixelDCSSvc->getHV(detectorElement);

  return m_defaultBiasVoltage;

}

float PixelSiliconConditionsSvc::depletionVoltage(const IdentifierHash & /*detectorElement*/){

  return m_defaultDepletionVoltage;
}

/// IOV CallBack
StatusCode PixelSiliconConditionsSvc::callBack(IOVSVC_CALLBACK_ARGS)
{
  // Nothing needs to be done
  return StatusCode::SUCCESS;
}


bool PixelSiliconConditionsSvc::hasCallBack()
{
  return (!m_useGeoModel && (m_useDBForHV || m_useDBForTemperature));
}


bool 
PixelSiliconConditionsSvc::setConditionsFromGeoModel()
{
  bool conditionsPresent = false;
  bool useCondDB = false;
   
  if (m_rdbSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Could not locate RDBAccessSvc" << endreq;
    return false;
  }

  if (m_geoModelSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return false;
  }

  m_rdbSvc->connect();

  DecodeVersionKey versionKey(&*m_geoModelSvc, "Pixel");
  msg(MSG::DEBUG) << "Checking GeoModel Version Tag: "<<  versionKey.tag() << " at Node: " << versionKey.node() << endreq;

  const IRDBRecordset * pixelConditionsSet = m_rdbSvc->getRecordset("PixelConditions",  versionKey.tag(), versionKey.node());
  if (pixelConditionsSet->size()) {
    msg(MSG::DEBUG) << "Default conditions available from GeoModel."  << endreq;
    const IRDBRecord * defaultConditions = (*pixelConditionsSet)[0];
    m_geoModelTemperature = defaultConditions->getDouble("TEMPERATURE");
    m_geoModelBiasVoltage = defaultConditions->getDouble("BIASVOLT");
    m_geoModelDepletionVoltage = defaultConditions->getDouble("DEPLETIONVOLT");
    conditionsPresent = true;
    // If m_forceUseGeoModel set then we ignore the USECONDDB column and
    // keep useCondDB = false
    if (!m_forceUseGeoModel) {
      useCondDB = (defaultConditions->getInt("USECONDDB"));
    }
  } else {
    // PixelConditions table doesn't exist so try old table
    const IRDBRecordset * plor = m_rdbSvc->getRecordset("PLOR",  versionKey.tag(), versionKey.node());
    if (plor->size() != 0) {
      msg(MSG::DEBUG) << "Default conditions available from GeoModel."  << endreq;
      m_geoModelTemperature = (*plor)[0]->getDouble("TEMPC");
      m_geoModelBiasVoltage = (*plor)[0]->getDouble("VOLTAGE");
      m_geoModelDepletionVoltage = 0;
      conditionsPresent = true;
    }
  }

  if (!conditionsPresent) { // Shouldn't normally be the case.
    msg(MSG::INFO) << "Default conditions not provided by GeoModel."  << endreq;
  }
 
  m_rdbSvc->disconnect();

  return (!useCondDB && conditionsPresent);


}
