/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <string>
#include "TRT_ConditionsAlgs/TRTCondWrite.h"
#include "InDetIdentifier/TRT_ID.h"



TRTCondWrite::TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm   (name, pSvcLocator),
   m_caldbtool("TRT_CalDbSvc",name),
   m_trtid(0),
   m_setup(false),
   par_calfile("")
{

  // declare algorithm parameters
  declareProperty("CalibInputFile",par_calfile);
  declareProperty("DbTool",m_caldbtool);

}

TRTCondWrite::~TRTCondWrite(void)
{}

StatusCode TRTCondWrite::initialize() {

  ATH_MSG_DEBUG("TRTCondWrite::initialize() called");

  //
  // Get ID helper
  StatusCode sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }
  if(m_trtid) ATH_MSG_DEBUG("TRT manager and helper found ");

  //get Database manager tool
  if (StatusCode::SUCCESS!=m_caldbtool.retrieve()) {
    msg(MSG::FATAL) << "TRTCalDbTool not found" << endreq;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG(" TRTCalDbTool found ");

  ATH_MSG_DEBUG(" TRT Calibration objects will be created");

  if (par_calfile!="") {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << " Constants will read from text file " << par_calfile << endreq;
  } else {
    ATH_MSG_INFO(" You have supplied no input file ");
    ATH_MSG_INFO(" Assume you want to check the Db by dumping contents ");
  }    
 
  if (par_calfile!="") ATH_MSG_INFO("Calibration constants will be read from text file ");

  return StatusCode::SUCCESS;
}


StatusCode TRTCondWrite::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  //
  // at first event:
  if (!m_setup) {
    m_setup=true;

    if (par_calfile!="") {

      // read calibration constants from text file
      if (par_calfile!="") sc=m_caldbtool->readTextFile(par_calfile);
      if(sc!=StatusCode::SUCCESS) {
        msg(MSG::ERROR) << " Could not read TRT Calibration objects from "
              << par_calfile << endreq;
        return sc;
      }

    } else {

      sc=m_caldbtool->writeTextFile("caliboutput.txt",1);

    }
  }

  return sc;
}

StatusCode TRTCondWrite::finalize() {
  return StatusCode::SUCCESS;
}
