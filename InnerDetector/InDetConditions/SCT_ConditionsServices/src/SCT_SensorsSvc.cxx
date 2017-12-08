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
  m_sensorsManufacturer{} {
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsSvc::initialize(){
  // Retrieve detector store
  if (m_detStore.retrieve().isFailure())  return msg(MSG:: FATAL)<< "Detector service is not found!" << endmsg, StatusCode::FAILURE;
  // Register callback function
  if (m_detStore->regFcn(&SCT_SensorsSvc::fillSensorsData, this, 
                         m_sensorsData,sensorsFolderName).isFailure()) {
    return msg(MSG::FATAL) << "Failed to register callback function for sensors" << endmsg, StatusCode::FAILURE;
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

void SCT_SensorsSvc::getSensorsData(std::vector<std::string> & userVector){ 
  for (unsigned int i(0);i!=m_sensorsValues.size();++i){
    userVector.push_back(m_sensorsValues[i]);
  }
}

std::string SCT_SensorsSvc::getManufacturer(unsigned int i){
  std::string manufacturer="";
  manufacturer = m_sensorsManufacturer[i];
  return manufacturer;
}

void SCT_SensorsSvc::printManufacturers() {
  for(auto it: m_sensorsManufacturer) {
    ATH_MSG_INFO("channel " << it.first << " manufacturer " << it.second);
  }
}

StatusCode SCT_SensorsSvc::fillSensorsData(int& /* i */ , std::list<std::string>& /*keys*/){
  if (m_detStore->retrieve(m_sensorsData,sensorsFolderName).isFailure()) {
    msg(MSG:: ERROR)<< "Could not fill sensors data" << endmsg;
    return StatusCode::FAILURE;
  }

  CondAttrListCollection::const_iterator sensorsData_itr;
  for(sensorsData_itr = m_sensorsData->begin(); sensorsData_itr!= m_sensorsData->end(); ++sensorsData_itr)
    {
      CondAttrListCollection::ChanNum  channelNumber=sensorsData_itr->first;
      m_sensorsManufacturer[channelNumber] = sensorsData_itr->second[0].data<std::string>();
    }

  return StatusCode::SUCCESS;
}


