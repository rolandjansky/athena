/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FastTRT_Digitization/TRTFastDigitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
TRTFastDigitization::TRTFastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TRTFastDigitization::initialize() {
// intitialize store gate active store
  ATH_CHECK (m_digTool.retrieve());

  ATH_MSG_DEBUG ( "Retrieved TRT Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode TRTFastDigitization::execute() {

  ATH_MSG_VERBOSE ( "execute()" );

  ATH_CHECK (m_digTool->processAllSubEvents(Gaudi::Hive::currentContext()));

  ATH_MSG_DEBUG ( "m_digTool->processAllSubEvents()" );

  return StatusCode::SUCCESS;
}
