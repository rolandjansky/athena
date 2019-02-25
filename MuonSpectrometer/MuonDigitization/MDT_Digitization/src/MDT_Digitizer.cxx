/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_Digitization/MDT_Digitizer.h"
#include "PileUpTools/IPileUpTool.h"


MDT_Digitizer::MDT_Digitizer(const std::string& name,
                           ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode MDT_Digitizer::initialize() {
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG("Retrieved MdtDigitizationTool (" << m_digTool->name() << ").");

  return StatusCode::SUCCESS;
}

StatusCode MDT_Digitizer::execute() {
  ATH_MSG_DEBUG("in execute()");
  return m_digTool->processAllSubEvents();
}
