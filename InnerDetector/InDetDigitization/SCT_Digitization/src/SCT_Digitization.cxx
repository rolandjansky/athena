/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Digitization/SCT_Digitization.h"
#include "SCT_Digitization/SCT_DigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//---------------------------------------------------------------------- 
SCT_Digitization::SCT_Digitization(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_Digitization::initialize() {
  ATH_MSG_DEBUG("SCT_Digitization::initialize()");
  return StatusCode::SUCCESS ;
}

//----------------------------------------------------------------------
// Execute method:
//---------------------------------------------------------------------- 

StatusCode SCT_Digitization::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_sctDigitizationTool->processAllSubEvents();
} 

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode SCT_Digitization::finalize() {
  ATH_MSG_DEBUG("SCT_Digitization::finalize()");
  return StatusCode::SUCCESS;
}
