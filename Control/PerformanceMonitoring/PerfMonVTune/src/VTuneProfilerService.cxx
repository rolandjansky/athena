/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "VTuneProfilerService.h"

//
// Convenience variable(s):
//
static const std::string BEGINEVENT_INCIDENT_NAME = "BeginEvent";
static const std::string ENDEVENT_INCIDENT_NAME   = "EndEvent";
static const std::string ENDEVTLOOP_INCIDENT_NAME = "EndEvtLoop";

/**
 * Constructor
 */
VTuneProfilerService::VTuneProfilerService( const std::string& name, ISvcLocator* svcloc )
  : AthService( name, svcloc ),
    m_incidentSvc( "IncidentSvc", name ),
     m_processedEvents( 0 )  {

      declareProperty( "ResumeEvent", m_resumeEvent = 0,
                       "Event in which to resume the profiling." );
      declareProperty( "PauseEvent", m_pauseEvent = -1,
                       "Event in which to pause the profiling. Negative number "
                       "profiles the entire event-loop." );
      declareProperty( "ProfiledAlgs", m_algs,
                       "List of profiled algorithms." );

}

/**
 * QueryInterface
 */
StatusCode VTuneProfilerService::queryInterface( const InterfaceID& riid,
                                                 void** ppvIF ) {

   if( ! ppvIF ) {
      return StatusCode::FAILURE;
   }

   if( riid == IVTuneProfilerSvc::interfaceID() ) {
      *ppvIF = static_cast< IVTuneProfilerSvc* >( this );
      return StatusCode::SUCCESS;
   }

   return AthService::queryInterface( riid, ppvIF );

}


/**
 * Initalize the service
 */
StatusCode VTuneProfilerService::initialize() {

  // Print information
  ATH_MSG_INFO( "Initializing VTuneProfilerService" );

  // Use the auditor if a list of algorithms is provided
  if(!m_algs.empty()) {

    // Resume/Pause event don't work in conjuction w/ a list of algorithms
    ATH_MSG_INFO("ResumeEvent/PauseEvent don't work in conjuction with ProfiledAlgs. " <<
                 "Execute methods of all provided algorithms will be sampled in all events." );

    // Create the auditor
    makeAuditor("VTuneAuditor",auditorSvc()).ignore();

    // Check if the auditor is correctly created
    if ((auditorSvc()==0) || auditorSvc()->getAuditor("VTuneAuditor")==0) {
      ATH_MSG_ERROR ("Cannot find [VTuneAuditor]. ");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_VERBOSE ("Found [VTuneAuditor].");
    }

  }

  // Set up listening to the incidents
  CHECK( m_incidentSvc.retrieve() );
  m_incidentSvc->addListener( this, BEGINEVENT_INCIDENT_NAME );
  m_incidentSvc->addListener( this, ENDEVENT_INCIDENT_NAME   );
  m_incidentSvc->addListener( this, ENDEVTLOOP_INCIDENT_NAME );

  // Reset the event counter:
  m_processedEvents = 0;

  return StatusCode::SUCCESS;

}

/**
 * Resume the profiling
 */
StatusCode VTuneProfilerService::resumeProfiling() {

  // Check whether the profiling is already running:
  if( m_runner ) {
    ATH_MSG_INFO( "VTune profiling already running!" );
    return StatusCode::SUCCESS;
  }

  // Print information
  ATH_MSG_INFO( "Starting VTune profiling." );

  // Resume VTune
  m_runner = std::make_unique< VTuneProfileRunner >();

  return StatusCode::SUCCESS;

}

/**
 * Pause the profiling
 */
StatusCode VTuneProfilerService::pauseProfiling() {

  // Check whether the profiling is already running:
  if( !m_runner ) {
    ATH_MSG_INFO( "VTune profiling is not running!" );
    return StatusCode::SUCCESS;
  }

  // Print information
  ATH_MSG_INFO( "Stopping VTune profiling." );

  // Pause VTune
  m_runner.reset();

  return StatusCode::SUCCESS;

}

/**
 * Is the profiling running?
 */
bool VTuneProfilerService::isProfilingRunning() const {

  return (m_runner!=nullptr);

}

/**
 * Handle when to resume/pause the profiling
 */
void VTuneProfilerService::handle( const Incident& inc ) {

  // Use incidents only if there is no auditor
  // Next step is to get rid of the incidents altogether
  if ( !m_algs.empty() ) return;

  //
  // Pause the profiling after the last event
  //
  if( inc.type() == ENDEVTLOOP_INCIDENT_NAME ) {
    if( m_runner ) {
      if( pauseProfiling().isFailure() ) {
        REPORT_MESSAGE( MSG::ERROR )
           << "Could not pause the profiling";
      }
    }
    return;
  }

  //
  // Resume/Pause the profiling on the right event
  //
  if( inc.type() == BEGINEVENT_INCIDENT_NAME ) {
     if( m_resumeEvent == m_processedEvents &&
         !m_runner ) {
        if( resumeProfiling().isFailure() ) {
           REPORT_MESSAGE( MSG::ERROR )
              << "Could not resume the profiling";
        }
     }
     ++m_processedEvents;
     return;
  }
  else if( inc.type() == ENDEVENT_INCIDENT_NAME ) {
     if( m_pauseEvent == m_processedEvents ) {
        if( pauseProfiling().isFailure() &&
            m_runner ) {
           REPORT_MESSAGE( MSG::ERROR )
              << "Could not pause the profiling";
        }
     }
     return;
  }

  // Complain if we received an incident that we didn't expect:
  ATH_MSG_WARNING( "Wrong incident type received: " << inc.type() );
  return;

}

/**
 * Create the auditor here ala PerfMonComps/PerfMonUtils
 */
StatusCode VTuneProfilerService::makeAuditor (const std::string& audName,
                                              IAuditorSvc* audSvc)
{
  if ( 0 == audSvc ) {
    ATH_MSG_ERROR ("Null pointer to IAuditorSvc !!");
    return StatusCode::FAILURE;
  }

  if ( 0 != audSvc->getAuditor( audName ) ) {
    ATH_MSG_VERBOSE ("AuditorSvc already knows about ["
         << audName << "]... good.");
    return StatusCode::SUCCESS;
  }

  const std::string propName = "Auditors";
  IProperty * audSvcProp = dynamic_cast<IProperty*>(audSvc);

  if ( 0 == audSvcProp ) {
    ATH_MSG_ERROR ("Could not dyn-cast IAuditorSvc to an IProperty !!");
    return StatusCode::FAILURE;
  }

  StringArrayProperty audNames;
  audNames.assign( audSvcProp->getProperty(propName) );
  std::vector<std::string> updatedNames( audNames.value() );
  updatedNames.push_back( audName );
  audNames.set( updatedNames );
  audNames.setName( propName );
  if ( !audSvcProp->setProperty( audNames ).isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not add [" << audName
       << "] to the list of auditors of [AuditorSvc] !!"
  << endmsg
       << audSvcProp->getProperty(propName));
    return StatusCode::FAILURE;
  }

  // make sure the auditor has been created...
  if ( 0 == audSvc->getAuditor( audName ) ) {
    ATH_MSG_ERROR ("Failed to make AuditorSvc instantiating ["
       << audName << "] !!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE ("[" << audName << "] successfully created.");
  return StatusCode::SUCCESS;
}
