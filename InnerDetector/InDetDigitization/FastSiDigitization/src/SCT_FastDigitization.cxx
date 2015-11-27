/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/SCT_FastDigitization.h"
#include "FastSiDigitization/ISCT_FastDigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
SCT_FastDigitization::SCT_FastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("SCT_FastDigitizationTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the SCT digitization");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_FastDigitization::initialize() {
// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve SCT Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE ( "Retrieved SCT Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode SCT_FastDigitization::execute() {

  ATH_MSG_VERBOSE ( " SCT_FastDigitization : execute()" );

  StatusCode sc =  m_digTool->processAllSubEvents();

  ATH_MSG_VERBOSE ( " SCT_FastDigitization : m_digTool->processAllSubEvents()" );


  return sc;
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode SCT_FastDigitization::finalize() {

  ATH_MSG_VERBOSE ( "SCT_FastDigitization : finalize()" );

  return StatusCode::SUCCESS;
}
