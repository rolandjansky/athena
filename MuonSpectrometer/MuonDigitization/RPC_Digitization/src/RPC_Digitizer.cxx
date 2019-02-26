/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_Digitization/RPC_Digitizer.h"
#include "PileUpTools/IPileUpTool.h"


RPC_Digitizer::RPC_Digitizer(const std::string& name,
                           ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode RPC_Digitizer::initialize() {
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG("Retrieved RpcDigitizationTool (" << m_digTool->name() << ").");

  return StatusCode::SUCCESS;
}

StatusCode RPC_Digitizer::execute() {
  ATH_MSG_DEBUG("in execute()");
  return m_digTool->processAllSubEvents();
}
