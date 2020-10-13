/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSTestReadWrite.h"


// Athena includes
#include "AthenaKernel/IOVSvcDefs.h" 

// Gaudi includes
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"



PixelDCSTestReadWrite::PixelDCSTestReadWrite(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_write(false)
{

  declareProperty("Write",m_write);

}

  
PixelDCSTestReadWrite::~PixelDCSTestReadWrite()
{}


StatusCode PixelDCSTestReadWrite::initialize()
{
  //m_log.setLevel(outputLevel());
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Entering PixelDCSTestReadWrite::initialize()" << endmsg;


  StatusCode sc;
  
//   if (StatusCode::SUCCESS!=service("ToolSvc",m_toolsvc)) {
//     msg(MSG::FATAL) << "Unable to retrieve ToolSvc" << endmsg;
//     return StatusCode::FAILURE;
//   } 
//   msg(MSG::INFO) << "ToolSvc retrieved" << endmsg;
    
  // sc = m_toolsvc->retrieveTool("PixelDCSTool",m_pixelDCSTool);

  sc = m_pixelDCSTool.retrieve();
  if(sc.isFailure()){
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Unable to retrieve PixelDCSTool" << endmsg;
    return StatusCode::FAILURE;
  }

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "PixelDCSTool retrieved" << endmsg;

  return StatusCode::SUCCESS;


}


StatusCode PixelDCSTestReadWrite::execute(){

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " in PixelDCSTestReadWrite::execute()" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  if(m_write){
    sc = m_pixelDCSTool->writeDataToDB();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write data to database" << endmsg;
      return(StatusCode::FAILURE);
    }
  }
  else{
    sc = m_pixelDCSTool->printData();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not print data" << endmsg;
      return(StatusCode::FAILURE);
    }

  }
  return StatusCode::SUCCESS;

}  

StatusCode PixelDCSTestReadWrite::finalize(){

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "in PixelDCSTestReadWrite::finalize()" << endmsg;
  
  return StatusCode::SUCCESS;
}  
