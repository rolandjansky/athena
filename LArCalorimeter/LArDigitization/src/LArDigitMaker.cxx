/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// +  "empty" algorithm to run LAr digitization                               +
// +  invoking new LArPileUpTool to do all the work                           +
// +                                                                          +
// +==========================================================================+
//
#include "LArDigitization/LArDigitMaker.h"

//----------------------------------------------------------------------------------

LArDigitMaker::LArDigitMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_LArPileUpTool("LArPileUpTool",this)
{
  declareProperty("LArPileUpTool",m_LArPileUpTool,"Tool to perform the real work");
  return;
}


// ----------------------------------------------------------------------------------

LArDigitMaker::~LArDigitMaker()
{
  return;
}


// ----------------------------------------------------------------------------------

StatusCode LArDigitMaker::initialize()
{

  ATH_MSG_DEBUG("initialize LArDigitMaker");

  if (m_LArPileUpTool.retrieve().isFailure())
  {
     ATH_MSG_ERROR(" Unable to access LArPileUpTool");
     return StatusCode::FAILURE;
  }


  ATH_MSG_DEBUG(" Initialization completed successfully");

  return StatusCode::SUCCESS;
}

// -------------------------------------------------------------------------------------

StatusCode LArDigitMaker::execute()
{

  if (m_LArPileUpTool->processAllSubEvents().isFailure()) {
       ATH_MSG_ERROR(" error in LArPileupTool::processAllSubEvents");
       return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------

StatusCode LArDigitMaker::finalize()
{
  ATH_MSG_DEBUG(" finalize LArDigitMaker");
  return StatusCode::SUCCESS;

}
