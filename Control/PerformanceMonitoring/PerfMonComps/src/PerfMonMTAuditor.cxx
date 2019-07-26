/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/INamedInterface.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonMTAuditor.h"
#include "GaudiKernel/MsgStream.h"

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
  MsgStream msg( msgSvc(), name() );
  //msg << MSG::INFO << "before Standard: " << etype << " - " << component->name() << endmsg;
  return m_perfMonMTSvc->startAud( toStr(etype) , component->name() );
}

void PerfMonMTAuditor::before( StandardEventType, const std::string& ) {
  return;
}

void PerfMonMTAuditor::before( CustomEventTypeRef, INamedInterface* ) {
  // don't poll for custom event type for now...
  return;
}

void PerfMonMTAuditor::before( CustomEventTypeRef etype, const std::string& component ) {
  MsgStream msg( msgSvc(), name() );
  //msg << MSG::INFO << "before Custom: " << etype << " - " << component << endmsg;
  return m_perfMonMTSvc->startAud( etype , component );
  // don't poll for custom event type for now...
  //return;
}

void PerfMonMTAuditor::after( StandardEventType etype, INamedInterface* component, const StatusCode& ) {
  MsgStream msg( msgSvc(), name() );
  //msg << MSG::INFO << "after Standard : " << etype << " - " << component->name() << endmsg;
  return m_perfMonMTSvc->stopAud( toStr(etype), component->name() );
}

void PerfMonMTAuditor::after( StandardEventType, const std::string&, const StatusCode& ) {
  return;
}

void PerfMonMTAuditor::after( CustomEventTypeRef, INamedInterface*, const StatusCode& ) {
  // don't poll for custom event type for now...
  return;
}

void PerfMonMTAuditor::after( CustomEventTypeRef etype, const std::string& component , const StatusCode& sc ) {
  MsgStream msg( msgSvc(), name() );
  //msg << MSG::INFO << "after Custom: " << etype << " - " << component << endmsg;

  return m_perfMonMTSvc->stopAud( etype, component );
  // don't poll for custom event type for now...
  //return;
}
