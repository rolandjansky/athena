/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// PixelCalibDbTestWriteRead.cxx
// Algorithm to create Pixel Calib objects and place them in Condition DB
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "PixelConditionsData/PixelCalibDataColl.h"
#include "PixelCalibDbTestWriteRead.h" 
#include "PixelCalibDbTool.h"
#include "GaudiKernel/IToolSvc.h"

PixelCalibDbTestWriteRead::PixelCalibDbTestWriteRead(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     m_calibdbtool("PixelCalibDbTool"),
     m_setup(false),
     par_read(false),
     par_write(false),
     par_rfile(""),
     par_wfile("")
{
 
  // declare algorithm parameters
  declareProperty("Read",par_read);
  declareProperty("Write",par_write);
  declareProperty("OutputTextFile",par_rfile);
  declareProperty("InputTextFile",par_wfile);
}


PixelCalibDbTestWriteRead::~PixelCalibDbTestWriteRead(void)
{}

StatusCode PixelCalibDbTestWriteRead::initialize() {
  
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "PixelCalibDbTestWriteRead::initialize() called" << endmsg;
  
  //get Database manager tool
  StatusCode sc; 
  //IToolSvc* m_toolsvc; 
  //sc = service("ToolSvc",m_toolsvc); 
  sc = m_calibdbtool.retrieve();

  if (StatusCode::SUCCESS!=sc) {
    if(msgLvl(MSG::FATAL))msg(MSG::FATAL) << "PixelCalibDbTool not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << " PixelCalibDbTool found " << endmsg;
  
  //print the options
  if (par_read){
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << " Read from Pixel Conditions database into a text file: " << par_rfile<<endmsg;
    if(par_rfile ==""){
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<< " It's reading, Output text file is required "<<endmsg; 
      return StatusCode::FAILURE; 
    }
  }
  if (par_write){
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<< " Write Calibration  file: "<<par_wfile << " into DB "<< endmsg;
    if(par_wfile ==""){ 
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<< " It's writing, the input text file is  required "<<endmsg; 
      return StatusCode::FAILURE; 
    }
  }  
  return StatusCode::SUCCESS;
}


StatusCode PixelCalibDbTestWriteRead::execute() {
  
  StatusCode sc;
  //
  // at first event:
    // create Conditions objects in the detectorstore
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" Event execute "<<endmsg; 

  if(!m_setup){
    m_setup = true; 
    // write calibration text file into database 
    if (par_write){
      if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" Write pixel calibration file: "<< par_wfile<< " into DB "<<endmsg; 
      sc=m_calibdbtool->writePixelCalibTextFiletoDB(par_wfile);
      if(sc!=StatusCode::SUCCESS) {
	if(msgLvl(MSG::ERROR))msg(MSG::ERROR) <<" Could not write Pixel Calibration file: "<<par_wfile<<" into database. "<< endmsg;
	return StatusCode::FAILURE;
      }
    }
    // read calibration constants from database 
    if (par_read){
      if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" Read pixel calibration from database into a file: "<< par_rfile<<endmsg; 
      sc=m_calibdbtool->readPixelCalibDBtoTextFile(par_rfile);
      if(sc!=StatusCode::SUCCESS) {
	if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << " Could not read Pixel Calibration objects from DB "<< endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}
 
StatusCode PixelCalibDbTestWriteRead::finalize() 
{  
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" PixelCalibDbTestWriteRead: finishing "<<endmsg; 
  return StatusCode::SUCCESS;
}
