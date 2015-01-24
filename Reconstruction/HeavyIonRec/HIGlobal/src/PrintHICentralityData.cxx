/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/PrintHICentralityData.h"
#include "HIGlobal/HICentralityData.h"

#include "StoreGate/StoreGateSvc.h"

PrintHICentralityData::PrintHICentralityData 
(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  // Set users' request
  declareProperty("HICentralityKey",  m_key="HICentrality");
  // to get cetrality bin in another percentage schema
  declareProperty("CentralityPercSchema",m_CentralityPercSchema);
}

StatusCode PrintHICentralityData::initialize(){
  
  ATH_MSG_DEBUG( ">>> PrintHICentralityData from Initialize" );
  return StatusCode::SUCCESS;
}

StatusCode PrintHICentralityData::execute() 
{
  ATH_MSG_DEBUG( ">>> PrintHICentralityData from execute" );
  
  const HICentralityData *hcd = nullptr;
  ATH_CHECK( evtStore()->retrieve(hcd, m_key) );

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
  
  ATH_MSG_DEBUG( ">>> PrintHICentralityData from finalize" );
  return StatusCode::SUCCESS;
}
