/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsSvc.h"

PixelSiliconConditionsSvc::PixelSiliconConditionsSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_pixelDCSSvc("PixelDCSSvc", name),
  m_pixman(0),
  m_pixid(0),
  m_useDB(true),
  m_defaultTemperature(-5.0),
  m_defaultBiasVoltage(150.0),
  m_defaultDepletionVoltage(80.0)
{
  declareProperty("PixelDCSSvc",m_pixelDCSSvc);
  declareProperty("UseDB",       m_useDB); 
  declareProperty("Temperature", m_defaultTemperature, "Default temperature in Celcius."); 
  declareProperty("BiasVoltage", m_defaultBiasVoltage, "Default bias voltage in Volt." ); 
  declareProperty("DepletionVoltage", m_defaultDepletionVoltage, "Default depletion voltage in Volt."); 
}

PixelSiliconConditionsSvc::~PixelSiliconConditionsSvc() {}

StatusCode PixelSiliconConditionsSvc::initialize() {

  ATH_MSG_INFO("PixelSiliconConditionsSvc::initialize()");

  CHECK(m_detStore.retrieve());

  CHECK(m_detStore->retrieve(m_pixman,"Pixel"));

  CHECK(m_detStore->retrieve(m_pixid,"PixelID"));

  if (m_useDB) {
    CHECK(m_pixelDCSSvc.retrieve());
    ATH_MSG_INFO("PixelDCSSvc retrieved");
    ATH_MSG_INFO("Registering callback.");
    CHECK(m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack,dynamic_cast<IPixelDCSSvc*>(&*m_pixelDCSSvc),&ISiliconConditionsSvc::callBack,dynamic_cast<ISiliconConditionsSvc*>(this),true));
  }
  return StatusCode::SUCCESS; 
}

StatusCode PixelSiliconConditionsSvc::finalize() {
  ATH_MSG_INFO("PixelSiliconConditionsSvc::finalize()");
  return StatusCode::SUCCESS; 
} 

StatusCode PixelSiliconConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if (riid == interfaceID()){
    *ppvIf = dynamic_cast<ISiliconConditionsSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}

float PixelSiliconConditionsSvc::temperature(const Identifier & detectorElement) {
  if (m_useDB) { return m_pixelDCSSvc->getTemperature(detectorElement); }
  return m_defaultTemperature;
}

float PixelSiliconConditionsSvc::biasVoltage(const Identifier & detectorElement) {
  if (m_useDB) { return m_pixelDCSSvc->getHV(detectorElement); }
  return m_defaultBiasVoltage;
}

float PixelSiliconConditionsSvc::depletionVoltage(const Identifier & /*detectorElement*/) {
  return m_defaultDepletionVoltage;
}

float PixelSiliconConditionsSvc::temperature(const IdentifierHash & detectorElement) {
  if (m_useDB) { return m_pixelDCSSvc->getTemperature(detectorElement); }
  return m_defaultTemperature;
}

float PixelSiliconConditionsSvc::biasVoltage(const IdentifierHash & detectorElement) {
  if (m_useDB) { return m_pixelDCSSvc->getHV(detectorElement); }
  return m_defaultBiasVoltage;
}

float PixelSiliconConditionsSvc::depletionVoltage(const IdentifierHash & /*detectorElement*/) {
  return m_defaultDepletionVoltage;
}

StatusCode PixelSiliconConditionsSvc::callBack(IOVSVC_CALLBACK_ARGS) {
  return StatusCode::SUCCESS;
}

bool PixelSiliconConditionsSvc::hasCallBack() {
  return m_useDB;
}

