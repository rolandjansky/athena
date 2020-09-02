/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FastSiDigitization/SiSmearedDigitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
SiSmearedDigitization::SiSmearedDigitization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SiSmearedDigitization::initialize() {

  ATH_CHECK (m_smearTool.retrieve());

  ATH_MSG_INFO ( "Retrieved Silicon Smearing Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode SiSmearedDigitization::execute() {

  ATH_MSG_INFO ( " SiSmearedDigitization : execute()" );

  ATH_CHECK (m_smearTool->processAllSubEvents(Gaudi::Hive::currentContext()));

  ATH_MSG_INFO ( " SiSmearedDigitization : m_smearTool->processAllSubEvents()" );

  return StatusCode::SUCCESS;
}

