/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataReplicationAlg.h"

namespace Muon
{
  TgcPrepDataReplicationAlg::TgcPrepDataReplicationAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
  {
  }

  StatusCode TgcPrepDataReplicationAlg::initialize() {
    ATH_MSG_DEBUG("initialize()");

    // Retrieve TgcPrepDataReplicationTool
    ATH_CHECK( m_tool.retrieve());
    return StatusCode::SUCCESS;
  }
 
  StatusCode TgcPrepDataReplicationAlg::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG("execute()");    
    ATH_CHECK(m_tool->replicate(ctx));
    return StatusCode::SUCCESS;
  }
} // end of namespace Muon 
