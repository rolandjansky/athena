/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WebBunchCrossingTool.cxx 457114 2011-09-05 09:35:49Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Local include(s):
#include "WebBunchCrossingTool.h"
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   WebBunchCrossingTool::WebBunchCrossingTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_incidentSvc( "IncidentSvc", name ) {

      // Declare the interfaces provided by the tool:
      declareInterface< IBunchCrossingTool >( this );
      declareInterface< IIncidentListener >( this );

      // Declare the properties of the tool:
      declareProperty( "MaxBunchSpacing", m_maxBunchSpacing = 500,
                       "Maximum bunch spacing in the trains in nanoseconds" );
      declareProperty( "FrontLength", m_frontLength = 300,
                       "Length of the front part of a train in nanoseconds" );
      declareProperty( "TailLength", m_tailLength = 300,
                       "Length of the tail part of a train in nanoseconds" );
   }

   StatusCode WebBunchCrossingTool::initialize() {

      // Configure the underlying logger:
      MsgWriter::instance()->setMinType( msg().level() );
      MsgWriter::instance()->setSource( name() );

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing WebBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );

      // Configure the base class:
      setMaxBunchSpacing( m_maxBunchSpacing );
      setFrontLength( m_frontLength );
      setTailLength( m_tailLength );

      // Retrieve the incident service, and register the tool for the trigger
      // configuration incidents:
      CHECK( m_incidentSvc.retrieve() );
      m_incidentSvc->addListener( this, IncidentType::BeginEvent );

      // Reset the bookkeeping variables:
      m_lastRun = 0; m_lastLB = 0;

      return StatusCode::SUCCESS;
   }

   StatusCode WebBunchCrossingTool::finalize() {

      ATH_MSG_INFO( "Finalizing WebBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   void WebBunchCrossingTool::handle( const Incident& inc ) {

      if( inc.type() == IncidentType::BeginEvent ) {
         const EventIncident* evinc = dynamic_cast< const EventIncident* >( &inc );
         if( ! evinc ) {
            REPORT_ERROR( StatusCode::FAILURE ) << "BeginEvent incident received without "
                                                << "an EventIncident argument";
            return;
         }
         if( load( evinc->eventInfo().event_ID() ).isFailure() ) {
            REPORT_ERROR( StatusCode::FAILURE ) << "Failed to load the correct configuration";
            REPORT_ERROR( StatusCode::FAILURE ) << "Results won't be reliable!";
         }
         return;
      }

      REPORT_MESSAGE( MSG::WARNING ) << "Received a not requested incident!";
      return;
   }

   void WebBunchCrossingTool::setMaxBunchSpacing( int spacing ) {

      BunchCrossingToolBase::setMaxBunchSpacing( spacing );
      return;
   }

   int WebBunchCrossingTool::maxBunchSpacing() const {

      return BunchCrossingToolBase::maxBunchSpacing();
   }

   void WebBunchCrossingTool::setFrontLength( int length ) {

      BunchCrossingToolBase::setFrontLength( length );
      return;
   }

   int WebBunchCrossingTool::frontLength() const {

      return BunchCrossingToolBase::frontLength();
   }

   void WebBunchCrossingTool::setTailLength( int length ) {

      BunchCrossingToolBase::setTailLength( length );
      return;
   }

   int WebBunchCrossingTool::tailLength() const {

      return BunchCrossingToolBase::tailLength();
   }

   StatusCode WebBunchCrossingTool::load( const EventID* event ) {

      if( ! event ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Received a null pointer for the event ID";
         return StatusCode::FAILURE;
      }

      if( ( m_lastRun != event->run_number() ) || ( m_lastLB != event->lumi_block() ) ) {
         if( ! loadConfig( event->run_number(), event->lumi_block() ) ) {
            REPORT_ERROR( StatusCode::FAILURE ) << "Couldn't load configuration for "
                                                << "run #" << event->run_number()
                                                << " LB #" << event->lumi_block();
            return StatusCode::FAILURE;
         }

         m_lastRun = event->run_number();
         m_lastLB  = event->lumi_block();
      }

      return StatusCode::SUCCESS;
   }

} // namespace Trig
