/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// PixelRecoDbTestWriteRead.cxx
// Algorithm to create Pixel offline Calib objects and place them in 
// Condition DB
// Author Tommaso Lari <tommaso.lari@mi.infn.it>
/////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
//#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "PixelRecoDbTestWriteRead.h" 
#include "PixelConditionsTools/IPixelRecoDbTool.h"
#include "GaudiKernel/IToolSvc.h"

PixelRecoDbTestWriteRead::PixelRecoDbTestWriteRead(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     // m_calibdbtool(0),
     m_calibdbtool("PixelRecoDbTool"),
     m_setup(false),
     par_read(false),
     par_write(false),
     par_rfile("")
{
 
  // declare algorithm parameters
  declareProperty("Read",par_read);
  declareProperty("Write",par_write);
  // declareProperty("OutputTextFile",par_rfile);
  // declareProperty("InputTextFile",par_wfile);
}


PixelRecoDbTestWriteRead::~PixelRecoDbTestWriteRead(void)
{}

StatusCode PixelRecoDbTestWriteRead::initialize() {
  
  msg(MSG::INFO) << "PixelRecoDbTestWriteRead::initialize() called" 
        << endreq;
  
  //get Database manager tool
  StatusCode sc; 
  //IToolSvc* m_toolsvc; 
  //sc = service("ToolSvc",m_toolsvc); 
  //sc = m_toolsvc->retrieveTool("PixelRecoDbTool","PixelRecoDbTool",
  //                             m_calibdbtool);
  sc = m_calibdbtool.retrieve();

  if (StatusCode::SUCCESS!=sc) {
    msg(MSG::FATAL) << "PixelRecoDbTool not found" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " PixelRecoDbTool found " << endreq;
  
  
  return StatusCode::SUCCESS;
}


StatusCode PixelRecoDbTestWriteRead::execute() {
  
  StatusCode sc;
  //
  // at first event:
    // create Conditions objects in the detectorstore
  msg(MSG::INFO)<<" Event execute "<<endreq; 

  if(!m_setup){
    m_setup = true; 
    // write calibration text file into database 
    // the name of the file is a property of the PixelRecoDbTool
    if (par_write){
      msg(MSG::INFO)<<" Write pixel calibration file into DB "<<endreq; 
      sc=m_calibdbtool->writePixelCalibTextFiletoDB();
      if(sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) <<" Could not write Pixel Calibration file: "<<par_wfile<<" into database. "<< endreq;
	return StatusCode::FAILURE;
      }
    }
    // read calibration constants from database 
    if (par_read){
      msg(MSG::INFO)<<" Read pixel calibration from database into a file: "<<endreq; 
      sc=m_calibdbtool->readPixelCalibDBtoTextFile();
      if(sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) << " Could not read Pixel Calibration objects from DB "<< endreq;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}
 
StatusCode PixelRecoDbTestWriteRead::finalize() 
{  
  msg(MSG::INFO)<<" PixelRecoDbTestWriteRead: finishing "<<endreq; 
  return StatusCode::SUCCESS;
}
