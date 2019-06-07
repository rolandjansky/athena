/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/INamedInterface.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTAuditor.h"

/*
 * Constructor
 */
PerfMonMTAuditor::PerfMonMTAuditor( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  Auditor ( name, pSvcLocator  ),
  m_perfMonMTSvc ( "PerfMonMTSvc", name )
{

}

/*
 * Initialize the Auditor
 */
StatusCode PerfMonMTAuditor::initialize()
{

  if ( !m_perfMonMTSvc.retrieve().isSuccess() ) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/*
 * Implementation of base class methods
 */
void PerfMonMTAuditor::before( StandardEventType etype, INamedInterface* component ) {
  return m_perfMonMTSvc->startAud( toStr(etype) , component->name() );
}

void PerfMonMTAuditor::before( StandardEventType, const std::string& ) {
  return;
}

void PerfMonMTAuditor::before( CustomEventTypeRef, INamedInterface* ) {
  // don't poll for custom event type for now...
  return;
}

void PerfMonMTAuditor::before( CustomEventTypeRef, const std::string& ) {
  // don't poll for custom event type for now...
  return;
}

void PerfMonMTAuditor::after( StandardEventType etype, INamedInterface* component, const StatusCode& ) {
  return m_perfMonMTSvc->stopAud( toStr(etype), component->name() );
}

void PerfMonMTAuditor::after( StandardEventType, const std::string&, const StatusCode& ) {
  return;
}

void PerfMonMTAuditor::after( CustomEventTypeRef, INamedInterface*, const StatusCode& ) {
  // don't poll for custom event type for now...
  return;
}

void PerfMonMTAuditor::after( CustomEventTypeRef, const std::string&, const StatusCode& ) {
  // don't poll for custom event type for now...
  return;
}
