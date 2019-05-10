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
  //ATH_MSG_INFO("Initialize");

  if ( !m_perfMonMTSvc.retrieve().isSuccess() ) {
    //ATH_MSG_ERROR("Could not retrieve PerfMonMTSvc!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/*
 * Implementation of base class methods 
 */
void PerfMonMTAuditor::before( StandardEventType, INamedInterface* ) { return; }
void PerfMonMTAuditor::before( StandardEventType, const std::string& ) { return; }
void PerfMonMTAuditor::before( CustomEventTypeRef, INamedInterface* ) { return; }
void PerfMonMTAuditor::before( CustomEventTypeRef, const std::string& ) { return; }
void PerfMonMTAuditor::after( StandardEventType, INamedInterface*, const StatusCode& ) { return; }
void PerfMonMTAuditor::after( StandardEventType, const std::string&, const StatusCode& ) { return; }
void PerfMonMTAuditor::after( CustomEventTypeRef, INamedInterface*, const StatusCode& ) { return; }
void PerfMonMTAuditor::after( CustomEventTypeRef, const std::string&, const StatusCode& ) { return; }
