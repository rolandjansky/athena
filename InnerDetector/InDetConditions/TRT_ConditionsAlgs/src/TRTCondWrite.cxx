/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <string>
#include "TRT_ConditionsAlgs/TRTCondWrite.h"
#include "InDetIdentifier/TRT_ID.h"

/**  @file TRTCondWrite.cxx
 *   Algoritm for publishing TRT Calibration constants to CondStore
 *
 *   If constants are supplied by text-files these are read
 *   filled into Storegate by help of TRT_CalDbSvc
 *
 *   If an output text file is specified, the contents of Storegate are dumped.
 *
 * @author Peter Hansen <phansen@nbi.dk>
**/

TRTCondWrite::TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm   (name, pSvcLocator),
   m_caldbtool("TRT_CalDbSvc",name),
   m_trtid(0),
   m_setup(false),
   m_par_calfile(""),
   m_par_caloutputfile("") //set this to calibout_n, where n=0,1,2,3 refers to the format
{
  // declare algorithm parameters
  declareProperty("CalibInputFile",m_par_calfile);
  declareProperty("CalibOutputFile",m_par_caloutputfile);
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

  //get calibration conditions service
  if (StatusCode::SUCCESS!=m_caldbtool.retrieve()) {
    msg(MSG::FATAL) << "TRT_CalDbSvc not found" << endmsg;
    return StatusCode::FAILURE;
  }

 
  if (m_par_calfile!="") ATH_MSG_INFO("Calibration constants will be read from text file ");

  return StatusCode::SUCCESS;
}

StatusCode TRTCondWrite::execute(){

  StatusCode sc = StatusCode::SUCCESS;
  //
  // at first event:
  if (!m_setup) {

    m_setup=true;


    //Read text file.
    if(!m_par_calfile.empty()) {
      msg(MSG::INFO) << " A calibration text file is supplied. Remember to block the relevant folders and set EvtMax=1" << endmsg;
      int format=0;

      sc=m_caldbtool->readTextFile(m_par_calfile, format);
      if(sc!=StatusCode::SUCCESS) {
         msg(MSG::ERROR) << " Could not read TRT Calibration from "
              << m_par_calfile << endmsg;
         return sc;
      }
    //Write text file.
    } else if (!m_par_caloutputfile.empty()) {

      std::ofstream outfile(m_par_caloutputfile.c_str());

      if(m_par_caloutputfile=="calibout_0.txt") {
        outfile << "# Fileformat=" << 0 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format0(outfile);
      } else if (m_par_caloutputfile=="calibout_1.txt") {
        outfile << "# Fileformat=" << 1 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format1(outfile);
      } else if (m_par_caloutputfile=="calibout_2.txt") {
        outfile << "# Fileformat=" << 2 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format2(outfile);
      } else if (m_par_caloutputfile=="calibout_3.txt") {
        outfile << "# Fileformat=" << 3 << std::endl ;
	sc=m_caldbtool->writeTextFile_Format3(outfile);
      } else {
        msg(MSG::ERROR) << " Output calibration text file must have name of the form caliibout_n.txt, n=0,1,2,3 " << endmsg;
        return StatusCode::FAILURE;
      }

      if(sc!=StatusCode::SUCCESS) {
        msg(MSG::ERROR) << " Could not write TRT Calibration to "
              << m_par_caloutputfile << endmsg;
        return sc;
      }
      outfile.close();
    }
  }

  return sc;
}


StatusCode TRTCondWrite::finalize() {
  return StatusCode::SUCCESS;
}
