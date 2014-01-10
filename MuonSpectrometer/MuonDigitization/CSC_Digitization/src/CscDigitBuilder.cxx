/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, October 27 2003

// Digitization algorithm for the CSC hits

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "CSC_Digitization/CscDigitBuilder.h"

CscDigitBuilder::CscDigitBuilder(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("CscDigitizationTool", this )
{
  declareProperty("DigitizationTool", m_digTool);
}

CscDigitBuilder::~CscDigitBuilder()  {

}

StatusCode CscDigitBuilder::initialize() {
  // intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve CSC Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved CSC Digitization Tool." );

  return StatusCode::SUCCESS;
}

StatusCode CscDigitBuilder::execute() {

  ATH_MSG_DEBUG ( "in execute()" );

  return m_digTool->digitize();
}

StatusCode CscDigitBuilder::finalize() {
    
  ATH_MSG_DEBUG ( "finalize." );

  return StatusCode::SUCCESS;
}


