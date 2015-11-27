/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/PixelFastDigitization.h"
#include "FastSiDigitization/IPixelFastDigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
PixelFastDigitization::PixelFastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("PixelFastDigitizationTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the Pixel digitization");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode PixelFastDigitization::initialize() {
// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve Pixel Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE ( "Retrieved Pixel Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode PixelFastDigitization::execute() {

  ATH_MSG_VERBOSE ( " PixelFastDigitization : execute()" );

  StatusCode sc =  m_digTool->processAllSubEvents();

  ATH_MSG_VERBOSE ( " PixelFastDigitization : m_digTool->processAllSubEvents()" );


  return sc;
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode PixelFastDigitization::finalize() {

  ATH_MSG_VERBOSE ( "PixelFastDigitization : finalize()" );

  return StatusCode::SUCCESS;
}
