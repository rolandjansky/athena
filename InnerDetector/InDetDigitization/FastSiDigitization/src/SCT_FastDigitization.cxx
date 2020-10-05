/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/SCT_FastDigitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
SCT_FastDigitization::SCT_FastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_FastDigitization::initialize() {

  ATH_CHECK (m_digTool.retrieve());

  ATH_MSG_VERBOSE ( "Retrieved SCT Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode SCT_FastDigitization::execute() {

  ATH_MSG_VERBOSE ( " SCT_FastDigitization : execute()" );

  ATH_CHECK (m_digTool->processAllSubEvents(Gaudi::Hive::currentContext()));

  ATH_MSG_VERBOSE ( " SCT_FastDigitization : m_digTool->processAllSubEvents()" );

  return StatusCode::SUCCESS;
}
