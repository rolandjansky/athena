/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSTestReadWrite.h"


// Athena includes
//#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IOVSvcDefs.h" 

// Gaudi includes
#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h"



PixelDCSTestReadWrite::PixelDCSTestReadWrite(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pixelDCSTool("PixelDCSTool"),
  m_write(false)
{

  declareProperty("Write",m_write);

}

  
PixelDCSTestReadWrite::~PixelDCSTestReadWrite()
{}


StatusCode PixelDCSTestReadWrite::initialize()
{
  //m_log.setLevel(outputLevel());
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Entering PixelDCSTestReadWrite::initialize()" << endreq;


  StatusCode sc;
  
//   if (StatusCode::SUCCESS!=service("ToolSvc",m_toolsvc)) {
//     msg(MSG::FATAL) << "Unable to retrieve ToolSvc" << endreq;
//     return StatusCode::FAILURE;
//   } 
//   msg(MSG::INFO) << "ToolSvc retrieved" << endreq;
    
  // sc = m_toolsvc->retrieveTool("PixelDCSTool",m_pixelDCSTool);

  sc = m_pixelDCSTool.retrieve();
  if(sc.isFailure()){
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Unable to retrieve PixelDCSTool" << endreq;
    return StatusCode::FAILURE;
  }

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "PixelDCSTool retrieved" << endreq;

  return StatusCode::SUCCESS;


}


StatusCode PixelDCSTestReadWrite::execute(){

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " in PixelDCSTestReadWrite::execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if(m_write){
    sc = m_pixelDCSTool->writeDataToDB();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write data to database" << endreq;
      return(StatusCode::FAILURE);
    }
  }
  else{
    sc = m_pixelDCSTool->printData();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not print data" << endreq;
      return(StatusCode::FAILURE);
    }

  }
  return StatusCode::SUCCESS;

}  

StatusCode PixelDCSTestReadWrite::finalize(){

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "in PixelDCSTestReadWrite::finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}  
