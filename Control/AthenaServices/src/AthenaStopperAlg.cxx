/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaStopperAlg.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/ServiceHandle.h"


AthenaStopperAlg::AthenaStopperAlg(const std::string& name, ISvcLocator* ploc)
  : AthAlgorithm(name, ploc)
{
  declareProperty("StopCount", m_stopcount);
}


StatusCode AthenaStopperAlg::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode AthenaStopperAlg::execute()
{
  static int count = 0;

  if ( ++count >= m_stopcount ) {
    ATH_MSG_INFO( "scheduling a event processing stop...." );
    ServiceHandle<IEventProcessor> evt ("ApplicationMgr", name());
    ATH_CHECK( evt.retrieve() );
    ATH_CHECK( evt->stopRun() );
  }

  return StatusCode::SUCCESS;
}


StatusCode AthenaStopperAlg::finalize()
{
  return StatusCode::SUCCESS;
}
