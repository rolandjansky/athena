/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_Digitization.h"

#include "PileUpTools/IPileUpTool.h"

LUCID_DigiTop::LUCID_DigiTop(const std::string& name,
                   ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_mergeTool("LUCID_PileUpTool", this)
{
  declareProperty("LUCID_PileUpTool", m_mergeTool);
}
StatusCode LUCID_DigiTop::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ); 
  if(m_mergeTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve LUCID_PileUpTool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved LUCID_DigiTopTool (" << m_mergeTool->name() << ").");
 
 return StatusCode::SUCCESS;
}
StatusCode LUCID_DigiTop::execute() {
  ATH_MSG_DEBUG("execute()");  
  return m_mergeTool->processAllSubEvents();
}
StatusCode LUCID_DigiTop::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}

