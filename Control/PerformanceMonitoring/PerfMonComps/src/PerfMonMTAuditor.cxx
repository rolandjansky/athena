/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/MsgStream.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonMTAuditor.h"


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

void PerfMonMTAuditor::before( StandardEventType etype, const std::string& compName ) {
  return m_perfMonMTSvc->startAud( toStr(etype) , compName );
}

void PerfMonMTAuditor::before( CustomEventTypeRef etype, INamedInterface* component ) {
  return m_perfMonMTSvc->startAud( etype , component->name() );
}

void PerfMonMTAuditor::before( CustomEventTypeRef etype, const std::string& compName ) {
  return m_perfMonMTSvc->startAud( etype , compName );
}

void PerfMonMTAuditor::after( StandardEventType etype, INamedInterface* component, const StatusCode& ) {
  return m_perfMonMTSvc->stopAud( toStr(etype), component->name() );
}

void PerfMonMTAuditor::after( StandardEventType etype, const std::string& compName, const StatusCode& ) {
  return m_perfMonMTSvc->stopAud( toStr(etype), compName );
}

void PerfMonMTAuditor::after( CustomEventTypeRef etype, INamedInterface* component, const StatusCode& ) {
  return m_perfMonMTSvc->stopAud( etype, component->name() );
}

void PerfMonMTAuditor::after( CustomEventTypeRef etype, const std::string& compName , const StatusCode& ) {
  return m_perfMonMTSvc->stopAud( etype, compName );
}
