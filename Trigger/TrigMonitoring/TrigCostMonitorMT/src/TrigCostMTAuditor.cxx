/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ThreadLocalContext.h"

#include "TrigCostMonitorMT/TrigCostMTAuditor.h"
#include "TrigCostMTSvc.h"


/////////////////////////////////////////////////////////////////////////////

TrigCostMTAuditor::TrigCostMTAuditor(const std::string& name, ISvcLocator* pSvcLocator) :
Auditor(name, pSvcLocator),
AthMessaging(msgSvc(), name)
{
  ATH_MSG_DEBUG("TrigCostMTAuditor constructor");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTAuditor::initialize(){
  ATH_MSG_DEBUG("TrigCostMTAuditor initialize()");
  ATH_CHECK( m_trigCostSvcHandle.retrieve() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostMTAuditor::finalize() {
  ATH_MSG_DEBUG("TrigCostMTAuditor finalize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::before(StandardEventType evt, const std::string& caller) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_DEBUG("Before Execute: " << caller);
  callService(caller, ITrigCostMTSvc::AuditType::Before);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::after(StandardEventType evt, const std::string& caller, const StatusCode& sc) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_DEBUG("After Execute: " << caller << " " << sc);
  callService(caller, ITrigCostMTSvc::AuditType::After);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostMTAuditor::callService(const std::string& caller, ITrigCostMTSvc::AuditType type) {
  // Note: Using ThreadLocalContext.h, better would be for the auditor to be told the Context
  if (m_trigCostSvcHandle->processAlg(Gaudi::Hive::currentContext(), caller, type).isFailure()) {
    ATH_MSG_FATAL("Error in TrigCostMTSvc called by TrigCostMTAuditor, auditing algorithm: " << caller);
    throw std::runtime_error("TrigCostMTAuditor exception");
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


