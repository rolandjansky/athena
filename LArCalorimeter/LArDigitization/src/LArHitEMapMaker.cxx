/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// +  "empty" algorithm to run LAr digitization                               +
// +  invoking new LArPileUpTool to do all the work                           +
// +                                                                          +
// +==========================================================================+
//
#include "LArHitEMapMaker.h"

//----------------------------------------------------------------------------------

LArHitEMapMaker::LArHitEMapMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_LArPileUpTool("LArPileUpTool",this)
{
  declareProperty("LArPileUpTool",m_LArPileUpTool,"Tool to perform the real work");
  return;
}

// ----------------------------------------------------------------------------------

StatusCode LArHitEMapMaker::initialize()
{

  ATH_MSG_DEBUG("initialize LArHitEMapMaker");

  if (m_LArPileUpTool.retrieve().isFailure())
  {
     ATH_MSG_ERROR(" Unable to access LArPileUpTool");
     return StatusCode::FAILURE;
  }


  ATH_MSG_DEBUG(" Initialization completed successfully");

  return StatusCode::SUCCESS;
}

// -------------------------------------------------------------------------------------

StatusCode LArHitEMapMaker::execute()
{

  if (m_LArPileUpTool->processAllSubEvents(Gaudi::Hive::currentContext()).isFailure()) {
       ATH_MSG_ERROR(" error in LArPileupTool::processAllSubEvents");
       return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}
