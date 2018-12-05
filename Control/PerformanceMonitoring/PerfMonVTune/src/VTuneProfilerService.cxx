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

  // Set up listening to the incidents
  CHECK( m_incidentSvc.retrieve() );
  m_incidentSvc->addListener( this, BEGINEVENT_INCIDENT_NAME );
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
     if( m_pauseEvent == m_processedEvents ) {
        if( pauseProfiling().isFailure() &&
            m_runner ) {
           REPORT_MESSAGE( MSG::ERROR )
              << "Could not pause the profiling";
        }
     }
     ++m_processedEvents;
     return;
  }

  // Complain if we received an incident that we didn't expect:
  ATH_MSG_WARNING( "Wrong incident type received: " << inc.type() );
  return;

}
