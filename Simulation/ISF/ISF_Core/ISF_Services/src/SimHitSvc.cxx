/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "SimHitSvc.h"

/** Constructor **/
ISF::SimHitSvc::SimHitSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_senDetTool("SensitiveDetectorMasterTool")
  , m_fastSimTool("FastSimulationMasterTool")
{
  declareProperty("SensitiveDetectorMasterTool", m_senDetTool );
  declareProperty("FastSimulationMasterTool", m_fastSimTool );
}


ISF::SimHitSvc::~SimHitSvc()
{
}

/** Initialize event */
StatusCode ISF::SimHitSvc::initializeEvent() {
  ATH_MSG_DEBUG("initializing hit collections");

  //FIXME Lazy initialization to be removed after FADS migration
  if(!m_senDetTool) {
    ATH_CHECK(m_senDetTool.retrieve());
  }
  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  return StatusCode::SUCCESS;
}

/** Simulation Call --- simply kills the particle */
StatusCode ISF::SimHitSvc::releaseEvent() {
  //FIXME Lazy initialization to be removed after FADS migration
  if(!m_senDetTool)
    {
      ATH_CHECK(m_senDetTool.retrieve());
    }
  ATH_CHECK(m_senDetTool->EndOfAthenaEvent());

  //FIXME Lazy initialization to be removed after FADS migration
  if(!m_fastSimTool)
    {
      ATH_CHECK(m_fastSimTool.retrieve());
    }
  ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  return StatusCode::SUCCESS;
}
