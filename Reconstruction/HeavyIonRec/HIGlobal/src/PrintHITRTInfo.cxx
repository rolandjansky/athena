/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/PrintHITRTInfo.h"
#include "HIGlobal/HITRTInfo.h"

#include "StoreGate/StoreGateSvc.h"

PrintHITRTInfo::PrintHITRTInfo 
(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_sgSvc(0)
{
  // Set users' request
  declareProperty("HITRT",  m_key="HITRT");
  // to get cetrality bin in another percentage schema
  declareProperty("OccupancyPerPart",m_OccupancyPerPart);
}

StatusCode PrintHITRTInfo::initialize(){
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHITRTInfo from Initialize" << endreq;
  
  // Get the Storegate collection
  m_sgSvc = 0;
  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }
  
  // Initialization terminated
  return StatusCode::SUCCESS;
}

StatusCode PrintHITRTInfo::execute() 
{
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHITRTInfo from execute" << endreq;
  
  const HITRTInfo *hcd;
  if ( m_sgSvc->retrieve(hcd, m_key).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve HITRTInfo:" << m_key
	<< endreq;

    return StatusCode::FAILURE;
  }

  // Have to use (non-const) copy to set a different percentage schema
  HITRTInfo hcdcopy = *hcd;
  //if(m_OccupancyPerPart.size()) hcdcopy.SetPerc(m_OccupancyPerPart);

  // printout 
  // ----------------------------
  //hcdcopy.Print();

  // End of execution for each event
  return StatusCode::SUCCESS;
}

StatusCode PrintHITRTInfo::finalize() {
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHITRTInfo from finalize" << endreq;
  
  // End of finalization step
  return StatusCode::SUCCESS;
}
