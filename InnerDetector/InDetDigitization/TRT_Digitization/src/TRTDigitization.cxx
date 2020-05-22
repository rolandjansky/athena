/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigitization.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
TRTDigitization::TRTDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TRTDigitization::initialize() {
  // intitialize store gate active store
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved TRT Digitization Tool." );
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------
StatusCode TRTDigitization::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
