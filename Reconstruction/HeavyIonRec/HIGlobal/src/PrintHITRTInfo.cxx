/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/PrintHITRTInfo.h"
#include "HIGlobal/HITRTInfo.h"

#include "StoreGate/StoreGateSvc.h"

PrintHITRTInfo::PrintHITRTInfo 
(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  // Set users' request
  declareProperty("HITRT",  m_key="HITRT");
  // to get cetrality bin in another percentage schema
  declareProperty("OccupancyPerPart",m_OccupancyPerPart);
}

StatusCode PrintHITRTInfo::initialize(){
  
  ATH_MSG_DEBUG( ">>> PrintHITRTInfo from Initialize" );
  return StatusCode::SUCCESS;
}

StatusCode PrintHITRTInfo::execute() 
{
  ATH_MSG_DEBUG( ">>> PrintHITRTInfo from execute" );
  
  const HITRTInfo *hcd = nullptr;
  ATH_CHECK( evtStore()->retrieve(hcd, m_key) );

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
  
  ATH_MSG_DEBUG( ">>> PrintHITRTInfo from finalize" );
  return StatusCode::SUCCESS;
}
