/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Gaudi External
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MM_Digitization/MM_Digitizer.h"

/*******************************************************************************/
MM_Digitizer::MM_Digitizer(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("MmDigitizationTool", this )
{
  declareProperty("MM_DigitizationTool", m_digTool , "AthAlgTool which performs the MicroMegas digitization");
}

/*******************************************************************************/
MM_Digitizer::~MM_Digitizer()  {}
/*******************************************************************************/
StatusCode MM_Digitizer::initialize() {
  // intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ("MM_Digitizer::Could not retrieve MM Digitization Tool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ("MM_Digitizer::Retrieved MM Digitization Tool.");
  return StatusCode::SUCCESS;

}
/*******************************************************************************/
StatusCode MM_Digitizer::execute() {
  ATH_MSG_DEBUG ("MM_Digitizer::in execute()");
  return m_digTool->digitize();
}
/*******************************************************************************/
StatusCode MM_Digitizer::finalize() {
  ATH_MSG_INFO ("MM_Digitizer::finalize");
  return StatusCode::SUCCESS;
}
/*******************************************************************************/

