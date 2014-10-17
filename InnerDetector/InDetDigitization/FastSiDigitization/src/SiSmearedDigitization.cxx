/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/SiSmearedDigitization.h"
#include "FastSiDigitization/ISiSmearedDigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
SiSmearedDigitization::SiSmearedDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_smearTool("SiSmearedDigitizationTool", this )
  
{
  declareProperty("SiSmearedDigitizationTool", m_smearTool, "AthAlgTool which performs the Pixel or SCT smearing");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SiSmearedDigitization::initialize() {
// intitialize store gate active store
  
  if (m_smearTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve Silicon Smearing Tool!" );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO ( "Retrieved Silicon Smearing Tool." );
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode SiSmearedDigitization::execute() {

  ATH_MSG_INFO ( " SiSmearedDigitization : execute()" );

  StatusCode sc =  m_smearTool->processAllSubEvents();
  
  ATH_MSG_INFO ( " SiSmearedDigitization : m_smearTool->processAllSubEvents()" );
  
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Error in SiSmearedDigitization : m_smearTool->processAllSubEvents()" );
    return StatusCode::FAILURE;
  }
  
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Error in SiSmearedDigitization : m_digTool->processAllSubEvents()" );
    return StatusCode::FAILURE;
  }
  
  return sc;
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode SiSmearedDigitization::finalize() {
  
  ATH_MSG_INFO ( "SiSmearedDigitization : finalize()" );

  return StatusCode::SUCCESS;
}

