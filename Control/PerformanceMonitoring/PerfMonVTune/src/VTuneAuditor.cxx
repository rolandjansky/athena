/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s)
#include "GaudiKernel/INamedInterface.h"

// Local include(s)
#include "VTuneAuditor.h"

/*
 * Constructor
 */
VTuneAuditor::VTuneAuditor( const std::string& name,
                            ISvcLocator* pSvcLocator ):
  Auditor( name, pSvcLocator ),
  m_vtuneProfilerSvc( "VTuneProfilerService", name )
{

    declareProperty( "ProfiledAlgs", m_algs,
                     "List of profiled algorithms." );

}

/*
 * Initialize the Auditor
 */
StatusCode VTuneAuditor::initialize()
{

  if( !m_vtuneProfilerSvc.retrieve().isSuccess() ) {
    return StatusCode::FAILURE;
  }

  // Copy algorithms from the service
  const IProperty* vtuneProfSvcProp = dynamic_cast<const IProperty*>(&(*m_vtuneProfilerSvc));
  if ( !vtuneProfSvcProp ) {
    msgStream() << MSG::ERROR
                << "Could not retrieve IProperty interface to VTuneProfilerService."
                << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<std::string> props2Copy = { "ProfiledAlgs" };
  for(auto& prop : props2Copy) {
    if ( !setProperty(vtuneProfSvcProp->getProperty(prop)) ) {
      msgStream() << MSG::ERROR
                  << "Cannot set " << prop << " property."
                  << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;

}

/*
 * Implementation of base class methods
 */
void VTuneAuditor::before( StandardEventType etype, INamedInterface* component ) {

  for( auto& val : m_algs ) {
    if ( etype == StandardEventType::Execute && component->name() == val ) {
      if ( m_vtuneProfilerSvc->resumeProfiling().isFailure() ) {
        msgStream() << MSG::ERROR
                    << "Could not resume the profiling from the auditor"
                    << endmsg;
      }
      break;
    }
  }
  return;

}

void VTuneAuditor::after( StandardEventType etype, INamedInterface* component, const StatusCode& ) {

  for( auto& val : m_algs ) {
    if ( etype == StandardEventType::Execute && component->name() == val ) {
      if ( m_vtuneProfilerSvc->pauseProfiling().isFailure() ) {
        msgStream() << MSG::ERROR
                    << "Could not pause the profiling from the auditor"
                    << endmsg;
      }
      break;
    }
  }
  return;

}
