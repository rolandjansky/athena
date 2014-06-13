/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/PrintHICentralityData.h"
#include "HIGlobal/HICentralityData.h"

#include "StoreGate/StoreGateSvc.h"

PrintHICentralityData::PrintHICentralityData 
(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_sgSvc(0)
{
  // Set users' request
  declareProperty("HICentralityKey",  m_key="HICentrality");
  // to get cetrality bin in another percentage schema
  declareProperty("CentralityPercSchema",m_CentralityPercSchema);
}

StatusCode PrintHICentralityData::initialize(){
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHICentralityData from Initialize" << endreq;
  
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

StatusCode PrintHICentralityData::execute() 
{
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHICentralityData from execute" << endreq;
  
  const HICentralityData *hcd;
  if ( m_sgSvc->retrieve(hcd, m_key).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve HICentralityData:" << m_key
	<< endreq;

    return StatusCode::FAILURE;
  }

  // Have to use (non-const) copy to set a different percentage schema
  HICentralityData hcdcopy = *hcd;
  if(m_CentralityPercSchema.size()) hcdcopy.SetPerc(m_CentralityPercSchema);

  // printout 
  // ----------------------------
  hcdcopy.Print();

  // End of execution for each event
  return StatusCode::SUCCESS;
}

StatusCode PrintHICentralityData::finalize() {
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << ">>> PrintHICentralityData from finalize" << endreq;
  
  // End of finalization step
  return StatusCode::SUCCESS;
}
