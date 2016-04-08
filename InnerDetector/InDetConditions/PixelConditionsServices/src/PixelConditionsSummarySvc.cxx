/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsSummarySvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsServices/IPixelDCSSvc.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "PixelConditionsData/SpecialPixelMap.h"
#include "PixelConditionsServices/IPixelTDAQSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "PixelGeoModel/IBLParameterSvc.h"

PixelConditionsSummarySvc::PixelConditionsSummarySvc(const std::string& name, ISvcLocator* sl)
  :
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_pixelID(0),
  m_specialPixelMapKey("SpecialPixelMap"),
  m_specialPixelMap(0),
  m_specialPixelMapSvc("SpecialPixelMapSvc", name),
  m_pixelDCSSvc("PixelDCSSvc", name),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_pixelBSErrorsSvc("PixelByteStreamErrorsSvc", name),
  m_pixelTDAQSvc("PixelTDAQSvc", name),
  m_useSpecialPixelMap(true),
  m_useDCS(false),
  m_useBS(false),
  m_useTDAQ(false)
{
  m_isActiveStatus.push_back("OK");
  m_isActiveStates.push_back("READY");

  declareProperty("PixelMapKey", m_specialPixelMapKey, "StoreGate key of default special pixel map");
  declareProperty("IsActiveStatus", m_isActiveStatus);
  declareProperty("IsActiveStates", m_isActiveStates);

  declareProperty("UseSpecialPixelMap", m_useSpecialPixelMap, "Switch for usage of special pixel map");
  declareProperty("UseDCS", m_useDCS, "Switch for usage of DCS");
  declareProperty("UseByteStream", m_useBS, "Switch for usage of the ByteStream error service");
  declareProperty("UseTDAQ", m_useTDAQ, "Switch for usage of TDAQ");
  declareProperty("TDAQSvcName", m_pixelTDAQSvc, "Instance of the PixelTDAQSvc");

  declareProperty("StoreGateSvc", m_detStore);
}

PixelConditionsSummarySvc::~PixelConditionsSummarySvc(){}


//======== Callback


StatusCode PixelConditionsSummarySvc::specialPixelMapCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){

  if (msgLvl(MSG::INFO))
    msg(MSG::INFO) << "Special pixel map callback for key " << *keys.begin() << " number " << I << endreq;

  StatusCode sc = m_detStore->retrieve(m_specialPixelMap, m_specialPixelMapKey);

  if( !sc.isSuccess() ){
    msg(MSG::ERROR) << "Unable to retrieve special pixel map" << endreq;
  }

  return sc;
}


//======== queryInterface, initialize, finalize


StatusCode PixelConditionsSummarySvc::initialize(){
  msg(MSG::INFO) << "Initializing PixelConditionsSummarySvc" << endreq;


  StatusCode sc = setProperties();
  if( !sc.isSuccess() ){
    msg(MSG::FATAL) << "Unable to set properties" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_IBLParameterSvc.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Could not retrieve IBLParameterSvc" << endreq;
  }
  else {
    m_IBLParameterSvc->setBoolParameters(m_useSpecialPixelMap,"EnableSpecialPixels");
  }
  sc = m_detStore.retrieve();
  if( !sc.isSuccess() ){
    msg(MSG::FATAL) << "Unable to retrieve detector store" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_useDCS){
    if (StatusCode::SUCCESS!=m_pixelDCSSvc.retrieve()) {
      msg(MSG::FATAL) << "Unable to retrieve PixelDCSSvc" << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "PixelDCSSvc retrieved" << endreq;
  }

  if(m_useBS){
    if (StatusCode::SUCCESS!=m_pixelBSErrorsSvc.retrieve()) {
      msg(MSG::FATAL) << "Unable to retrieve PixelBSErrorSvc" << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "PixelBSErrorsSvc retrieved" << endreq;
  }

  sc = m_detStore->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  if(m_useSpecialPixelMap){
    sc = m_specialPixelMapSvc.retrieve();

    if( !sc.isSuccess() ){
      msg(MSG::FATAL) << "Unable to retrieve SpecialPixelMapSvc" << endreq;
      return StatusCode::FAILURE;
    }

    if(m_specialPixelMapSvc->getNPixelMaps() == 0){
      msg(MSG::WARNING) << "No special pixel maps configured" << endreq;
      msg(MSG::WARNING) << "Disabling use of special pixel maps" << endreq;
      m_useSpecialPixelMap = false;
    }
    else{
      sc = m_detStore->regFcn(&ISpecialPixelMapSvc::IOVCallBack, dynamic_cast<ISpecialPixelMapSvc*>(m_specialPixelMapSvc.operator->()),
			      &PixelConditionsSummarySvc::specialPixelMapCallBack, this);

      if( !sc.isSuccess()){
	msg(MSG::FATAL) << "Unable to register callback for special pixel map" << endreq;
	return StatusCode::FAILURE;
      }
    }
  }

  if(m_useTDAQ){
    sc = m_pixelTDAQSvc.retrieve();

    if( !sc.isSuccess() ){
      ATH_MSG_FATAL("Unable to retrieve PixelTDAQSvc");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelConditionsSummarySvc::finalize(){
  msg(MSG::INFO) << "Finalizing PixelConditionsSummarySvc" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode PixelConditionsSummarySvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< PixelConditionsSummarySvc* > (this);
  }
  else if(IInDetConditionsSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast< IInDetConditionsSvc* > (this);
  }
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();

  return StatusCode::SUCCESS;
}



//======== isActive methods


bool PixelConditionsSummarySvc::isActive(const Identifier & elementId, const InDetConditions::Hierarchy h){

  //Identifier moduleID       = m_pixelID->wafer_id(elementId);
  IdentifierHash moduleHash = m_pixelID->wafer_hash(elementId);

  if(m_useBS && !m_pixelBSErrorsSvc->isActive(moduleHash)) return false;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(moduleHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }

    if(!isDCSActive) return isDCSActive;

  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(moduleHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    unsigned int pixel = 0;
    unsigned int mchips = m_specialPixelMap->module(moduleHash)->chipsPerModule(); // number of chips 
    unsigned int mtype = m_specialPixelMap->module(moduleHash)->chipType(); // type of chips 
    mtype +=10*mchips; // 10*mchips + mtype for encodePixelID 

    switch (h) {

    case InDetConditions::DEFAULT:
    case InDetConditions::PIXEL_MODULE:
      return !(m_specialPixelMap->module(moduleHash)->moduleStatus() & (1 << 1));

    case InDetConditions::PIXEL_CHIP:
      getID(elementId, pixel,mtype);
      return !(m_specialPixelMap->module(moduleHash)->chipStatus(pixel % (mchips>2?mchips:2)) & (1 << 1));

    case InDetConditions::PIXEL_PIXEL:
      getID(elementId, pixel,mtype);
      return !m_specialPixelMap->module(moduleHash)->pixelStatusBit(pixel, 1);

    default:
      return false;
    }

  }

  return true;
}


bool PixelConditionsSummarySvc::isActive(const IdentifierHash & elementHash){

  if(m_useBS && !m_pixelBSErrorsSvc->isActive(elementHash)) return false;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);


    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }


    if(!isDCSActive) return isDCSActive;

  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(elementHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    return !(m_specialPixelMap->module(elementHash)->moduleStatus() & (1 << 1));

  }

  return true;
}

bool PixelConditionsSummarySvc::isActive(const IdentifierHash & elementHash, const Identifier & elementId){

  if(m_useBS && !m_pixelBSErrorsSvc->isActive(elementHash)) return false;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }

    if(!isDCSActive) return isDCSActive;

  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(elementHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    unsigned int pixel = 0;
    unsigned int mchips = m_specialPixelMap->module(elementHash)->chipsPerModule();
    unsigned int mtype = m_specialPixelMap->module(elementHash)->chipType();
    mtype +=mchips*10; // mchips*10+mtype for encodePixelID 

    getID(elementId, pixel, mtype);

    return !m_specialPixelMap->module(elementHash)->pixelStatusBit(pixel, 1);

  }

  return true;
}

double PixelConditionsSummarySvc::activeFraction(const IdentifierHash & elementHash, const Identifier & idStart,
						 const Identifier & idEnd){

  if(m_useBS && !m_pixelBSErrorsSvc->isActive(elementHash)) return 0.;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }

    if(!isDCSActive) return 0.;

  }

  if(m_useTDAQ){
    if(!m_pixelTDAQSvc->tdaq_module_enabled(elementHash))
      return 0.;
  }

  // SpecialPixelMap is the only source that can provide an 
  // active fraction different from 0 or 1
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    Identifier moduleId = m_pixelID->wafer_id(elementHash);

    int endcap = m_pixelID->barrel_ec(moduleId);
    unsigned int phimod = m_pixelID->phi_module(moduleId);

    int phiStart = static_cast<int>(m_pixelID->phi_index(idStart));
    int etaStart = static_cast<int>(m_pixelID->eta_index(idStart));

    int phiEnd = static_cast<int>(m_pixelID->phi_index(idEnd));
    int etaEnd = static_cast<int>(m_pixelID->eta_index(idEnd));

    double nTotal =
      (static_cast<double>(std::abs(phiStart - phiEnd)) + 1.) *
      (static_cast<double>(std::abs(etaStart - etaEnd)) + 1.);

    double nActive = 0.;
    unsigned int mchips = m_specialPixelMap->module(elementHash)->chipsPerModule();
    unsigned int mtype = m_specialPixelMap->module(elementHash)->chipType();
    mtype +=mchips*10; 

    for(int i = std::min(phiStart, phiEnd); i <= std::max(phiStart, phiEnd); i++){
      for(int j = std::min(etaStart, etaEnd); j <= std::max(etaStart, etaEnd); j++){

	unsigned int pixel = ModuleSpecialPixelMap::encodePixelID( endcap, phimod, j, i, mtype );

	nActive += m_specialPixelMap->module(elementHash)->pixelStatusBit(pixel, 1) ? 0. : 1.;
      }
    }

    return nActive / nTotal;

  }

  return 1.;
}



//======== isGood methods


bool PixelConditionsSummarySvc::isGood(const Identifier & elementId, const InDetConditions::Hierarchy h){

  Identifier moduleID       = m_pixelID->wafer_id(elementId);
  IdentifierHash moduleHash = m_pixelID->wafer_hash(elementId);

  if(m_useBS && !m_pixelBSErrorsSvc->isGood(moduleHash)) return false;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(moduleHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_pixelDCSSvc->getFSMStatus(moduleHash);


    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }
    for(unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++){
      if(m_isActiveStatus[istatus] == dcsStatus) isDCSGood = true;
    }

    if( !(isDCSActive && isDCSGood) ) return false;

  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(moduleHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    unsigned int pixel = 0;
    unsigned int mchips = m_specialPixelMap->module(moduleHash)->chipsPerModule();
    unsigned int mtype = m_specialPixelMap->module(moduleHash)->chipType();
    mtype +=mchips*10; // mchips*10+mtype for encodePixelID

    switch (h) {

    case InDetConditions::DEFAULT:
    case InDetConditions::PIXEL_MODULE:

      moduleHash = m_pixelID->wafer_hash(elementId);

      return !(m_specialPixelMap->module(moduleHash)->moduleStatus() & 3);

    case InDetConditions::PIXEL_CHIP:

      moduleID = m_pixelID->wafer_id(elementId);
      moduleHash = m_pixelID->wafer_hash(moduleID);

      getID(elementId, pixel, mtype);

      return !(m_specialPixelMap->module(moduleHash)->chipStatus(pixel % (mchips>2?mchips:2)) & 3);

    case InDetConditions::PIXEL_PIXEL:

      moduleID = m_pixelID->wafer_id(elementId);
      moduleHash = m_pixelID->wafer_hash(moduleID);

      getID(elementId, pixel, mtype);

      return !(m_specialPixelMap->module(moduleHash)->pixelStatus(pixel) & 3 );

    default:

      return false;
    }

  }

  return true;
}

bool PixelConditionsSummarySvc::isGood(const IdentifierHash & elementHash){

  if(m_useBS && !m_pixelBSErrorsSvc->isGood(elementHash)) return false;

   if(m_useDCS){


    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_pixelDCSSvc->getFSMStatus(elementHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }
    for(unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++){
      if(m_isActiveStatus[istatus] == dcsStatus) isDCSGood = true;
    }

    if( !(isDCSActive && isDCSGood) ) return false;

  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(elementHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    return !(m_specialPixelMap->module(elementHash)->moduleStatus() & 3);

  }

  return true;
}

bool PixelConditionsSummarySvc::isGood(const IdentifierHash & elementHash, const Identifier & elementId){

  if(m_useBS && !m_pixelBSErrorsSvc->isGood(elementHash)) return false;

   if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_pixelDCSSvc->getFSMStatus(elementHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }
    for(unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++){
      if(m_isActiveStatus[istatus] == dcsStatus) isDCSGood = true;
    }

    if( !(isDCSActive && isDCSGood) ) return false;
  }

  if(m_useTDAQ && !(m_pixelTDAQSvc->tdaq_module_enabled(elementHash)) )
    return false;

  // SpecialPixelMap is the more detailed conditions DB 
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    unsigned int pixel = 0;
    unsigned int mchips = m_specialPixelMap->module(elementHash)->chipsPerModule();
    unsigned int mtype = m_specialPixelMap->module(elementHash)->chipType();
    mtype +=mchips*10; // mchips*10+mtype needed for encodePixelID

    getID(elementId, pixel, mtype);

    return !(m_specialPixelMap->module(elementHash)->pixelStatus(pixel) & 3 );

  }

  return true;
}

double PixelConditionsSummarySvc::goodFraction(const IdentifierHash & elementHash, const Identifier & idStart,
					       const Identifier & idEnd){

  if(m_useBS && !m_pixelBSErrorsSvc->isGood(elementHash)) return 0.;

  if(m_useDCS){

    bool isDCSActive = false;
    std::string dcsState = m_pixelDCSSvc->getFSMState(elementHash);
    bool isDCSGood = false;
    std::string dcsStatus = m_pixelDCSSvc->getFSMStatus(elementHash);

    for(unsigned int istate=0; istate<m_isActiveStates.size(); istate++){
      if(m_isActiveStates[istate] == dcsState) isDCSActive = true;
    }
    for(unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++){
      if(m_isActiveStatus[istatus] == dcsStatus) isDCSGood = true;
    }

    if( !(isDCSActive && isDCSGood) ) return 0.;

  }

  if(m_useTDAQ){
    if(!m_pixelTDAQSvc->tdaq_module_enabled(elementHash))
      return 0.;
  }

  // SpecialPixelMap is the only source that can provide an 
  // active fraction different from 0 or 1
  // (granularity at the pixel level) and must be checked last.
  if(m_useSpecialPixelMap){

    Identifier moduleId = m_pixelID->wafer_id(elementHash);

    int endcap = m_pixelID->barrel_ec(moduleId);
    unsigned int phimod = m_pixelID->phi_module(moduleId);

    int phiStart = static_cast<int>(m_pixelID->phi_index(idStart));
    int etaStart = static_cast<int>(m_pixelID->eta_index(idStart));

    int phiEnd = static_cast<int>(m_pixelID->phi_index(idEnd));
    int etaEnd = static_cast<int>(m_pixelID->eta_index(idEnd));

    double nTotal =
      (static_cast<double>(std::abs(phiStart - phiEnd)) + 1.) *
      (static_cast<double>(std::abs(etaStart - etaEnd)) + 1.);

    double nGood = 0.;

    unsigned int mchips = m_specialPixelMap->module(elementHash)->chipsPerModule();
    unsigned int mtype = m_specialPixelMap->module(elementHash)->chipType();
    mtype +=mchips*10; // mchips*10 + mtype for encodePixelID

    for(int i = std::min(phiStart, phiEnd); i <= std::max(phiStart, phiEnd); i++){
      for(int j = std::min(etaStart, etaEnd); j <= std::max(etaStart, etaEnd); j++){

	unsigned int pixel = ModuleSpecialPixelMap::encodePixelID( endcap, phimod, j, i, mtype );

	nGood += (m_specialPixelMap->module(elementHash)->pixelStatus(pixel) & 3 ) ? 0. : 1.;
      }
    }

    return nGood / nTotal;

  }

  return 1.;
}


void PixelConditionsSummarySvc::getID(const Identifier& id, unsigned int& pixID, unsigned int& mchips ) const{

  int endcap = m_pixelID->barrel_ec(id);
  unsigned int phimod = m_pixelID->phi_module(id);
  unsigned int phipix = m_pixelID->phi_index(id);
  unsigned int etapix = m_pixelID->eta_index(id);

  pixID = ModuleSpecialPixelMap::encodePixelID( endcap, phimod, etapix, phipix, mchips );
}
