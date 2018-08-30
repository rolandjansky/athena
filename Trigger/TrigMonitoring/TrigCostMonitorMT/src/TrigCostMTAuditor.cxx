/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCostMonitorMT/TrigCostMTAuditor.h"
#include "TrigCostMonitorMT/TrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////

TrigCostMTAuditor::TrigCostMTAuditor(const std::string& name, ISvcLocator* pSvcLocator) :
Auditor(name, pSvcLocator),
AthMessaging(msgSvc(), name)
{
  ATH_MSG_INFO("default constructor");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTAuditor::initialize(){
  ATH_MSG_INFO("initialize()");
  ATH_CHECK( m_trigCostSvcHandle.retrieve() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTAuditor::finalize() {
  ATH_MSG_INFO("finalize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::before(StandardEventType evt, const std::string& caller) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_INFO("before (var standard) " << evt << " " << caller);
  if (m_trigCostSvcHandle->beginAlg(caller).isFailure()) {
    ATH_MSG_FATAL("Unable to access TrigCostMTSvc to record start time of " << caller);
    throw std::runtime_error("TrigCostMTAuditor exception");
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::before(CustomEventTypeRef evt, const std::string& caller) {
  // Not monitored
  ATH_MSG_DEBUG("before (var custom) " << evt << " " << caller);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::after(StandardEventType evt, const std::string& caller, const StatusCode& sc) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_INFO("after (var standard) " << evt << " " << caller << " " << sc);
  if (m_trigCostSvcHandle->endAlg(caller).isFailure()) {
    ATH_MSG_FATAL("Unable to access TrigCostMTSvc to record end time of " << caller);
    throw std::runtime_error("TrigCostMTAuditor exception");
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::after(CustomEventTypeRef evt, const std::string& caller, const StatusCode& sc) {
  // Not monitored
  ATH_MSG_DEBUG("after (var custom) " << evt << " " << caller << " " << sc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


