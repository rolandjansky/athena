/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////// 
//TRTStrawStatusWrite.cxx  
////////////////////////////

#include "TRT_ConditionsAlgs/TRTStrawStatusWrite.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"

TRTStrawStatusWrite::TRTStrawStatusWrite( const std::string &name, ISvcLocator *pSvcLocator)  :  
  AthAlgorithm( name, pSvcLocator ),
  m_trtStrawStatusIF("TRT_StrawStatusSummarySvc",name),
  m_trtStrawStatus("TRT_StrawStatusSummarySvc",name),
  //par_statusfile("/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/AprilRepro/listHotStraws.collisions2010.athenaFormat.txt"),
  par_statusfile("dummyNonExisting_TRT_StrawStatus_InputFile"), //  /afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/2010_09_10_sasa/listNoisyStraws.0162690.athenaFormat.txt"), 
  par_statusfilepermanent(""),
  par_statusfileHT("HTtest.t"),
  m_setup(false)
{ 
  declareProperty("StatusTool",m_trtStrawStatusIF);
  declareProperty("StrawStatusTool",m_trtStrawStatus);
  declareProperty("StatusInputFile",par_statusfile);
  declareProperty("StatusInputFilePermanent",par_statusfilepermanent);
  declareProperty("StatusInputFileHT",par_statusfileHT);
}

TRTStrawStatusWrite::~TRTStrawStatusWrite( )
{ }

StatusCode TRTStrawStatusWrite::initialize()
{

//	par_statusfile= "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/AprilRepro/listHotStraws.collisions2010.athenaFormat.txt";
//  par_statusfile="/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/2010_09_10_sasa/listNoisyStraws.0162690.athenaFormat.txt";
//"/afs/cern.ch/user/a/attrtcal/TRT_Calibration/uploadedDB/Status/2010_06_30/listNoisyStraws.0158269.athenaFormat.txt";

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTStrawStatusWrite initialise" << endreq;
  StatusCode sc;

  if(StatusCode::SUCCESS != m_trtStrawStatusIF.retrieve()){
    msg(MSG::ERROR) << " Can't get TRTStrawtatusTool" << endreq;
    return StatusCode::FAILURE;
  }

  /*
  m_trtStrawStatus=dynamic_cast<TRTStrawStatusSummaryTool *>(&(* m_trtStrawStatusIF));
  if(!m_trtStrawStatus){
    msg(MSG::ERROR) << " Can't do a dynamic cast to TRTStrawStatusSummaryTool" << endreq;
    return StatusCode::FAILURE;
  }
*/

  if( m_trtStrawStatus.retrieve().isFailure() ) {
    msg(MSG::ERROR) << " Can't do a dynamic cast to TRTStrawStatusSummaryTool" << endreq;
    return StatusCode::FAILURE;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode TRTStrawStatusWrite::execute()
{

//  static bool first_event = true;

//  if ( !first_event )  return StatusCode::SUCCESS;

  StatusCode sc;

  //par_statusfile= "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/AprilRepro/listHotStraws.collisions2010.athenaFormat.txt";
//  par_statusfile="/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/2010_09_10_sasa/listNoisyStraws.0162690.athenaFormat.txt";
// "/afs/cern.ch/user/a/attrtcal/TRT_Calibration/uploadedDB/Status/2010_06_30/listNoisyStraws.0158269.athenaFormat.txt";

  msg(MSG::INFO) << "TRTStrawStatusWrite::execute write DB tag for straw status file " << par_statusfile << endreq;

  //
  // at first event:
//  if (!m_setup) {
//    m_setup=true;

//    if ( (par_statusfile!="") ||  (par_statusfilepermanent!="") ) {
      // read status constants from text file
//      if (par_statusfile!="") 
sc=m_trtStrawStatus->readFromTextFile(par_statusfile);
//      if (par_statusfilepermanent!="") sc=m_trtStrawStatus->readFromTextFile(par_statusfilepermanent);
//      if (par_statusfileHT!="") sc=m_trtStrawStatus->readFromTextFile(par_statusfileHT);

      if(sc!=StatusCode::SUCCESS) {
        msg(MSG::ERROR) << " Could not read TRT StrawStatusSummary objects from "
              << par_statusfile << endreq;
        return StatusCode::FAILURE;
      }
//
//    } else {
//
//      sc=m_trtStrawStatus->writeToTextFile("strawstatussummary.txt");
//
//    }
//  }
 // first_event=false;
  return StatusCode::SUCCESS;

}

StatusCode TRTStrawStatusWrite::finalize()
{
  
  return StatusCode::SUCCESS;
}
