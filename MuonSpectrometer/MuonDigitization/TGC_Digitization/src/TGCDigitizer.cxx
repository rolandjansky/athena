/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCDigitizer.h"
#include "PileUpTools/IPileUpTool.h"


TGCDigitizer::TGCDigitizer(const std::string& name,
                           ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode TGCDigitizer::initialize() {
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG("Retrieved TgcDigitizationTool (" << m_digTool->name() << ").");

  return StatusCode::SUCCESS;
}

StatusCode TGCDigitizer::execute() {
  ATH_MSG_DEBUG("in execute()");
  return m_digTool->processAllSubEvents();
}
