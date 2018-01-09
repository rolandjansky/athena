/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigitization.h"
#include "TRTDigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
TRTDigitization::TRTDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("TRTDigitizationTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the TRT digitization");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TRTDigitization::initialize() {
// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve TRT Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved TRT Digitization Tool." );
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------
StatusCode TRTDigitization::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  return m_digTool->processAllSubEvents();
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode TRTDigitization::finalize() {
  ATH_MSG_DEBUG ( "finalize()" );
  return StatusCode::SUCCESS;
}
