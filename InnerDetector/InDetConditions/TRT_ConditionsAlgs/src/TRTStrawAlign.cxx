/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRTStrawAlign.cxx
 * @brief Algorithm to implement individual straw alignment 
 * @author John Alison <johnda@hep.upenn.edu> and Peter Hansen <phansen@nbi.dk>
 **/

//
#include <fstream>
#include <iostream>
#include <string>
#include "TRT_ConditionsAlgs/TRTStrawAlign.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h" 
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TRT_ConditionsData/StrawDxContainer.h"

#include "TRT_ConditionsServices/ITRT_StrawAlignDbSvc.h"
#include "TRT_ConditionsServices/ITRT_AlignDbSvc.h"

TRTStrawAlign::TRTStrawAlign(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm   (name, pSvcLocator),
   p_caldbtool("TRT_StrawAlignDbSvc",name),
   p_aligndbtool("TRT_AlignDbSvc",name),
   m_trtman(0),
   m_trt(0),
   m_setup(false),
   m_doWriteToPOOL(false),
   m_doRegIOV(false),
   m_inputModuleAlignmentTextFile(""),
   m_inputStrawAlignmentTextFile(""),
   m_outputModuleAlignmentTextFile(""),
   m_outputStrawAlignmentTextFile(""),
   m_outputPOOLFile(""),
   m_moduleAlignTag("TRTAlign_00"),
   m_stawAlignTag("TrtCalibDx_00"),
   m_runRangeBegin(0),
   m_runRangeEnd(9999999),
   m_eventRangeBegin(0),
   m_eventRangeEnd(9999999)

{
  declareProperty("Write",m_doWriteToPOOL);
  declareProperty("RegisterIOV",m_doRegIOV);
  declareProperty("InputFile",m_inputModuleAlignmentTextFile);
  declareProperty("InputStrawAlignmentFile",m_inputStrawAlignmentTextFile);
  declareProperty("PoolOutputFile",m_outputPOOLFile);
  declareProperty("TextOutputFile",m_outputModuleAlignmentTextFile);
  declareProperty("StrawAlignmentTextOutputFile",m_outputStrawAlignmentTextFile);
  declareProperty("VersionTag",m_moduleAlignTag);
  declareProperty("VersionTagStrawAlign",m_stawAlignTag);
  declareProperty("ValidRun1",m_runRangeBegin);
  declareProperty("ValidRun2",m_runRangeEnd);
  declareProperty("ValidEvent1",m_eventRangeBegin);
  declareProperty("ValidEvent2",m_eventRangeEnd);
  declareProperty("DbTool",p_caldbtool);
  declareProperty("AlignDbTool",p_aligndbtool);
  declareProperty("DoStrawAlign",m_doStrawAlign);
  declareProperty("DoModuleAlign",m_doModuleAlign);
}


TRTStrawAlign::~TRTStrawAlign(void)
{}

StatusCode TRTStrawAlign::initialize() {

  ATH_MSG_DEBUG(" in initialize() ");

  //
  // Get TRT manager and ID helper
  StatusCode sc = AthAlgorithm::detStore()->retrieve(m_trtman,"TRT");
  if(sc.isFailure() || m_trtman==0) 
  {
    msg(MSG::FATAL) << "Could not find TRT manager " << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_trt,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Couldn't retrieve TRT ID helper." );
    return sc;
  }
  if(m_trt) ATH_MSG_DEBUG("TRT manager and helper found ");

  //get Database manager tools
  if (m_doStrawAlign) {
    if (StatusCode::SUCCESS!=p_caldbtool.retrieve()) {
      msg(MSG::FATAL) << "TRTStrawAlignDbTool not found" << endreq;
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG(" TRTStrawAlignDbTool found ");
    if (m_doWriteToPOOL && msgLvl(MSG::INFO)) msg(MSG::INFO)
		       << "Straw alignment will be written to POOL file " 
		       << m_outputPOOLFile << endreq;
    if (m_doRegIOV) {
      ATH_MSG_INFO("Straw alignment will be registered with IOV");
      if (msgLvl(MSG::INFO)) {
	msg(MSG::INFO) << " run range: "
			<< m_runRangeBegin << " to " << m_runRangeEnd << endreq;
	msg(MSG::INFO) << " version tag: " << m_stawAlignTag << endreq;
      }
    }
    if (m_inputStrawAlignmentTextFile!="" && msgLvl(MSG::INFO)) msg(MSG::INFO)
			       << "Straw Alignment will read from text file "
			       << m_inputStrawAlignmentTextFile << endreq;
    if (m_outputStrawAlignmentTextFile!="" && msgLvl(MSG::INFO)) msg(MSG::INFO)
				<< "Straw Alignment will be written on text file "
				<< m_outputStrawAlignmentTextFile << endreq;
  
  }
  if (m_doModuleAlign) {

    if (StatusCode::SUCCESS!=p_aligndbtool.retrieve()) {
      msg(MSG::FATAL) << "TRTAlignDbTool not found" << endreq;
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG(" TRTAlignDbTool found ");
    if (m_doWriteToPOOL && msgLvl(MSG::INFO)) msg(MSG::INFO)
		       << "Module alignment will be written to POOL file " 
		       << m_outputPOOLFile << endreq;
    if (m_doRegIOV) {
      ATH_MSG_INFO("Module alignment will be registered with IOV");
      if ( msgLvl(MSG::INFO) ) {
	msg(MSG::INFO) << " run range: " << m_runRangeBegin << " to " << m_runRangeEnd
		       << endreq; 
	msg(MSG::INFO) << " version tag: " << m_moduleAlignTag << endreq;
      }
    }
    if (m_inputModuleAlignmentTextFile!="" && msgLvl(MSG::INFO)) msg(MSG::INFO)
			       << "Module Alignment will read from text file "
			       << m_inputModuleAlignmentTextFile << endreq;
    if (m_outputModuleAlignmentTextFile!="" && msgLvl(MSG::INFO)) msg(MSG::INFO)
				<< "Module Alignment will be written on text file "
				<< m_outputModuleAlignmentTextFile << endreq;
  }

  return StatusCode::SUCCESS;
}


StatusCode TRTStrawAlign::execute() {

  StatusCode sc=StatusCode::SUCCESS;
  //
  // at first event:
  if (!m_setup) {
    m_setup=true;

    if (m_doStrawAlign) {
      // read alignment constants from text file
      if (m_inputStrawAlignmentTextFile!="") {
	sc=p_caldbtool->readTextFile(m_inputStrawAlignmentTextFile);
	if(sc!=StatusCode::SUCCESS) {
          msg(MSG::ERROR) << " Could not read input text file "
	        << endreq;
          return StatusCode::FAILURE;
	}
      }

      // write alignment constants to text file
      if (m_outputStrawAlignmentTextFile!="") {
	sc=p_caldbtool->writeTextFile(m_outputStrawAlignmentTextFile);
	if(sc!=StatusCode::SUCCESS) {
          msg(MSG::ERROR) << " Could not write output text file "
	        << endreq;
          return StatusCode::FAILURE;
	}
      }

      if(m_doWriteToPOOL) {
	if( StatusCode::SUCCESS != p_caldbtool->streamOutObjects()) {
	  msg(MSG::ERROR) << " Could not stream Straw Alignment objects to "
		<< m_outputPOOLFile << endreq;
	  return StatusCode::FAILURE;
	}
      }
      if(m_doRegIOV) {
	if( StatusCode::SUCCESS != p_caldbtool->registerObjects(m_stawAlignTag,m_runRangeBegin,m_eventRangeBegin,m_runRangeEnd,m_eventRangeEnd) ) {
	  msg(MSG::ERROR) << " Could not register Straw Alignment objects "
		<< endreq;
	  return StatusCode::FAILURE;
	}
      }
    }

    if (m_doModuleAlign) {
      // read alignment constants from text file
      if (m_inputModuleAlignmentTextFile!="") {
	sc=p_aligndbtool->readAlignTextFile(m_inputModuleAlignmentTextFile);
	if(sc!=StatusCode::SUCCESS) {
          msg(MSG::ERROR) << " Could not read input text file "
	        << endreq;
          return StatusCode::FAILURE;
	}
      }

      // write alignment constants to text file
      if (m_outputModuleAlignmentTextFile!="") {
	sc=p_aligndbtool->writeAlignTextFile(m_outputModuleAlignmentTextFile);
	if(sc!=StatusCode::SUCCESS) {
          msg(MSG::ERROR) << " Could not write output text file "      << endreq;
	  return StatusCode::FAILURE;
	}
      }

      if(m_doWriteToPOOL) {
	if( StatusCode::SUCCESS != p_aligndbtool->streamOutAlignObjects()) {
	  msg(MSG::ERROR) << " Could not stream Module Alignment objects to " << m_outputPOOLFile << endreq;
	  return StatusCode::FAILURE;
	}
      }
      if(m_doRegIOV) {
	if( StatusCode::SUCCESS != p_aligndbtool->registerAlignObjects(m_moduleAlignTag
								       ,m_runRangeBegin
								       ,m_eventRangeBegin
								       ,m_runRangeEnd
								       ,m_eventRangeEnd) ) {
	  
	  msg(MSG::ERROR) << " Could not register Module Alignment objects " << endreq;
	  return StatusCode::FAILURE;
	}
      }
    }

  }

  return StatusCode::SUCCESS;
}


StatusCode TRTStrawAlign::finalize() {

  //
  // check a few straw positions
  for(int strlay=0;strlay<3;strlay++) {
    for(int str=7;str<10;str++) {
      Identifier id=m_trt->layer_id(-1,1,0,strlay);
      const InDetDD::TRT_BaseElement* element = m_trtman->getElement(id);
      float y = element->strawCenter(str).x();
      float x = element->strawCenter(str).y();
      float z = element->strawCenter(str).z();
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	    << "bec -1 layer 0 sector 1 plane "
            << strlay << " straw " << str << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	    << " x " << x << " y " << y << " z " << z << endreq;
      id=m_trt->layer_id(1,1,0,strlay);
      element = m_trtman->getElement(id);
      y = element->strawCenter(str).x();
      x = element->strawCenter(str).y();
      z = element->strawCenter(str).z();
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	    << "bec 1 layer 0 sector 1 plane "
            << strlay << " straw " << str << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	    << " x " << x << " y " << y << " z " << z << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

