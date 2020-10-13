/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//database access headers
#include "PixelDCSTool.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "CoralBase/Attribute.h" 
#include "CoralBase/AttributeListSpecification.h" 


//Athena headers
#include "SGTools/TransientAddress.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h" 


#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"


//C++ standard library headers
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

PixelDCSTool::PixelDCSTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  // m_toolsvc(0),
  m_toolsvc("ToolSvc",name),
  // m_IOVSvc(0),
  m_IOVSvc("IOVSvc",name),
  // m_IOVRegistrationSvc(0),
  m_IOVRegistrationSvc("IOVRegistrationSvc",name),
  //m_streamer(0),
  // m_streamer("AthenaOutputStreamTool/CondStreamPixelDCSTest"),
  m_streamer("AthenaOutputStreamTool/CondStreamPixelDCSTest"),
  m_pixid(0),
  m_pixelDCSData(0),
  m_temperatureKey(""),
  m_HVKey(""),
  m_FSMStatusKey(""),
  m_FSMStateKey(""),
  m_pixelDCSDataSGKey("/InDetConditions/PixelDCSData"),
  m_registerCallback(false),
  m_temperature(false),
  m_HV(false),
  m_FSMStatus(false),
  m_FSMState(false),
  m_writeDefault(false),
  m_temperatureFile(""),
  m_HVFile(""),
  m_FSMStatusFile(""),
  m_FSMStateFile(""),
  m_par_temperatureField("temperature"),
  m_par_HVField("HV"),
  m_par_FSMStatusField("FSM_status"),
  m_par_FSMStateField("FSM_state"),
  m_temperatureTag(""),
  m_HVTag(""),
  m_FSMStatusTag(""),
  m_FSMStateTag(""),
  m_temperatureValue(-9999.),
  m_HVValue(-9999.),
  m_FSMStatusValue("NO_DATA"),
  m_FSMStateValue("NO_DATA"),
  m_maxAttempt(1)
{

  declareInterface< IPixelDCSTool >(this); 

  declareProperty("TemperatureFolder", m_temperatureKey); 
  declareProperty("HVFolder", m_HVKey); 
  declareProperty("FSMStatusFolder", m_FSMStatusKey); 
  declareProperty("FSMStateFolder", m_FSMStateKey);
  declareProperty("RegisterCallback", m_registerCallback); 
  declareProperty("WriteDefault", m_writeDefault); 
  declareProperty("Temperature", m_temperature); 
  declareProperty("HV", m_HV); 
  declareProperty("FSMStatus", m_FSMStatus); 
  declareProperty("FSMState", m_FSMState);
  declareProperty("TemperatureFile", m_temperatureFile); 
  declareProperty("HVFile", m_HVFile); 
  declareProperty("FSMStatusFile", m_FSMStatusFile); 
  declareProperty("FSMStateFile", m_FSMStateFile); 
  declareProperty("TemperatureFieldName", m_par_temperatureField); 
  declareProperty("HVFieldName", m_par_HVField); 
  declareProperty("FSMStatusFieldName", m_par_FSMStatusField);
  declareProperty("FSMStateFieldName", m_par_FSMStateField); 
  declareProperty("TemperatureTag", m_temperatureTag); 
  declareProperty("HVTag", m_HVTag); 
  declareProperty("FSMStatusTag", m_FSMStatusTag); 
  declareProperty("FSMStateTag", m_FSMStateTag); 
  declareProperty("TemperatureValue",m_temperatureValue);
  declareProperty("HVValue",m_HVValue);
  declareProperty("FSMStatusValue",m_FSMStatusValue);
  declareProperty("FSMStateValue",m_FSMStateValue);
  declareProperty("MaxAttempt",m_maxAttempt);

}

PixelDCSTool::~PixelDCSTool(){}

StatusCode PixelDCSTool::initialize()
{
  //m_log.setLevel(outputLevel());
  if (msgLvl(MSG::INFO))msg(MSG::INFO) << " Entering PixelDCSTool::initialize()" << endmsg;

   StatusCode sc = StatusCode::SUCCESS; 

  //Retrieve Tool Service
  // sc = service("ToolSvc", m_toolsvc);
  sc = m_toolsvc.retrieve();
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL))msg(MSG::FATAL)<< "Unable to retrieve ToolSvc"<< endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "ToolSvc retrieved" << endmsg;



  //Retrieve IOV Service
  //sc = service("IOVSvc", m_IOVSvc); 
  sc = m_IOVSvc.retrieve();
  if(sc.isFailure()){ 
    if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to retrieve IOVSvc" << endmsg; 
    return StatusCode::FAILURE; 
  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "IOVSvc retrieved" << endmsg;



  //Retrieve IOV registration Service
  //sc = service("IOVRegistrationSvc", m_IOVRegistrationSvc); 
  sc = m_IOVRegistrationSvc.retrieve();
  if(sc.isFailure()){ 
    if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to retrieve IOVRegistrationSvc" << endmsg; 
    return StatusCode::FAILURE; 
  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "IOVRegistrationSvc retrieved" << endmsg;


  // Retrieve Athena Output Stream Tool
  //sc = m_toolsvc->retrieveTool("AthenaOutputStreamTool","CondStreamPixelDCSTest", m_streamer);
  sc = m_streamer.retrieve();
  if(sc.isFailure()){ 
    if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to retrieve AthenaOutputStreamTool" << endmsg;
    return StatusCode::FAILURE; 
  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "AthenaOutputStreamTool retrieved" << endmsg;





  // Get the geometry 
  if (detStore()->retrieve(m_pixid, "PixelID").isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  for (PixelID::const_id_iterator wafer_it=m_pixid->wafer_begin(); wafer_it!=m_pixid->wafer_end(); ++wafer_it) {
    Identifier ident = *wafer_it;
    if(m_pixid->is_pixel(ident)){
      IdentifierHash id_hash = m_pixid->wafer_hash(ident);
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "identifier   " <<ident.get_compact() << "   hash     " << (unsigned int)id_hash << endmsg;
    }
  }



  //Register callback fonctions
  if(m_registerCallback){

    if(m_temperature){
      const DataHandle<CondAttrListCollection> attrListCollT;
      if( (detStore()->regFcn(&IPixelDCSTool::IOVCallBack, dynamic_cast<IPixelDCSTool*>(this),
			      attrListCollT, m_temperatureKey)).isFailure()){ 
	if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to register callback for folder " << m_temperatureKey << endmsg; 
	return StatusCode::FAILURE; 
      }
    }
    
    if(m_HV){
      const DataHandle<CondAttrListCollection> attrListCollHV;
      if( (detStore()->regFcn(&IPixelDCSTool::IOVCallBack, dynamic_cast<IPixelDCSTool*>(this),
			      attrListCollHV, m_HVKey)).isFailure()){ 
	if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to register callback for folder " << m_HVKey << endmsg; 
	return StatusCode::FAILURE; 
      }
    }

    if(m_FSMStatus){
      const DataHandle<CondAttrListCollection> attrListCollFSMS;
      if( (detStore()->regFcn(&IPixelDCSTool::IOVCallBack, dynamic_cast<IPixelDCSTool*>(this),
			      attrListCollFSMS, m_FSMStatusKey)).isFailure()){ 
	if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to register callback for folder " << m_FSMStatusKey << endmsg; 
	return StatusCode::FAILURE; 
      }
    }

    if(m_FSMState){
      const DataHandle<CondAttrListCollection> attrListCollFSMSt;
      if( (detStore()->regFcn(&IPixelDCSTool::IOVCallBack, dynamic_cast<IPixelDCSTool*>(this),
			      attrListCollFSMSt, m_FSMStateKey)).isFailure()){ 
	if (msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Unable to register callback for folder " << m_FSMStateKey << endmsg; 
	return StatusCode::FAILURE; 
      }
    }
    
  }

  return StatusCode::SUCCESS; 
}

StatusCode PixelDCSTool::finalize(){
  if (msgLvl(MSG::INFO))msg(MSG::INFO) << "Entering PixelDCSTool::finalize()" << endmsg; 
  return StatusCode::SUCCESS; 
} 



StatusCode PixelDCSTool::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys))
{

 
  StatusCode sc = StatusCode::SUCCESS; 
  std::list<std::string>::const_iterator si; 


  bool data_change = false;
  for(si=keys.begin(); si!=keys.end(); ++si){
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "IOVCALLBACK for key " << *si << " number " << I << endmsg; 
    if(*si == m_temperatureKey || *si == m_HVKey || *si ==m_FSMStatusKey || *si ==m_FSMStateKey){ 
      data_change = true;
    }
  }

  if(data_change){
   
    if (this->createDCSData().isFailure()) {
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not create DCS data" << endmsg;
      return(StatusCode::FAILURE);
    }

    if(m_temperature){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Loading DCS temperature from DB "  << endmsg; 

      const CondAttrListCollection* atrc_temp; 
      sc = detStore()->retrieve(atrc_temp, m_temperatureKey); 

      if(sc.isFailure()) { 
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR)<<"could not retrieve CondAttrListCollection from DB folder "<<m_temperatureKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_temp; 
      for(itrx_temp = atrc_temp->begin(); itrx_temp !=atrc_temp->end(); ++itrx_temp){ 
	CondAttrListCollection::ChanNum channum_temp = itrx_temp->first;
	const coral::AttributeList& atr_temp = itrx_temp->second; 
	float temperaturedata = atr_temp[m_par_temperatureField.c_str()].data<float>();
	
	(*m_pixelDCSData)[channum_temp]->setTemperature(temperaturedata);
      }
    }

    if(m_HV){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Loading DCS  HV from DB "  << endmsg; 

      const CondAttrListCollection* atrc_hv; 
      sc = detStore()->retrieve(atrc_hv, m_HVKey); 

      if(sc.isFailure()) { 
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR)<<"could not retrieve CondAttrListCollection from DB folder "<<m_HVKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_hv; 
      for(itrx_hv = atrc_hv->begin(); itrx_hv !=atrc_hv->end(); ++itrx_hv){ 
	CondAttrListCollection::ChanNum channum_hv = itrx_hv->first;
	const coral::AttributeList& atr_hv = itrx_hv->second; 
	float hvdata = atr_hv[m_par_HVField.c_str()].data<float>();
	
	(*m_pixelDCSData)[channum_hv]->setHV(hvdata);

      }
    }


    if(m_FSMStatus){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Loading DCS  FSM status from DB "  << endmsg; 

      const CondAttrListCollection* atrc_fsmstatus; 
      sc = detStore()->retrieve(atrc_fsmstatus, m_FSMStatusKey); 
      
      if(sc.isFailure()) { 
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR)<<"could not retrieve CondAttrListCollection from DB folder "<<m_FSMStatusKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_fsmstatus; 
      for(itrx_fsmstatus = atrc_fsmstatus->begin(); itrx_fsmstatus !=atrc_fsmstatus->end(); ++itrx_fsmstatus){ 
	CondAttrListCollection::ChanNum channum_fsmstatus = itrx_fsmstatus->first;
	const coral::AttributeList& atr_fsmstatus = itrx_fsmstatus->second; 
	std::string fsmstatusdata = atr_fsmstatus[m_par_FSMStatusField.c_str()].data<std::string>();

	(*m_pixelDCSData)[channum_fsmstatus]->setFSMStatus(fsmstatusdata);

      }
    }

    if(m_FSMState){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Loading DCS  FSM state from DB "  << endmsg; 

      const CondAttrListCollection* atrc_fsmstate; 
      sc = detStore()->retrieve(atrc_fsmstate, m_FSMStateKey); 
      
      if(sc.isFailure()) { 
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR)<<"could not retrieve CondAttrListCollection from DB folder "<<m_FSMStateKey<<endmsg; 
	return sc; 
      }

      CondAttrListCollection::const_iterator itrx_fsmstate; 
      for(itrx_fsmstate = atrc_fsmstate->begin(); itrx_fsmstate !=atrc_fsmstate->end(); ++itrx_fsmstate){ 
	CondAttrListCollection::ChanNum channum_fsmstate = itrx_fsmstate->first;
	const coral::AttributeList& atr_fsmstate = itrx_fsmstate->second; 
	std::string fsmstatedata = atr_fsmstate[m_par_FSMStateField.c_str()].data<std::string>();

	(*m_pixelDCSData)[channum_fsmstate]->setFSMState(fsmstatedata);

      }
    }

  }

  sc = this->recordDCSData();
  if(!sc.isSuccess()){
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register PixelDCSData to detector store" << endmsg;
    return StatusCode::FAILURE;
  }

  return sc; 


}

StatusCode PixelDCSTool::createDCSData()
{


  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "in PixelDCSTool::createDCSData()" << endmsg;

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "get iterator to detector element ok" << endmsg;

  m_pixelDCSData = new PixelDCSData();
  m_pixelDCSData->resize(m_pixid->wafer_hash_max());



  // Check all detector elements in the present geometry setup 
  for (PixelID::const_id_iterator wafer_it=m_pixid->wafer_begin(); wafer_it!=m_pixid->wafer_end(); ++wafer_it) {
    Identifier ident = *wafer_it;
    if(m_pixid->is_pixel(ident)){
      IdentifierHash id_hash = m_pixid->wafer_hash(ident); 
      PixelModuleDCSData* mdcsd = new PixelModuleDCSData(ident);
      (*m_pixelDCSData)[id_hash] = mdcsd;
    }
  }

  return StatusCode::SUCCESS; 

} 

StatusCode PixelDCSTool::writeDataToDB()
{

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "writeDataToDB()" << endmsg;

  StatusCode sc = StatusCode::SUCCESS; 

  if(m_writeDefault){

    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Start writing default values to db " << endmsg;


    CondAttrListCollection* attrListColl_temp = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_temp = new coral::AttributeListSpecification(); 
    attrSpec_temp->extend(m_par_temperatureField.c_str(), "float");
    coral::AttributeList attrList_temp(*attrSpec_temp);

    CondAttrListCollection* attrListColl_hv = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_hv = new coral::AttributeListSpecification(); 
    attrSpec_hv->extend(m_par_HVField.c_str(), "float");
    coral::AttributeList attrList_hv(*attrSpec_hv);

    CondAttrListCollection* attrListColl_fsmstatus = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_fsmstatus = new coral::AttributeListSpecification(); 
    attrSpec_fsmstatus->extend(m_par_FSMStatusField.c_str(), "string");
    coral::AttributeList attrList_fsmstatus(*attrSpec_fsmstatus);


    CondAttrListCollection* attrListColl_fsmstate = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_fsmstate = new coral::AttributeListSpecification(); 
    attrSpec_fsmstate->extend(m_par_FSMStateField.c_str(), "string");
    coral::AttributeList attrList_fsmstate(*attrSpec_fsmstate);


    IOVTime start(0);
    IOVTime stop(IOVTime::MAXTIMESTAMP);
    IOVRange iov(start, stop);

    for (PixelID::const_id_iterator wafer_it=m_pixid->wafer_begin(); wafer_it!=m_pixid->wafer_end(); ++wafer_it) {
      Identifier ident = *wafer_it;
      if(m_pixid->is_pixel(ident)){

        IdentifierHash id_hash = m_pixid->wafer_hash(ident); 
        CondAttrListCollection::ChanNum channum = (unsigned int)id_hash;

        attrList_temp[m_par_temperatureField.c_str()].setValue(m_temperatureValue);
        attrListColl_temp->add(channum, attrList_temp);
        attrListColl_temp->add(channum, iov);

        attrList_hv[m_par_HVField.c_str()].setValue(m_HVValue);
        attrListColl_hv->add(channum, attrList_hv);
        attrListColl_hv->add(channum, iov);

        attrList_fsmstatus[m_par_FSMStatusField.c_str()].setValue(m_FSMStatusValue);
        attrListColl_fsmstatus->add(channum, attrList_fsmstatus);
        attrListColl_fsmstatus->add(channum, iov);

        attrList_fsmstate[m_par_FSMStateField.c_str()].setValue(m_FSMStateValue);
        attrListColl_fsmstate->add(channum, attrList_fsmstate);
        attrListColl_fsmstate->add(channum, iov);


      }
    }

    sc = this->commitDataToDetectorStore(attrListColl_temp, m_temperatureKey);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_temperatureKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_temperatureKey << endmsg;
 
    sc = this->registerIOVData(m_temperatureKey, m_temperatureTag);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_temperatureKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_temperatureKey << endmsg;
 


    sc = this->commitDataToDetectorStore(attrListColl_hv, m_HVKey);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_HVKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_HVKey << endmsg;
 
    sc = this->registerIOVData(m_HVKey, m_HVTag);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_HVKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_HVKey << endmsg;



    sc = this->commitDataToDetectorStore(attrListColl_fsmstatus, m_FSMStatusKey);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStatusKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStatusKey << endmsg;

    sc = this->registerIOVData(m_FSMStatusKey, m_FSMStatusTag);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStatusKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStatusKey << endmsg;


    sc = this->commitDataToDetectorStore(attrListColl_fsmstate, m_FSMStateKey);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStateKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStateKey << endmsg;

    sc = this->registerIOVData(m_FSMStateKey, m_FSMStateTag);
    if(sc.isFailure()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStateKey << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStateKey << endmsg;


  }


  std::vector<unsigned int> moduleToStore;

  if(m_temperature){

    CondAttrListCollection* attrListColl_temp = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_temp = new coral::AttributeListSpecification(); 
    attrSpec_temp->extend(m_par_temperatureField.c_str(), "float");
    coral::AttributeList attrList_temp(*attrSpec_temp);
    //AthenaAttributeList attrList_temp(*attrSpec_temp);

    double startTemperature = 0;
    float temperature =0;
    unsigned int  module_id =0;

    std::ifstream temperatureData(m_temperatureFile.c_str());

    while(!temperatureData.eof()){

      temperatureData >> module_id >> startTemperature >> temperature;

      if(!temperatureData.fail()){
	unsigned int module_hash = (unsigned int)m_pixid->wafer_hash(Identifier(module_id));
      
	for(unsigned int i=0; i<moduleToStore.size(); i++){
	  if(moduleToStore[i] == module_hash){

	    sc = this->commitDataToDetectorStore(attrListColl_temp, m_temperatureKey);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_temperatureKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_temperatureKey << endmsg;
 
	    sc = this->registerIOVData(m_temperatureKey, m_temperatureTag);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_temperatureKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_temperatureKey << endmsg;
 

	    moduleToStore.clear();
	    attrListColl_temp = new CondAttrListCollection(true);
	    break;
	  }
	}

	moduleToStore.push_back(module_hash);
	CondAttrListCollection::ChanNum channum_temp = module_hash;
	attrList_temp[m_par_temperatureField.c_str()].setValue(temperature);
	attrListColl_temp->add(channum_temp, attrList_temp);
	IOVTime start_temp((uint64_t)(startTemperature*1000000000));
	IOVTime stop_temp(IOVTime::MAXTIMESTAMP);
	IOVRange iov_temp(start_temp, stop_temp);
	attrListColl_temp->add(channum_temp, iov_temp);

      }

    }

    if(moduleToStore.size()){

      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "writing last values for  " << m_temperatureKey << endmsg;
 
      sc = this->commitDataToDetectorStore(attrListColl_temp, m_temperatureKey);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_temperatureKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_temperatureKey << endmsg;
 
      sc = this->registerIOVData(m_temperatureKey, m_temperatureTag);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_temperatureKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_temperatureKey << endmsg;
 

      moduleToStore.clear();
    }

  }
    
  if(m_HV){
  
    CondAttrListCollection* attrListColl_hv = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_hv = new coral::AttributeListSpecification(); 
    attrSpec_hv->extend(m_par_HVField.c_str(), "float");
    coral::AttributeList attrList_hv(*attrSpec_hv);

    float hv = 0;
    double starthv =0;
    unsigned int  module_id =0;

    std::ifstream HVData(m_HVFile.c_str());

    while(!HVData.eof()){

      HVData >> module_id >> starthv >> hv;
 
      if(!HVData.fail()){

	unsigned int module_hash =  (unsigned int)m_pixid->wafer_hash(Identifier(module_id));

	for(unsigned int i=0; i<moduleToStore.size(); i++){
	  if(moduleToStore[i] == module_hash){

	    sc = this->commitDataToDetectorStore(attrListColl_hv, m_HVKey);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_HVKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_HVKey << endmsg;
 
	    sc = this->registerIOVData(m_HVKey, m_HVTag);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_HVKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_HVKey << endmsg;


	    moduleToStore.clear();
	    attrListColl_hv = new CondAttrListCollection(true);
	    break;
	  }
	}
	moduleToStore.push_back(module_hash);

	CondAttrListCollection::ChanNum channum_hv = module_hash;
	attrList_hv[m_par_HVField.c_str()].setValue(hv);
	attrListColl_hv->add(channum_hv, attrList_hv);
	IOVTime start_hv((uint64_t)(starthv*1000000000));
	IOVTime stop_hv(IOVTime::MAXTIMESTAMP);
	IOVRange iov_hv(start_hv, stop_hv);
	attrListColl_hv->add(channum_hv, iov_hv);
      }
    }

    if(moduleToStore.size()){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "writing last values for  " << m_HVKey << endmsg;

      sc = this->commitDataToDetectorStore(attrListColl_hv, m_HVKey);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_HVKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_HVKey << endmsg;
 
      sc = this->registerIOVData(m_HVKey, m_HVTag);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_HVKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_HVKey << endmsg;


      moduleToStore.clear();
    }

  }


  if(m_FSMStatus){

    CondAttrListCollection* attrListColl_fsmstatus = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_fsmstatus = new coral::AttributeListSpecification(); 
    attrSpec_fsmstatus->extend(m_par_FSMStatusField.c_str(), "string");
    coral::AttributeList attrList_fsmstatus(*attrSpec_fsmstatus);

    std::string fsm = "";
    double startfsm =0;
    unsigned int  module_id =0;

    std::ifstream FSMData(m_FSMStatusFile.c_str());

    while(!FSMData.eof()){
 
      FSMData >> module_id >> startfsm >> fsm;
      if(!FSMData.fail()){

	unsigned int module_hash = (unsigned int)m_pixid->wafer_hash(Identifier(module_id));

	for(unsigned int i=0; i<moduleToStore.size(); i++){
	  if(moduleToStore[i] == module_hash){

	    sc = this->commitDataToDetectorStore(attrListColl_fsmstatus, m_FSMStatusKey);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStatusKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStatusKey << endmsg;

	    sc = this->registerIOVData(m_FSMStatusKey, m_FSMStatusTag);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStatusKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStatusKey << endmsg;


	    moduleToStore.clear();
	    attrListColl_fsmstatus = new CondAttrListCollection(true);
	    break;
	  }
	}
	moduleToStore.push_back(module_hash);

	CondAttrListCollection::ChanNum channum_fsmstatus = module_hash;
	attrList_fsmstatus[m_par_FSMStatusField.c_str()].setValue(fsm);
	attrListColl_fsmstatus->add(channum_fsmstatus, attrList_fsmstatus);
	IOVTime start_fsmstatus((uint64_t)(startfsm*1000000000));
	IOVTime stop_fsmstatus(IOVTime::MAXTIMESTAMP);
	IOVRange iov_fsmstatus(start_fsmstatus, stop_fsmstatus);
	attrListColl_fsmstatus->add(channum_fsmstatus, iov_fsmstatus);
      }
    }
 
    if(moduleToStore.size()){

      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "writing last values for  " << m_FSMStatusKey << endmsg;

      sc = this->commitDataToDetectorStore(attrListColl_fsmstatus, m_FSMStatusKey);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStatusKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStatusKey << endmsg;

      sc = this->registerIOVData(m_FSMStatusKey, m_FSMStatusTag);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStatusKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStatusKey << endmsg;

      moduleToStore.clear();
    }

  }


  if(m_FSMState){

    CondAttrListCollection* attrListColl_fsmstate = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec_fsmstate = new coral::AttributeListSpecification(); 
    attrSpec_fsmstate->extend(m_par_FSMStateField.c_str(), "string");
    coral::AttributeList attrList_fsmstate(*attrSpec_fsmstate);

    std::string fsm = "";
    double startfsm =0;
    unsigned int  module_id =0;

    std::ifstream FSMData(m_FSMStateFile.c_str());

    while(!FSMData.eof()){
 
      FSMData >> module_id >> startfsm >> fsm;
      if(!FSMData.fail()){

	unsigned int module_hash = (unsigned int)m_pixid->wafer_hash(Identifier(module_id));

	for(unsigned int i=0; i<moduleToStore.size(); i++){
	  if(moduleToStore[i] == module_hash){

	    sc = this->commitDataToDetectorStore(attrListColl_fsmstate, m_FSMStateKey);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStateKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStateKey << endmsg;

	    sc = this->registerIOVData(m_FSMStateKey, m_FSMStateTag);
	    if(sc.isFailure()){
	      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStateKey << endmsg;
	      return StatusCode::FAILURE;
	    }
	    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStateKey << endmsg;


	    moduleToStore.clear();
	    attrListColl_fsmstate = new CondAttrListCollection(true);
	    break;
	  }
	}
	moduleToStore.push_back(module_hash);

	CondAttrListCollection::ChanNum channum_fsmstate = module_hash;
	attrList_fsmstate[m_par_FSMStateField.c_str()].setValue(fsm);
	attrListColl_fsmstate->add(channum_fsmstate, attrList_fsmstate);
	IOVTime start_fsmstate((uint64_t)(startfsm*1000000000));
	IOVTime stop_fsmstate(IOVTime::MAXTIMESTAMP);
	IOVRange iov_fsmstate(start_fsmstate, stop_fsmstate);
	attrListColl_fsmstate->add(channum_fsmstate, iov_fsmstate);
      }
    }
 
    if(moduleToStore.size()){

      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "writing last values for  " << m_FSMStateKey << endmsg;

      sc = this->commitDataToDetectorStore(attrListColl_fsmstate, m_FSMStateKey);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not commit " << m_FSMStateKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully commit " << m_FSMStateKey << endmsg;

      sc = this->registerIOVData(m_FSMStateKey, m_FSMStateTag);
      if(sc.isFailure()){
	if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register " << m_FSMStateKey << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Successfully register " << m_FSMStateKey << endmsg;

      moduleToStore.clear();
    }

  }





  return StatusCode::SUCCESS;

}

StatusCode PixelDCSTool::recordDCSData() 
{
 if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "recordDCSData() "  << endmsg;
  
  StatusCode sc;

  PixelDCSData* dcsd;
  if(detStore()->contains<PixelDCSData>(m_pixelDCSDataSGKey)){
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "found " <<m_pixelDCSDataSGKey << endmsg;
    sc = detStore()->retrieve(dcsd,m_pixelDCSDataSGKey);
    if(!sc.isSuccess()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not retrieve PixelDCSData" << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "retrieved  " <<m_pixelDCSDataSGKey << endmsg;

    sc = detStore()->remove(dcsd);
    if(!sc.isSuccess()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not remove PixelDCSData from detector store" << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "removed  " <<m_pixelDCSDataSGKey << endmsg;

  }

  sc = detStore()->record(m_pixelDCSData,m_pixelDCSDataSGKey);  
  if(!sc.isSuccess()){
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not register PixelDCSData to detector store" << endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "recorded  " <<m_pixelDCSDataSGKey << endmsg;
  return sc;

}



StatusCode PixelDCSTool::commitDataToDetectorStore(CondAttrListCollection* attrListColl, std::string dataKey) const
{
 

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "commitDataToDetectorStore()" << endmsg;
  StatusCode sc = StatusCode::SUCCESS; 

  CondAttrListCollection* calc;

  if(detStore()->contains<CondAttrListCollection>(dataKey)){

    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "found  dataKey  "<< dataKey << endmsg;
    sc = detStore()->retrieve(calc,dataKey);

    if(!sc.isSuccess()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not retrieve CondAttrListCollection" << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "retrieved  dataKey  "<< dataKey << endmsg;

    sc = detStore()->removeDataAndProxy(calc);
    if(!sc.isSuccess()){
      if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "could not remove CondAttrListCollection from detector store" << endmsg;
      return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "removed  dataKey  "<< dataKey << endmsg;
  }

  sc = detStore()->record(attrListColl, dataKey); 
  if (sc.isFailure()) {
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Unable to record CondAttrListCollection with key " << dataKey << endmsg;
    return StatusCode::FAILURE;
  }

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Successfully record CondAttrListCollection with key " << dataKey << endmsg;

 
  return StatusCode::SUCCESS;

}


StatusCode PixelDCSTool::connectOutput()
{

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "in connectOutput() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS; 
  sc = m_streamer->connectOutput();
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING))msg(MSG::WARNING) <<"Could not connect Output"  <<endmsg;
    return( StatusCode::FAILURE);

  }
  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "connectOutput " << endmsg;

  return sc;

}

StatusCode PixelDCSTool::registerIOVData(std::string dataKey, std::string dataTag)
{
  

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "in registerIOVData()" << endmsg;

  StatusCode sc = StatusCode::SUCCESS; 

  for(int ai=0; ai<m_maxAttempt; ai++){
    sc = this->connectOutput();
    if (!sc.isFailure()) {
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Successfully connect output  "  << endmsg;
      break;
    }
    if (msgLvl(MSG::WARNING))msg(MSG::WARNING) <<"Could not connect output"  <<endmsg;
    if (msgLvl(MSG::INFO))msg(MSG::INFO) <<"retry to connect output attempt number " << ai+1 <<endmsg;
  }
  if (sc.isFailure()) {
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) <<"Could not connect Output"  <<endmsg;
    return( StatusCode::FAILURE);
  }

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "connectOutput ok " << dataKey << endmsg;

  IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(1);

  IAthenaOutputStreamTool::TypeKeyPair pair("CondAttrListCollection", dataKey);
  typeKeys[0] = pair;

  sc = m_streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) <<"Could not stream out typeKeys" << dataKey <<endmsg;
    return( StatusCode::FAILURE);
  }

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "streamObjects  ok " << dataKey << endmsg;

  sc = m_streamer->commitOutput();
  if (sc.isFailure()) {
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) <<"Could not commit output stream" <<endmsg;
    return( StatusCode::FAILURE);
  }

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "commitOutput ok " << dataKey << endmsg;

  sc = m_IOVRegistrationSvc->registerIOV( "CondAttrListCollection", dataKey, dataTag);
  if(sc.isFailure()) { 
    if (msgLvl(MSG::ERROR))msg(MSG::ERROR) <<"could not register IOV "<< dataKey <<endmsg; 
    return sc; 
  }

  if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "registerIOV ok " << dataKey << endmsg;


  return StatusCode::SUCCESS;


}


StatusCode PixelDCSTool::printData() const
{

  StatusCode sc;


  if(m_temperature){

    const CondAttrListCollection* atrc_temp; 
    StatusCode sc = detStore()->retrieve(atrc_temp, m_temperatureKey); 

    if(sc.isFailure()) { 
      std::cout<<"could not retrieve CondAttrListCollection from DB folder "<<m_temperatureKey<<std::endl; 
      return sc; 
    }

    CondAttrListCollection::const_iterator itrx_temp; 
    for(itrx_temp = atrc_temp->begin(); itrx_temp !=atrc_temp->end(); ++itrx_temp){ 
      CondAttrListCollection::ChanNum channum_temp = itrx_temp->first;
      const coral::AttributeList& atr_temp = itrx_temp->second; 
      std::cout <<"temperature     " << atr_temp[m_par_temperatureField.c_str()].data<float>() << std::endl;
 

      std::ostringstream attrStr_temp;
      atr_temp.toOutputStream(attrStr_temp);
      std::cout << "ChanNum " << channum_temp << " Attribute list " << attrStr_temp.str() << std::endl;

    }
  }

  if(m_HV){
    const CondAttrListCollection* atrc_hv; 
    sc = detStore()->retrieve(atrc_hv, m_HVKey); 

    if(sc.isFailure()) { 
      std::cout<<"could not retrieve CondAttrListCollection from DB folder "<<m_HVKey<<std::endl; 
      return sc; 
    }

    CondAttrListCollection::const_iterator itrx_hv; 
    for(itrx_hv = atrc_hv->begin(); itrx_hv !=atrc_hv->end(); ++itrx_hv){ 
      CondAttrListCollection::ChanNum channum_hv = itrx_hv->first;
      const coral::AttributeList& atr_hv = itrx_hv->second; 
      std::cout <<"HV     " <<  atr_hv[m_par_HVField.c_str()].data<float>() << std::endl;


      std::ostringstream attrStr_hv;
      atr_hv.toOutputStream(attrStr_hv);
      std::cout << "ChanNum " << channum_hv << " Attribute list " << attrStr_hv.str() << std::endl;

    }
  }

  if(m_FSMStatus){

    const CondAttrListCollection* atrc_fsmstatus; 
    sc = detStore()->retrieve(atrc_fsmstatus, m_FSMStatusKey); 

    if(sc.isFailure()) { 
      std::cout<<"could not retrieve CondAttrListCollection from DB folder "<<m_FSMStatusKey<<std::endl; 
      return sc; 
    }

    CondAttrListCollection::const_iterator itrx_fsmstatus; 
    for(itrx_fsmstatus = atrc_fsmstatus->begin(); itrx_fsmstatus !=atrc_fsmstatus->end(); ++itrx_fsmstatus){ 
      CondAttrListCollection::ChanNum channum_fsmstatus = itrx_fsmstatus->first;
      const coral::AttributeList& atr_fsmstatus = itrx_fsmstatus->second; 


      std::ostringstream attrStr_fsmstatus;
      atr_fsmstatus.toOutputStream(attrStr_fsmstatus);
      std::cout << "ChanNum " << channum_fsmstatus << " Attribute list " << attrStr_fsmstatus.str() << std::endl;

    }

  }


  if(m_FSMState){

    const CondAttrListCollection* atrc_fsmstate; 
    sc = detStore()->retrieve(atrc_fsmstate, m_FSMStateKey); 

    if(sc.isFailure()) { 
      std::cout<<"could not retrieve CondAttrListCollection from DB folder "<<m_FSMStateKey<<std::endl; 
      return sc; 
    }

    CondAttrListCollection::const_iterator itrx_fsmstate; 
    for(itrx_fsmstate = atrc_fsmstate->begin(); itrx_fsmstate !=atrc_fsmstate->end(); ++itrx_fsmstate){ 
      CondAttrListCollection::ChanNum channum_fsmstate = itrx_fsmstate->first;
      const coral::AttributeList& atr_fsmstate = itrx_fsmstate->second; 


      std::ostringstream attrStr_fsmstate;
      atr_fsmstate.toOutputStream(attrStr_fsmstate);
      std::cout << "ChanNum " << channum_fsmstate << " Attribute list " << attrStr_fsmstate.str() << std::endl;

    }

  }


  if(m_pixelDCSData){

    for(unsigned int dcsditr=0; dcsditr < m_pixid->wafer_hash_max();  dcsditr++){

      std::cout << *(*m_pixelDCSData)[dcsditr] << std::endl;

    }

  }


 
  return StatusCode::SUCCESS;
    

}



