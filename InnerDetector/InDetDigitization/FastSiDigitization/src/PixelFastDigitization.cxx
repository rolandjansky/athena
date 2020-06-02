/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/PixelFastDigitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
PixelFastDigitization::PixelFastDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode PixelFastDigitization::initialize() {

  ATH_CHECK (m_digTool.retrieve());

  ATH_MSG_VERBOSE ( "Retrieved Pixel Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------
StatusCode PixelFastDigitization::execute() {

  ATH_MSG_VERBOSE ( " PixelFastDigitization : execute()" );

  ATH_CHECK (m_digTool->processAllSubEvents(Gaudi::Hive::currentContext()));

  ATH_MSG_VERBOSE ( " PixelFastDigitization : m_digTool->processAllSubEvents()" );

  return StatusCode::SUCCESS;
}
