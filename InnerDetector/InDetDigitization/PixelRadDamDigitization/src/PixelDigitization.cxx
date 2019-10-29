/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelDigitization.cxx
//   Implementation file for class PixelDigitization
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////

#include "PixelDigitization.h"
#include "PileUpTools/IPileUpTool.h"

using namespace RadDam;

// Constructor with parameters:
PixelDigitization::PixelDigitization(const std::string &name,
                                     ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
}

// Initialize method:
StatusCode PixelDigitization::initialize() {

  ATH_MSG_DEBUG("initialize()");
  ATH_CHECK(m_pixelDigitizationTool.retrieve());
  ATH_MSG_DEBUG ( "Successfully retreived IPixelDigitizaitonTool." );
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PixelDigitization::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  return m_pixelDigitizationTool->processAllSubEvents();
}
