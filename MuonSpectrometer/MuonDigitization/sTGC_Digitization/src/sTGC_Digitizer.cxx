/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Gaudi External 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
#include "sTGC_Digitization/sTGC_Digitizer.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

/*******************************************************************************/
sTGC_Digitizer::sTGC_Digitizer(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("sTgcDigitizationTool", this )
{
  declareProperty("sTGC_DigitizationTool", m_digTool);
}
/*******************************************************************************/
sTGC_Digitizer::~sTGC_Digitizer()  {}
/*******************************************************************************/
StatusCode sTGC_Digitizer::initialize() {   
  // intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL (" Could not retrieve sTGC Digitization Tool!" ); 
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ("Retrieved sTGC Digitization Tool." );
  return StatusCode::SUCCESS; 
}
/*******************************************************************************/
StatusCode sTGC_Digitizer::execute() {   
  ATH_MSG_DEBUG ("sTGC_Digitizer::in execute()"); 
  return m_digTool->digitize();
}
/*******************************************************************************/
StatusCode sTGC_Digitizer::finalize() { 
  ATH_MSG_INFO (" sTGC_Digitizer::finalize ");
  return StatusCode::SUCCESS;
}
/*******************************************************************************/

