/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataReplicationAlg.h"

namespace Muon
{
  TgcPrepDataReplicationAlg::TgcPrepDataReplicationAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  {
  }

  StatusCode TgcPrepDataReplicationAlg::initialize() {
    ATH_MSG_DEBUG("initialize()");

    // Retrieve TgcPrepDataReplicationTool
    StatusCode sc = m_tool.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("TgcPrepDataReplicationTool could not be retrieved");
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcPrepDataReplicationAlg::finalize() {
    ATH_MSG_DEBUG("finalize()");
    return StatusCode::SUCCESS;
  }
  
  StatusCode TgcPrepDataReplicationAlg::execute() {
    ATH_MSG_DEBUG("execute()");
    
    StatusCode sc = m_tool->replicate();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("TgcPrepDataReplicationAlg::execute m_tool->replicate() returns StatusCode::FAILURE");
      return sc;
    }

    return StatusCode::SUCCESS;
  }
} // end of namespace Muon 
