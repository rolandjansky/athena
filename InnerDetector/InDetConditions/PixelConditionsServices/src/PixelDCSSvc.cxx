/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSSvc.h"

#include "CoralBase/Attribute.h" 
#include "CoralBase/AttributeListSpecification.h" 

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h" 

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "AthenaKernel/IIOVDbSvc.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

PixelDCSSvc::PixelDCSSvc(const std::string& name, ISvcLocator* sl):
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_pixelDCSData(0),
  m_pixid(nullptr),
  m_pixman(nullptr),
  m_par_temperatureKey(""),
  m_par_HVKey(""),
  m_par_FSMStatusKey(""),
  m_par_FSMStateKey(""),
  m_par_pixelDCSDataSGKey("/InDetConditions/PixelDCSData"),
  m_par_temperatureField("temperature"),
  m_par_HVField("HV"),
  m_par_FSMStatusField("FSM_status"),
  m_par_FSMStateField("FSM_state"),
  m_par_useTemperature(true),
  m_par_useHV(true),
  m_par_useFSMStatus(true),
  m_par_useFSMState(true),
  m_par_registerCallback(true)
{

  declareProperty("TemperatureFolder", m_par_temperatureKey); 
  declareProperty("HVFolder", m_par_HVKey); 
  declareProperty("FSMStatusFolder", m_par_FSMStatusKey);
  declareProperty("FSMStateFolder", m_par_FSMStateKey); 
  declareProperty("RegisterCallback", m_par_registerCallback); 
  
  declareProperty("TemperatureFieldName", m_par_temperatureField); 
  declareProperty("HVFieldName", m_par_HVField); 
  declareProperty("FSMStatusFieldName", m_par_FSMStatusField);
  declareProperty("FSMStateFieldName", m_par_FSMStateField); 

  declareProperty("UseTemperature", m_par_useTemperature); 
  declareProperty("UseHV", m_par_useHV); 
  declareProperty("UseFSMStatus", m_par_useFSMStatus); 
  declareProperty("UseFSMState", m_par_useFSMState); 


}

PixelDCSSvc::~PixelDCSSvc(){}


inline StatusCode PixelDCSSvc::queryInterface(const InterfaceID& riid, void** ppvIf){

  if (riid == interfaceID()){
    *ppvIf = dynamic_cast<PixelDCSSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  if (riid == IPixelDCSSvc::interfaceID()){
    *ppvIf = dynamic_cast<IPixelDCSSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );

}


StatusCode PixelDCSSvc::initialize()
{


   msg(MSG::INFO) << " Entering PixelDCSSvc::initialize()" << endmsg;

  StatusCode sc;

  sc = m_detStore.retrieve();
  if (!sc.isSuccess() || 0 == m_detStore)  {
    msg(MSG::FATAL)<< "Unable to retrieve detector store" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Detector store retrieved" << endmsg;


  // Get the geometry 
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 
  if(StatusCode::SUCCESS != m_detStore->retrieve(m_pixman, "Pixel") || m_pixman==0){
     msg(MSG::FATAL)<< "Could not find Pixel manager "<<endmsg; 
    return StatusCode::FAILURE; 
  }

  itermin = m_pixman->getDetectorElementBegin(); 
  itermax = m_pixman->getDetectorElementEnd(); 

  sc = m_detStore->retrieve( m_pixid, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }


  if (this->createDCSData().isFailure()) {
    msg(MSG::FATAL) << "Could not create DCS data" << endmsg;
    return StatusCode::FAILURE;
  }

  //Register a callback
  if(m_par_registerCallback){

    if(m_par_useTemperature){

      const DataHandle<CondAttrListCollection> attrListCollT;
      if( (m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack, dynamic_cast<IPixelDCSSvc*>(this),
			      attrListCollT, m_par_temperatureKey, true)).isFailure()){ 
	msg(MSG::FATAL) << "Unable to register callback for folder " << m_par_temperatureKey << endmsg; 
	return StatusCode::FAILURE; 
      }

    }
    
    if(m_par_useHV){

      const DataHandle<CondAttrListCollection> attrListCollHV;
      if( (m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack, dynamic_cast<IPixelDCSSvc*>(this),
			      attrListCollHV, m_par_HVKey, true)).isFailure()){ 
	msg(MSG::FATAL) << "Unable to register callback for folder " << m_par_HVKey << endmsg; 
	return StatusCode::FAILURE; 
      }

    }

    if(m_par_useFSMStatus){

      const DataHandle<CondAttrListCollection> attrListCollFSMS;
      if( (m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack, dynamic_cast<IPixelDCSSvc*>(this),
			      attrListCollFSMS, m_par_FSMStatusKey, true)).isFailure()){ 
	msg(MSG::FATAL)  << "Unable to register callback for folder " << m_par_FSMStatusKey << endmsg; 
	return StatusCode::FAILURE; 
      }

    }

    if(m_par_useFSMState){

      const DataHandle<CondAttrListCollection> attrListCollFSMSt;
      if( (m_detStore->regFcn(&IPixelDCSSvc::IOVCallBack, dynamic_cast<IPixelDCSSvc*>(this),
			      attrListCollFSMSt, m_par_FSMStateKey, true)).isFailure()){ 
	msg(MSG::FATAL) << "Unable to register callback for folder " << m_par_FSMStateKey << endmsg; 
	return StatusCode::FAILURE; 
      }
 
    }

  }

  return StatusCode::SUCCESS; 
}

StatusCode PixelDCSSvc::finalize(){
  msg(MSG::INFO) << "Entering PixelDCSSvc::finalize()" << endmsg; 

  if(m_pixelDCSData) delete m_pixelDCSData;

  return StatusCode::SUCCESS; 
} 



StatusCode PixelDCSSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys))
{

  static ServiceHandle<IIOVDbSvc> lIOVDbSvc("IOVDbSvc",this->name());
  StatusCode sc = StatusCode::SUCCESS; 
  std::list<std::string>::const_iterator si; 

  for(si=keys.begin(); si!=keys.end(); ++si){

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IOVCALLBACK for key " << *si << " number " << I << endmsg; 

    if(*si == m_par_temperatureKey ){ 

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Loading DCS temperature from DB "  << endmsg; 

      const CondAttrListCollection* atrc_temp; 
      sc = m_detStore->retrieve(atrc_temp, m_par_temperatureKey); 

      if(sc.isFailure()) { 
	msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_temperatureKey<<endmsg; 
	return sc; 
      }
      
      CondAttrListCollection::const_iterator itrx_temp; 
      for(itrx_temp = atrc_temp->begin(); itrx_temp !=atrc_temp->end(); ++itrx_temp){ 
	CondAttrListCollection::ChanNum channum_temp = itrx_temp->first;
	const coral::AttributeList& atr_temp = itrx_temp->second; 
        float temperaturedata;
	
        //if(atr_temp["temperaturem_par_temperatureField.c_str()"].isNull()){
        if(atr_temp[m_par_temperatureField.c_str()].isNull()){
	 temperaturedata = -999.;
         msg(MSG::DEBUG) << " Id_hash = " << channum_temp << " ** Temperature =  "  <<  temperaturedata << endmsg;
	}else{
	 temperaturedata = atr_temp[m_par_temperatureField.c_str()].data<float>();
         msg(MSG::DEBUG) << " Id_hash = " << channum_temp << " ** Temperature =  "  <<  temperaturedata << endmsg;
	}
	
	(*m_pixelDCSData)[channum_temp]->setTemperature(temperaturedata);
      }

    }

    if( *si == m_par_HVKey ){ 

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Loading DCS  HV from DB "  << endmsg; 

      const CondAttrListCollection* atrc_hv; 
      sc = m_detStore->retrieve(atrc_hv, m_par_HVKey); 

      if(sc.isFailure()) { 
	msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_HVKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_hv; 
      for(itrx_hv = atrc_hv->begin(); itrx_hv !=atrc_hv->end(); ++itrx_hv){ 
	CondAttrListCollection::ChanNum channum_hv = itrx_hv->first;
	const coral::AttributeList& atr_hv = itrx_hv->second; 
	float hvdata;
	
        //if(atr_hv["hv"m_par_HVField.c_str()].isNull()){
        if(atr_hv[m_par_HVField.c_str()].isNull()){
	 hvdata = -999.;
         msg(MSG::DEBUG) << " Id_hash = " << channum_hv << " ** HV =  "  << hvdata << endmsg;
	}else{
  	 hvdata = atr_hv[m_par_HVField.c_str()].data<float>();
         msg(MSG::DEBUG) << " Id_hash = " << channum_hv << " ** HV =  "  << hvdata << endmsg;
	}

	(*m_pixelDCSData)[channum_hv]->setHV(hvdata);

      }

    }

    if( *si ==m_par_FSMStatusKey ){ 

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Loading DCS  FSM status from DB "  << endmsg; 

      const CondAttrListCollection* atrc_fsmstatus; 
      sc = m_detStore->retrieve(atrc_fsmstatus, m_par_FSMStatusKey); 
      
      if(sc.isFailure()) { 
	msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_FSMStatusKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_fsmstatus; 
      for(itrx_fsmstatus = atrc_fsmstatus->begin(); itrx_fsmstatus !=atrc_fsmstatus->end(); ++itrx_fsmstatus){ 
	CondAttrListCollection::ChanNum channum_fsmstatus = itrx_fsmstatus->first;
	const coral::AttributeList& atr_fsmstatus = itrx_fsmstatus->second; 
	std::string fsmstatusdata;

        if(atr_fsmstatus[m_par_FSMStatusField.c_str()].isNull()){
	 fsmstatusdata = "ERROR";
         msg(MSG::DEBUG) << " Id_hash = " << channum_fsmstatus << " ** FSMSTATUS =  "  << fsmstatusdata << endmsg;
	}else{
  	 fsmstatusdata = atr_fsmstatus[m_par_FSMStatusField.c_str()].data<std::string>();
         msg(MSG::DEBUG) << " Id_hash = " << channum_fsmstatus << " ** FSMSTATUS =  "  << fsmstatusdata << endmsg;
	}

	(*m_pixelDCSData)[channum_fsmstatus]->setFSMStatus(fsmstatusdata);

      }

    }

    if(*si ==m_par_FSMStateKey){ 

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Loading DCS  FSM state from DB "  << endmsg; 

      const CondAttrListCollection* atrc_fsmstate; 
      sc = m_detStore->retrieve(atrc_fsmstate, m_par_FSMStateKey); 
      
      if(sc.isFailure()) { 
	msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_FSMStateKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_fsmstate; 
      for(itrx_fsmstate = atrc_fsmstate->begin(); itrx_fsmstate !=atrc_fsmstate->end(); ++itrx_fsmstate){ 
	CondAttrListCollection::ChanNum channum_fsmstate = itrx_fsmstate->first;
	const coral::AttributeList& atr_fsmstate = itrx_fsmstate->second; 
	std::string fsmstatedata = atr_fsmstate[m_par_FSMStateField.c_str()].data<std::string>();

        if(atr_fsmstate[m_par_FSMStateField.c_str()].isNull()){
	 fsmstatedata = "UNDEFINED";
         msg(MSG::DEBUG) << " Id_hash = " << channum_fsmstate << " ** FSMSTATE =  "  << fsmstatedata << endmsg;
	}else{
  	 fsmstatedata = atr_fsmstate[m_par_FSMStateField.c_str()].data<std::string>();
         msg(MSG::DEBUG) << " Id_hash = " << channum_fsmstate << " ** FSMSTATE =  "  << fsmstatedata << endmsg;
	}

	(*m_pixelDCSData)[channum_fsmstate]->setFSMState(fsmstatedata);

      }

    }
    // lIOVDbSvc->dropObject(*si,false); // commented, bug 55586

  } /// keys

  return sc; 

}

StatusCode PixelDCSSvc::createDCSData()
{

  // Get the geometry 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in createDCSData" << endmsg;

  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 
  itermin = m_pixman->getDetectorElementBegin(); 
  itermax = m_pixman->getDetectorElementEnd(); 


  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "get iterator to detector element ok" << endmsg;

  m_pixelDCSData = new PixelDCSData();
  m_pixelDCSData->resize(m_pixid->wafer_hash_max());



  for( iter=itermin; iter !=itermax; ++iter){ 
    const InDetDD::SiDetectorElement* element = *iter; 
    if(element !=0){ 
      Identifier ident = element->identify(); 
      if(m_pixid->is_pixel(ident)){ 
	IdentifierHash id_hash = m_pixid->wafer_hash(ident); 
	PixelModuleDCSData* mdcsd = new PixelModuleDCSData(ident);
	(*m_pixelDCSData)[id_hash] = mdcsd;
      }
    }
  }

  return StatusCode::SUCCESS; 

} 





StatusCode PixelDCSSvc::printData() const
{


  msg(MSG::INFO)  << " in PixelDCSSvc::printData " << endmsg;

  msg(MSG::ALWAYS) << ":::::: Printing  CondAttrListCollection objects ::::::" << endmsg;

  StatusCode sc;

  if(m_par_useTemperature){
    const CondAttrListCollection* atrc_temp; 
    sc = m_detStore->retrieve(atrc_temp, m_par_temperatureKey); 

    if(sc.isFailure()) { 
      msg(MSG::ERROR)  <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_temperatureKey<<endmsg; 
      return sc; 
    }
    msg(MSG::ALWAYS) << "Temperature key at " << std::hex << (void*)atrc_temp << std::dec << endmsg;

    CondAttrListCollection::const_iterator itrx_temp; 
    for(itrx_temp = atrc_temp->begin(); itrx_temp !=atrc_temp->end(); ++itrx_temp){ 
      CondAttrListCollection::ChanNum channum_temp = itrx_temp->first;
      const coral::AttributeList& atr_temp = itrx_temp->second; 
      std::ostringstream attrStr_temp;
      atr_temp.toOutputStream(attrStr_temp);
      msg(MSG::ALWAYS) << "ChanNum " << channum_temp << " Attribute list " << attrStr_temp.str() << endmsg;

    }

  }

  if(m_par_useHV){

    const CondAttrListCollection* atrc_hv; 
    sc = m_detStore->retrieve(atrc_hv, m_par_HVKey); 

    if(sc.isFailure()) { 
      msg(MSG::ERROR)  <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_HVKey<<endmsg; 
      return sc; 
    }
    msg(MSG::ALWAYS) << "HV key at " << std::hex << (void*)atrc_hv << std::dec << endmsg;

    CondAttrListCollection::const_iterator itrx_hv; 
    for(itrx_hv = atrc_hv->begin(); itrx_hv !=atrc_hv->end(); ++itrx_hv){ 
      CondAttrListCollection::ChanNum channum_hv = itrx_hv->first;
      const coral::AttributeList& atr_hv = itrx_hv->second; 
      std::ostringstream attrStr_hv;
      atr_hv.toOutputStream(attrStr_hv);
      msg(MSG::ALWAYS) << "ChanNum " << channum_hv << " Attribute list " << attrStr_hv.str() << endmsg;

    }

  }

  if(m_par_useFSMStatus){

    const CondAttrListCollection* atrc_fsmstatus; 
    sc = m_detStore->retrieve(atrc_fsmstatus, m_par_FSMStatusKey); 

    if(sc.isFailure()) { 
      msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_FSMStatusKey<<endmsg; 
      return sc; 
    }
    msg(MSG::ALWAYS) << "FSM status key at " << std::hex << (void*)atrc_fsmstatus << std::dec << endmsg;

    CondAttrListCollection::const_iterator itrx_fsmstatus; 
    for(itrx_fsmstatus = atrc_fsmstatus->begin(); itrx_fsmstatus !=atrc_fsmstatus->end(); ++itrx_fsmstatus){ 
      CondAttrListCollection::ChanNum channum_fsmstatus = itrx_fsmstatus->first;
      const coral::AttributeList& atr_fsmstatus = itrx_fsmstatus->second; 
      std::ostringstream attrStr_fsmstatus;
      atr_fsmstatus.toOutputStream(attrStr_fsmstatus);
      msg(MSG::ALWAYS) << "ChanNum " << channum_fsmstatus << " Attribute list " << attrStr_fsmstatus.str() << endmsg;

    }

  }

  if(m_par_useFSMState){


    const CondAttrListCollection* atrc_fsmstate; 
    sc = m_detStore->retrieve(atrc_fsmstate, m_par_FSMStateKey); 

    if(sc.isFailure()) { 
      msg(MSG::ERROR) <<"could not retrieve CondAttrListCollection from DB folder "<<m_par_FSMStateKey<<endmsg; 
      return sc; 
    }
    msg(MSG::ALWAYS) << "FSM state key at " << std::hex << (void*)atrc_fsmstate << std::dec << endmsg;

    CondAttrListCollection::const_iterator itrx_fsmstate; 
    for(itrx_fsmstate = atrc_fsmstate->begin(); itrx_fsmstate !=atrc_fsmstate->end(); ++itrx_fsmstate){ 
      CondAttrListCollection::ChanNum channum_fsmstate = itrx_fsmstate->first;
      const coral::AttributeList& atr_fsmstate = itrx_fsmstate->second; 
      std::ostringstream attrStr_fsmstate;
      atr_fsmstate.toOutputStream(attrStr_fsmstate);
      msg(MSG::ALWAYS) << "ChanNum " << channum_fsmstate << " Attribute list " << attrStr_fsmstate.str() << endmsg;

    }


  }

  msg(MSG::ALWAYS) << ":::::: Printing  PixelDCSData Objects ::::::" << endmsg;

  for(unsigned int dcsditr=0; dcsditr < m_pixid->wafer_hash_max();  dcsditr++){

    msg(MSG::ALWAYS) << *(*m_pixelDCSData)[dcsditr] << endmsg;

  }

  return StatusCode::SUCCESS;
    
}
