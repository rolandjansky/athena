/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_Digitization.h"

#include "PileUpTools/IPileUpTool.h"

LUCID_DigiTop::LUCID_DigiTop(const std::string& name,
                   ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
}

StatusCode LUCID_DigiTop::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK(m_mergeTool.retrieve());
  ATH_MSG_DEBUG("Retrieved LUCID_DigiTopTool (" << m_mergeTool->name() << ").");

 return StatusCode::SUCCESS;
}

StatusCode LUCID_DigiTop::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_mergeTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
