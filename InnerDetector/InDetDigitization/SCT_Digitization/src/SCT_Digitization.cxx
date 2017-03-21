/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Digitization/SCT_Digitization.h"
#include "SCT_Digitization/SCT_DigitizationTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//---------------------------------------------------------------------- 
SCT_Digitization::SCT_Digitization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_sctDigitizationTool("SCT_DigitizationTool", this)
{
  declareProperty("DigitizationTool",     m_sctDigitizationTool,                 "SCT_DigitizationTool name");              
}

SCT_Digitization::~SCT_Digitization()
{

}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_Digitization::initialize() {

  ATH_MSG_DEBUG ( "SCT_Digitization::initialize()" );

  if (m_sctDigitizationTool.retrieve().isFailure())
  {
    ATH_MSG_FATAL ( "Could not retrieve ISCT_DigitizationTool");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ( "Successfully retreived ISCT_DigitizaitonTool." );

  return StatusCode::SUCCESS ;
}

//----------------------------------------------------------------------
// Execute method:
//---------------------------------------------------------------------- 

StatusCode SCT_Digitization::execute() {
 
  ATH_MSG_DEBUG ( "execute()" );
  return m_sctDigitizationTool->processAllSubEvents();
} 

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode SCT_Digitization::finalize() {

  ATH_MSG_DEBUG ( "SCT_Digitization::finalize()" );
  return StatusCode::SUCCESS;
}
