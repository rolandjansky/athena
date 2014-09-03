/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DumpCabling.h"
#include "PixelCablingData.h"
#include "Identifier/Identifier.h"

//#include "PixelConditionsServices/IPixelRunConditionsDbSvc.h"
#include "PixelCabling/IPixelConnCoolDbSvc.h"
#include "PixelCabling/IPixelReadoutSpeedDbSvc.h"
#include "PixelCabling/IPixelCablingSvc.h"

DumpCabling::DumpCabling(const std::string& name,
			 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  //  m_runCondDbSvc("PixelRunConditionsDbSvc", name),
  m_connCoolDbSvc("PixelConnCoolDbSvc", name),
  m_readoutspeedDbSvc("PixelReadoutSpeedDbSvc", name),
  m_pixelCablingSvc("PixelCablingSvc", name),
  m_write(false)
{
  declareProperty("Write", m_write);
}

StatusCode DumpCabling::initialize()
{
  msg(MSG::INFO) << "succesfully retrieved PixelConnCoolDbSvc" << endreq;


//   if(!m_connCoolDbSvc.retrieve().isSuccess()){
//     ATH_MSG_FATAL("Unable to retrieve ConnCoolDbSvc");
//     return StatusCode::FAILURE;
//   }
//   msg(MSG::INFO) << "succesfully retrieved PixelConnCoolDbSvc" << endreq;

  if(!m_readoutspeedDbSvc.retrieve().isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve readoutspeedDbSvc");
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "succesfully retrieved PixelReadoutSpeedDbSvc" << endreq;

  if(!m_pixelCablingSvc.retrieve().isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve pixelCablingSvc");
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "succesfully retrieved pixelCablingSvc" << endreq;


  return StatusCode::SUCCESS;
}

StatusCode DumpCabling::execute()
{
  // here we should get the readout speed PER ROR stored by on-line CoolAgent
  const std::string readoutspeed_per_rod = m_readoutspeedDbSvc->readoutspeed_per_ROD() ;

  std::cout << "readoutspeed_per_rod: " << readoutspeed_per_rod << std::endl; 

  return StatusCode::SUCCESS;
}

StatusCode DumpCabling::finalize()
{
  return StatusCode::SUCCESS;
}
