/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// GPT include(s):
#ifdef ATLAS_CMAKE
#include <google/profiler.h>
#else
#include <gperftools/profiler.h>
#endif

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "ProfilerService.h"

namespace GPT {

   //
   // Convenience variable(s):
   //
   static const std::string BEGINEVENT_INCIDENT_NAME = "BeginEvent";
   static const std::string ENDEVTLOOP_INCIDENT_NAME = "EndEvtLoop";

   ProfilerService::ProfilerService( const std::string& name, ISvcLocator* svcloc )
      : AthService( name, svcloc ),
        m_incidentSvc( "IncidentSvc", name ),
        m_running( false ), m_fileName( "" ), m_processedEvents( 0 ) {

      declareProperty( "ControlledProfiling", m_controlledProfiling = false,
                       "Will the service be controlled by another component?" );
      declareProperty( "InitEvent", m_initEvent = -1,
                       "Event in which to start the profiling. Negative number "
                       "profiles the entire job." );
      declareProperty( "ProfileFileName", m_profFileName = "gpt.profile",
                       "Profile file name in con-controlled mode." );
   }

   ProfilerService::~ProfilerService() {

      // Stop the CPU profiling at this point if it's still running:
      if( m_running ) {
         stop().ignore();
      }
   }

   StatusCode ProfilerService::queryInterface( const InterfaceID& riid,
                                               void** ppvIF ) {

      if( ! ppvIF ) {
         return StatusCode::FAILURE;
      }

      if( riid == IProfilerSvc::interfaceID() ) {
         *ppvIF = static_cast< IProfilerSvc* >( this );
         return StatusCode::SUCCESS;
      }

      return AthService::queryInterface( riid, ppvIF );
   }

   StatusCode ProfilerService::initialize() {

      ATH_MSG_INFO( "Initializing GPT::ProfilerService" );

      // Don't do anything fancy if the profiling is under user control, just
      // remind the user of it:
      if( m_controlledProfiling ) {
         ATH_MSG_INFO( "  Profiling is under user control" );
         return StatusCode::SUCCESS;
      }

      // Start the profiling at this point if full-job profiling is requested:
      if( m_initEvent < 0 ) {

         CHECK( startCPUProfiling( m_profFileName ) );

      } else {

         // Set up listening to the incidents only if we are profiling the
         // event loop with this service:
         CHECK( m_incidentSvc.retrieve() );
         m_incidentSvc->addListener( this, BEGINEVENT_INCIDENT_NAME );
         m_incidentSvc->addListener( this, ENDEVTLOOP_INCIDENT_NAME );

      }

      // Reset the event counter:
      m_processedEvents = 0;

      return StatusCode::SUCCESS;
   }

   StatusCode ProfilerService::finalize() {

      ATH_MSG_INFO( "Finalizing GPT::ProfilerService" );

      // The profiling should only be stopped in the destructor even if we
      // are not under user control...

      return StatusCode::SUCCESS;
   }

   /**
    * This function can be used to start the profiling, collecting the
    * data into a file with the specified name.
    *
    * @param filename Name of the GPT profile file
    * @returns <code>StatusCode::SUCCESS</code> if the operation was
    *          successful, something else otherwise
    */
   StatusCode ProfilerService::startCPUProfiling( const std::string& filename ) {

      // Check whether the CPU profiling is already running:
      if( m_running ) {
         CHECK( stopCPUProfiling() );
      }

      // Tell the user what's happening:
      ATH_MSG_INFO( "Starting CPU profiling using output file: " << filename );

      // Remember the profile file name:
      m_fileName = filename;

      // Start the Google profiler:
      ProfilerStart( m_fileName.c_str() );

      // Remember that we are in a running state:
      m_running = true;

      return StatusCode::SUCCESS;
   }

   /**
    * This function should be called to stop the CPU profiling.
    *
    * @returns <code>StatusCode::SUCCESS</code> if the operation was
    *          successful, something else otherwise
    */
   StatusCode ProfilerService::stopCPUProfiling() {

      // This should not be called when we are not running:
      if( ! m_running ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "CPU profiling not running at the time of call";
         return StatusCode::RECOVERABLE;
      }

      // Stop the profiler:
      ProfilerStop();

      // Tell the user what happened:
      ATH_MSG_INFO( "Stopped CPU profiling using output file: " << m_fileName );

      // Remember that we are not running anymore:
      m_running = false;

      return StatusCode::SUCCESS;
   }

   /**
    * This function can be used to ask the service whether CPU profiling
    * is running at the moment.
    *
    * @returns <code>true</code> if the profiling is running at the moment,
    *          <code>false</code> otherwise
    */
   bool ProfilerService::isCPUProfilingRunning() const {

      return m_running;
   }

   /**
    * The event loop is profiled by listening to incidents coming from
    * the event loop manager. This function is called every time an
    * "interesting" incident happens.
    *
    * @param inc The incident that happened
    */
   void ProfilerService::handle( const Incident& inc ) {

      //
      // Stop the profiling after the last event, if we are only profiling
      // the event loop:
      //
      if( ( inc.type() == ENDEVTLOOP_INCIDENT_NAME ) &&
          ( ! m_controlledProfiling ) &&
          ( m_initEvent >= 0 ) ) {
         if( stopCPUProfiling().isFailure() ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Could not stop the CPU profiling";
         }
         return;
      }

      //
      // Start the profiling if we only profile the event loop,
      // and we're in the correct event:
      //
      if( ( inc.type() == BEGINEVENT_INCIDENT_NAME ) &&
          ( ! m_controlledProfiling ) ) {
         if( m_initEvent == m_processedEvents ) {
            if( startCPUProfiling( m_profFileName ).isFailure() ) {
               REPORT_MESSAGE( MSG::ERROR )
                  << "Could not start the CPU profiling";
            }
         }
         ++m_processedEvents;
         return;
      }

      // Complain if we received an incident that we didn't expect:
      ATH_MSG_WARNING( "Wrong incident type received: " << inc.type() );
      return;
   }

} // namespace GPT
