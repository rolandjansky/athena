/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <string>
#include "TRT_ConditionsAlgs/TRTCondWrite.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "InDetIdentifier/TRT_ID.h"
#include "AthenaKernel/IOVTime.h"

/**  @file TRTCondWrite.cxx
 *   Algoritm for publishing TRT Calibration constants to CondStore
 *
 *   If constants are supplied by text-files these are read
 *   filled into CondStore by help of TRT_CalDbSvc
 *
 *   If an output text file is specified, the contents of CondStore are dumped.
 *
 * @author Peter Hansen <phansen@nbi.dk>
**/

TRTCondWrite::TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator)
  :AthReentrantAlgorithm   (name, pSvcLocator),
   m_caldbtool("TRT_CalDbSvc",name),
   m_trtid(0),
   m_setup(false),
   par_calfile(""),
   par_caloutputfile(""), //set this to calibout_n, where n=0,1,2,3 refers to the format
   m_condSvc("CondSvc",name),
   m_rtReadKey("/TRT/Calib/RT"),
   m_errReadKey("/TRT/Calib/errors2d"),
   m_slopeReadKey("/TRT/Calib/slopes"),
   m_t0ReadKey("/TRT/Calib/T0"),
   m_rtWriteKey("/TRT/Calib/RT"),
   m_errWriteKey("/TRT/Calib/Errors2d"),
   m_slopeWriteKey("/TRT/Calib/Slopes"),
   m_t0WriteKey("/TRT/Calib/T0"),
   m_run1(1),
   m_lbl1(0),
   m_run2(2147483647),
   m_lbl2(2147483647)
{
  // declare algorithm parameters
  declareProperty("CalibInputFile",par_calfile);
  declareProperty("CalibOutputFile",par_caloutputfile);
  declareProperty("StartRunIOV",m_run1);
  declareProperty("StartLblIOV",m_lbl1);
  declareProperty("EndRunIOV",m_run2);
  declareProperty("EndLblIOV",m_lbl2);
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


  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read keys
  ATH_CHECK( m_rtReadKey.initialize() );
  ATH_CHECK( m_errReadKey.initialize() );
  ATH_CHECK( m_slopeReadKey.initialize() );
  ATH_CHECK( m_t0ReadKey.initialize() );
  // Write keys
  ATH_CHECK( m_rtWriteKey.initialize() );
  ATH_CHECK( m_errWriteKey.initialize() );
  ATH_CHECK( m_slopeWriteKey.initialize() );
  ATH_CHECK( m_t0WriteKey.initialize() );

 
  if (par_calfile!="") ATH_MSG_INFO("Calibration constants will be read from text file ");

  return StatusCode::SUCCESS;
}

StatusCode TRTCondWrite::execute_r(const EventContext& ctx) const {

  StatusCode sc = StatusCode::SUCCESS;
  //
  // at first event:
  if (!m_setup) {
    const_cast<TRTCondWrite*>(this)->m_setup=true;


    EventIDRange rangeW(runlbl(m_run1,m_lbl1),runlbl(m_run2,m_lbl2)); //IOV


    if(!par_calfile.empty()) {
      bool useit=true;
      m_caldbtool->useCachedPtr(useit);  //to avoid reading the pointers from CondStore before they are written

      msg(MSG::INFO) << " A calibration text file is supplied. Remember to block the relevant folders in InDetRecConditionsAccess.py" << endmsg;
      int format=0; // text file format
      //Read text file.
      sc=m_caldbtool->readTextFile(par_calfile, format);
      if(sc!=StatusCode::SUCCESS) {
         msg(MSG::ERROR) << " Could not read TRT Calibration from "
              << par_calfile << endmsg;
         return sc;
      }

      // In the following, the new containers are recorded in SG
      // Register Rt write handle
      //if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_rtWriteKey, m_rtWriteKey.dbKey()).isFailure()) {
      if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_rtWriteKey).isFailure()) {
          ATH_MSG_ERROR("unable to register WriteCondHandle " << m_rtWriteKey.fullKey() << " with CondSvc");
          return StatusCode::FAILURE;
      }
      SG::WriteCondHandle<RtRelationContainer> rtWriteHandle(m_rtWriteKey, ctx);

      if(rtWriteHandle.record(rangeW,m_caldbtool->getRtContainer()).isFailure()){
          ATH_MSG_ERROR("Failed to record RtRelationContainer " << rtWriteHandle.fullKey() << " with CondSvc ");
          return StatusCode::FAILURE;
      }

       // Register T0 write handle
      //if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_t0WriteKey, m_t0WriteKey.dbKey()).isFailure()) {
      if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_t0WriteKey).isFailure()) {
          ATH_MSG_ERROR("unable to register WriteCondHandle " << m_t0WriteKey.fullKey() << " with CondSvc");
          return StatusCode::FAILURE;
       }
       // Write container to SG
       SG::WriteCondHandle<StrawT0Container> t0WriteHandle(m_t0WriteKey, ctx);
       if(t0WriteHandle.record(rangeW,m_caldbtool->getT0Container()).isFailure()){
          ATH_MSG_ERROR("Failed to record StrawT0Container " << t0WriteHandle.fullKey() << " with CondSvc ");
          return StatusCode::FAILURE;
       }

       if(format>1) {  // the drift circle errors are included in the text file
        // Register Error write handle
	 //if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_errWriteKey, m_errWriteKey.dbKey()).isFailure() ) {
	 if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_errWriteKey).isFailure() ) {
          ATH_MSG_ERROR("unable to register WriteCondHandle " << m_errWriteKey.fullKey() << " with CondSvc");
          return StatusCode::FAILURE;
        }
        // Write container to SG
        SG::WriteCondHandle<RtRelationContainer> errWriteHandle(m_errWriteKey, ctx);
        if(errWriteHandle.record(rangeW,m_caldbtool->getErrContainer()).isFailure()){
          ATH_MSG_ERROR("Failed to record DC Error Container " << errWriteHandle.fullKey() << " with CondSvc ");
          return StatusCode::FAILURE;
        }
       } else {      // we read the errors from SG 
         SG::ReadCondHandle<RtRelationContainer> erc(m_errReadKey, ctx);
         m_caldbtool->setErrContainer( const_cast<RtRelationContainer*>(*erc) );
       }

       if(format>2) {  // the slope of the drift circle errors are included in the text file
         // Register slope write handle
	 //if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_slopeWriteKey, m_slopeWriteKey.dbKey()).isFailure() ) {
	 if ( m_condSvc->regHandle(const_cast<TRTCondWrite*>(this), m_slopeWriteKey).isFailure() ) {
            ATH_MSG_ERROR("unable to register WriteCondHandle " << m_slopeWriteKey.fullKey() << " with CondSvc");
            return StatusCode::FAILURE;
        }
        // Write container to SG
        SG::WriteCondHandle<RtRelationContainer> slopeWriteHandle(m_slopeWriteKey, ctx);
        if(slopeWriteHandle.record(rangeW,m_caldbtool->getSlopeContainer()).isFailure()){
           ATH_MSG_ERROR("Failed to record slope container " << slopeWriteHandle.fullKey() << " with CondSvc ");
           return StatusCode::FAILURE;
        }
       } else {      // we read the slopes from SG
         SG::ReadCondHandle<RtRelationContainer> slc(m_slopeReadKey, ctx);
         m_caldbtool->setSlopeContainer( const_cast<RtRelationContainer*>(*slc) );
       }

    } else {  //no text file supplied. Read from SG
      SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey, ctx);
      m_caldbtool->setRtContainer( const_cast<RtRelationContainer*>(*rtc) );
      SG::ReadCondHandle<StrawT0Container> t0c(m_t0ReadKey, ctx);
      m_caldbtool->setT0Container( const_cast<StrawT0Container*>(*t0c) );
      SG::ReadCondHandle<RtRelationContainer> erc(m_errReadKey, ctx);
      m_caldbtool->setErrContainer( const_cast<RtRelationContainer*>(*erc) );
      SG::ReadCondHandle<RtRelationContainer> slc(m_slopeReadKey, ctx);
      m_caldbtool->setSlopeContainer( const_cast<RtRelationContainer*>(*slc) );
    }
    //Dump conditions containers

    if (par_caloutputfile!="") {
      std::ofstream outfile(par_caloutputfile.c_str());

      if(par_caloutputfile=="calibout_0.txt") {
        outfile << "# Fileformat=" << 0 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format0(outfile);
      } else if (par_caloutputfile=="calibout_1.txt"){
        outfile << "# Fileformat=" << 1 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format1(outfile);
      } else if(par_caloutputfile=="calibout_2.txt") {
        outfile << "# Fileformat=" << 2 << std::endl ;
        sc=m_caldbtool->writeTextFile_Format2(outfile);
      } else if (par_caloutputfile=="calibout_3.txt") {
        outfile << "# Fileformat=" << 3 << std::endl ;
	sc=m_caldbtool->writeTextFile_Format3(outfile);
      } else {
        msg(MSG::ERROR) << " Output calibration text file must have name of the form caliibout_n.txt, n=0,1,2,3 " << endmsg;
        return StatusCode::FAILURE;
      }

      if(sc!=StatusCode::SUCCESS) {
        msg(MSG::ERROR) << " Could not write TRT Calibration to "
              << par_caloutputfile << endmsg;
        return sc;
      }
      outfile.close();
    }
    return sc;
  }
  // at each event:
  // cache an EventSpecificPtr in TRT_CalDbTool

  if(par_calfile.empty()) {
    SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey, ctx);
    m_caldbtool->setRtContainer( const_cast<RtRelationContainer*>(*rtc) );
    SG::ReadCondHandle<StrawT0Container> t0c(m_t0ReadKey, ctx);
    m_caldbtool->setT0Container( const_cast<StrawT0Container*>(*t0c) );
    SG::ReadCondHandle<RtRelationContainer> erc(m_errReadKey, ctx);
    m_caldbtool->setErrContainer( const_cast<RtRelationContainer*>(*erc) );
    SG::ReadCondHandle<RtRelationContainer> slc(m_slopeReadKey, ctx);
    m_caldbtool->setSlopeContainer( const_cast<RtRelationContainer*>(*slc) );
  }



  return sc;
}


StatusCode TRTCondWrite::finalize() {
  return StatusCode::SUCCESS;
}
