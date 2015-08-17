/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastTRT_Digitization/TRTFastDigitization.h"
#include "FastTRT_Digitization/ITRTFastDigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
TRTFastDigitization::TRTFastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("TRTFastDigitizationTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the TRT digitization");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TRTFastDigitization::initialize() {
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

StatusCode TRTFastDigitization::execute() {

  ATH_MSG_VERBOSE ( "execute()" );

  StatusCode sc =  m_digTool->processAllSubEvents();

  ATH_MSG_DEBUG ( "m_digTool->processAllSubEvents()" );


  return sc;
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode TRTFastDigitization::finalize() {

  ATH_MSG_VERBOSE ( "finalize()" );

  return StatusCode::SUCCESS;
}
