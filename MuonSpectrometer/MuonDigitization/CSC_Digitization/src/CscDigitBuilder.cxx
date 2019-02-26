/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, October 27 2003

// Digitization algorithm for the CSC hits

#include "CSC_Digitization/CscDigitBuilder.h"
#include "PileUpTools/IPileUpTool.h"

CscDigitBuilder::CscDigitBuilder(const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode CscDigitBuilder::initialize() {
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved CscDigitizationTool (" << m_digTool->name() << ")." );

  return StatusCode::SUCCESS;
}

StatusCode CscDigitBuilder::execute() {
  ATH_MSG_DEBUG ( "in execute()" );
  return m_digTool->processAllSubEvents();
}
