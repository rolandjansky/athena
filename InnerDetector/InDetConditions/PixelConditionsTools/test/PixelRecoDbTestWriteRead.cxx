/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "Identifier/Identifier.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "PixelRecoDbTestWriteRead.h" 
#include "GaudiKernel/IToolSvc.h"

PixelRecoDbTestWriteRead::PixelRecoDbTestWriteRead(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     m_setup(false),
     m_par_read(false),
     m_par_write(false),
     m_par_rfile("")
{
 
  // declare algorithm parameters
  declareProperty("Read",m_par_read);
  declareProperty("Write",m_par_write);
  // declareProperty("OutputTextFile",m_par_rfile);
  // declareProperty("InputTextFile",m_par_wfile);
}


PixelRecoDbTestWriteRead::~PixelRecoDbTestWriteRead(void)
{}

StatusCode PixelRecoDbTestWriteRead::initialize() {
  
  msg(MSG::INFO) << "PixelRecoDbTestWriteRead::initialize() called" 
        << endmsg;
  
  //get Database manager tool
  StatusCode sc; 
  //IToolSvc* m_toolsvc; 
  //sc = service("ToolSvc",m_toolsvc); 
  //sc = m_toolsvc->retrieveTool("PixelRecoDbTool","PixelRecoDbTool",
  //                             m_calibdbtool);
  sc = m_calibdbtool.retrieve();

  if (StatusCode::SUCCESS!=sc) {
    msg(MSG::FATAL) << "PixelRecoDbTool not found" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " PixelRecoDbTool found " << endmsg;
  
  
  return StatusCode::SUCCESS;
}


StatusCode PixelRecoDbTestWriteRead::execute() {
  
  StatusCode sc;
  //
  // at first event:
    // create Conditions objects in the detectorstore
  msg(MSG::INFO)<<" Event execute "<<endmsg; 

  if(!m_setup){
    m_setup = true; 
    // write calibration text file into database 
    // the name of the file is a property of the PixelRecoDbTool
    if (m_par_write){
      msg(MSG::INFO)<<" Write pixel calibration file into DB "<<endmsg; 
      sc=m_calibdbtool->writePixelCalibTextFiletoDB();
      if(sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) <<" Could not write Pixel Calibration file: "<<m_par_wfile<<" into database. "<< endmsg;
	return StatusCode::FAILURE;
      }
    }
    // read calibration constants from database 
    if (m_par_read){
      msg(MSG::INFO)<<" Read pixel calibration from database into a file: "<<endmsg; 
      sc=m_calibdbtool->readPixelCalibDBtoTextFile();
      if(sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) << " Could not read Pixel Calibration objects from DB "<< endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}
 
StatusCode PixelRecoDbTestWriteRead::finalize() 
{  
  msg(MSG::INFO)<<" PixelRecoDbTestWriteRead: finishing "<<endmsg; 
  return StatusCode::SUCCESS;
}
