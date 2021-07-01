/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ThreadLocalContext.h"

#include "TrigCostMonitor/TrigCostAuditor.h"
#include "TrigCostSvc.h"


/////////////////////////////////////////////////////////////////////////////

TrigCostAuditor::TrigCostAuditor(const std::string& name, ISvcLocator* pSvcLocator) :
Auditor(name, pSvcLocator),
AthMessaging(msgSvc(), name)
{
  ATH_MSG_DEBUG("TrigCostAuditor constructor");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostAuditor::initialize(){
  ATH_MSG_DEBUG("TrigCostAuditor initialize()");
  ATH_CHECK( m_trigCostSvcHandle.retrieve() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostAuditor::finalize() {
  ATH_MSG_DEBUG("TrigCostAuditor finalize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostAuditor::before(StandardEventType evt, const std::string& caller) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_DEBUG("Before Execute: " << caller);
  callService(caller, ITrigCostSvc::AuditType::Before);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostAuditor::after(StandardEventType evt, const std::string& caller, const StatusCode& sc) {
  if (evt != StandardEventType::Execute) return; // I only care for execution time
  ATH_MSG_DEBUG("After Execute: " << caller << " " << sc);
  callService(caller, ITrigCostSvc::AuditType::After);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TrigCostAuditor::callService(const std::string& caller, ITrigCostSvc::AuditType type) {
  // Note: Using ThreadLocalContext.h, better would be for the auditor to be told the Context
  if (m_trigCostSvcHandle->processAlg(Gaudi::Hive::currentContext(), caller, type).isFailure()) {
    ATH_MSG_FATAL("Error in TrigCostSvc called by TrigCostAuditor, auditing algorithm: " << caller);
    throw std::runtime_error("TrigCostAuditor exception");
  }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


