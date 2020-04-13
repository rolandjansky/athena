/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Digitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
SCT_Digitization::SCT_Digitization(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_Digitization::initialize() {
  ATH_MSG_DEBUG("SCT_Digitization::initialize()");
  return StatusCode::SUCCESS ;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode SCT_Digitization::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_sctDigitizationTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
