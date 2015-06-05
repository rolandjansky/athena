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
  m_detStore("DetectorStore",name){
  //initialize string vector
   for (unsigned int i(0);i!=4088;++i){
     m_sensorsValues.push_back("A");;
   }
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode SCT_SensorsSvc::initialize(){
  // Retrieve detector store
  if (m_detStore.retrieve().isFailure())  return msg(MSG:: FATAL)<< "Detector service is not found!" << endreq, StatusCode::FAILURE;
  // Register callback function
  if (m_detStore->regFcn(&SCT_SensorsSvc::fillSensorsData,this, m_sensorsData,sensorsFolderName).isFailure()) {
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

void SCT_SensorsSvc::getSensorsData(std::vector<std::string> & userVector){ 
  for (unsigned int i(0);i!=m_sensorsValues.size();++i){
    userVector.push_back(m_sensorsValues[i]); 
    }
}

StatusCode SCT_SensorsSvc::fillSensorsData(int& /* i */ , std::list<std::string>& /*keys*/){
   if (m_detStore->retrieve(m_sensorsData,sensorsFolderName).isFailure())  return msg(MSG:: ERROR)<< "Could not fill sensors data" << endreq, StatusCode::FAILURE;
  //DUMMY CODE
  for (unsigned int i(0);i!=4088;++i){
     m_sensorsValues[i]="B";
   }

  return StatusCode::SUCCESS;
}


