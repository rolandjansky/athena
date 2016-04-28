/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsSvc.cxx
 * implementation file for SCT conditions service giving sensors Vdep, crystal orientation and Mfr
 * @author shaun.roe@cern.ch
**/
#include <limits>
#include <cmath>
#include <algorithm>
#include <cstdio>

#include "SCT_SensorsSvc.h"


const std::string sensorsFolderName("/SCT/Sensors");

SCT_SensorsSvc::SCT_SensorsSvc( const std::string& name, ISvcLocator* pSvcLocator ):
  AthService(name, pSvcLocator),
  m_detStore("DetectorStore",name),
  m_sensorsManufacturer(0){
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsSvc::initialize(){
  std::cout <<" hi there "<<std::endl;
  m_sensorsManufacturer = new std::map<CondAttrListCollection::ChanNum, std::string >;
  // Retrieve detector store
  if (m_detStore.retrieve().isFailure())  return msg(MSG:: FATAL)<< "Detector service is not found!" << endreq, StatusCode::FAILURE;
  // Register callback function
  if (m_detStore->regFcn(&SCT_SensorsSvc::fillSensorsData, this, 
			 m_sensorsData,sensorsFolderName).isFailure()) {
    return msg(MSG::FATAL) << "Failed to register callback function for sensors" << endreq, StatusCode::FAILURE;
     }
  
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsSvc::finalize(){
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISCT_SensorsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void SCT_SensorsSvc::getSensorsData(std::vector<std::string> & /*userVector*/){ 

  //for (unsigned int i(0);i!=m_sensorsValues.size();++i){
  //userVector.push_back(m_sensorsValues[i]); 
  //}
}

//std::string SCT_SensorsSvc::getManufacturer(const IdentifierHash &hashId){
std::string SCT_SensorsSvc::getManufacturer(unsigned int i){

  std::string manufacturer="";
  manufacturer = (*m_sensorsManufacturer)[i];
  return manufacturer;
}

StatusCode SCT_SensorsSvc::fillSensorsData(int& /* i */ , std::list<std::string>& /*keys*/){
   if (m_detStore->retrieve(m_sensorsData,sensorsFolderName).isFailure())  return msg(MSG:: ERROR)<< "Could not fill sensors data" << endreq, StatusCode::FAILURE;

  CondAttrListCollection::const_iterator m_sensorsData_itr;
  for(m_sensorsData_itr = m_sensorsData->begin(); m_sensorsData_itr!= m_sensorsData->end(); ++m_sensorsData_itr)
    {
      CondAttrListCollection::ChanNum  channelNumber=m_sensorsData_itr->first;
      m_sensorsManufacturer->insert(std::make_pair(channelNumber, m_sensorsData_itr->second[0].data<std::string>()));
    }

  return StatusCode::SUCCESS;
}


