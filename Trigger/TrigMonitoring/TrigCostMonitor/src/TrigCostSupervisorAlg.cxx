/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCostSupervisorAlg.h"


TrigCostSupervisorAlg::TrigCostSupervisorAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode TrigCostSupervisorAlg::initialize() {
    ATH_MSG_DEBUG("TrigCostSupervisorAlg start");
    ATH_CHECK(m_trigCostSvcHandle.retrieve());
    return StatusCode::SUCCESS;
}


StatusCode TrigCostSupervisorAlg::execute (const EventContext& ctx) const {
    ATH_MSG_DEBUG("TrigCostSupervisorAlg execute");
    ATH_CHECK(m_trigCostSvcHandle->startEvent(ctx, true));
    return StatusCode::SUCCESS;
}