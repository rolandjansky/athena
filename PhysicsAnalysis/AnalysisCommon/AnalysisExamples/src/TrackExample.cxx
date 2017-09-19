/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackExample.cxx 637956 2015-01-07 05:11:27Z ssnyder $

// ROOT include(s):
#include <TH1.h>

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

/// Local include(s):
#include "TrackExample.h"

TrackExample::TrackExample( const std::string& name,
                            ISvcLocator* pSvcLocator)
   : AthHistogramAlgorithm( name, pSvcLocator ), 
     m_analysisTools( "AnalysisTools", this ),  
     m_isolationTool( "xAOD::TrackIsolationTool/TrackIsolationTool", this ),
     m_histNumberOfTrackParticles(0),
     m_histQoverP(0),
     m_histP(0),
     m_histQ(0),
     m_histEta(0),
     m_histPhi(0),
     m_histIsolationP(0),
     m_histOriginType(0),
     m_histVertexX(0),
     m_histVertexY(0),
     m_histVertexZ(0),
     m_histd0wrtPrimVtx(0),
     m_histz0wrtPrimVtx(0)
{

   /// switches to control the analysis through job options
   declareProperty( "AnalysisTools", m_analysisTools );
   declareProperty( "TrackIsolationTool", m_isolationTool );
   declareProperty( "TrackParticleContainerName",
                    m_trackParticleContainerName = "InDetTrackParticles" );
   declareProperty( "EventWeight", m_eventWeight = 1.0 );
   declareProperty( "TrackMomentumCut", m_trackPCut = 500.0 );
   declareProperty( "TrackIsolationCone",
                    m_isolationCone = xAOD::Iso::ptcone40 );
}

StatusCode TrackExample::initialize() {

   ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );

   // Retrieve the needed services:
   ATH_CHECK( m_analysisTools.retrieve() );
   ATH_CHECK( m_isolationTool.retrieve() );

   //
   // Book the histograms:
   //
   m_histNumberOfTrackParticles =
      bookGetPointer( TH1F( "nTrackParticles", "Number of TrackParticles",
                            100, 0.0, 100.0 ), "TrackParticles" );
   m_histQoverP =
      bookGetPointer( TH1F( "QoverP", "QoverP", 100, -1.0e-2, 1.0e-2 ),
                      "TrackParticles" );
   m_histP =
      bookGetPointer( TH1F( "TrackP", "TrackP", 100, 0.0, 500000.0 ),
                      "TrackParticles" );
   m_histQ =
      bookGetPointer( TH1F( "TrackQ", "TrackQ", 100, -2.0, 2.0 ),
                      "TrackParticles" );
   m_histEta =
      bookGetPointer( TH1F( "TrackEta", "TrackEta", 100, -3.2, 3.2 ),
                      "TrackParticles" );
   m_histPhi =
      bookGetPointer( TH1F( "TrackPhi", "TrackPhi", 100, -3.2, 3.2 ),
                      "TrackParticles" );

   m_histVertexX =
      bookGetPointer( TH1F( "VertexX", "VertexX", 100, -0.5, 0.5 ),
                      "TrackParticles" );
   m_histVertexY =
      bookGetPointer( TH1F( "VertexY", "VertexY", 100, -0.5, 0.5 ),
                      "TrackParticles" );
   m_histVertexZ =
      bookGetPointer( TH1F( "VertexZ","VertexZ", 100, -50.0, 50.0 ),
                      "TrackParticles" );

   m_histd0wrtPrimVtx =
      bookGetPointer( TH1F( "d0wrtPrimVtx", "d0wrtPrimVtx", 100,
                            -20.0, 20.0 ), "TrackParticles" );
   m_histz0wrtPrimVtx =
      bookGetPointer( TH1F( "z0wrtPrimVtx", "z0wrtPrimVtx", 100,
                            -20.0, 20.0 ), "TrackParticles" );

   m_histOriginType =
      bookGetPointer( TH1F( "OriginType", "OriginType", 10, 0, 10 ),
                      "TrackParticles" );
   m_histIsolationP =
      bookGetPointer( TH1F( "IsolationP", "IsolationP", 100, 0, 100000.0 ),
                      "TrackParticles" );

   // Return gracefully:
   return StatusCode::SUCCESS;
}		 

StatusCode TrackExample::execute() {

   ATH_MSG_DEBUG( "Executing..." );

   // Retrieve the track particles:
   const xAOD::TrackParticleContainer* trackTES = 0;
   if( evtStore()->retrieve( trackTES,
                             m_trackParticleContainerName ).isFailure() ) {
      ATH_MSG_WARNING( "No TrackParticle container found in TDS" );
      return StatusCode::SUCCESS;
   }
   ATH_MSG_DEBUG( "TrackParticleContainer successfully retrieved" );

   /// number of TrackParticles in the container
   m_histNumberOfTrackParticles->Fill( 1.0 * trackTES->size(), m_eventWeight );

   /// iterators over the container 
   xAOD::TrackParticleContainer::const_iterator trackItr  = trackTES->begin();
   xAOD::TrackParticleContainer::const_iterator trackItrE = trackTES->end();

   // Loop over the container:
   for( ; trackItr != trackItrE; ++trackItr ) {

      /// this TrackParticle
      const xAOD::TrackParticle* trackParticle = ( *trackItr );

      /// track momemtum and charge sign
      m_histP->Fill( trackParticle->p4().P(), m_eventWeight );
      m_histEta->Fill( trackParticle->eta(), m_eventWeight );
      m_histPhi->Fill( trackParticle->phi(), m_eventWeight );
      m_histQ->Fill( trackParticle->charge(), m_eventWeight );
      m_histQoverP->Fill( trackParticle->qOverP(), m_eventWeight);

      /// track vertex position
      m_histVertexX->Fill( trackParticle->vx(), m_eventWeight );
      m_histVertexY->Fill( trackParticle->vy(), m_eventWeight );
      m_histVertexZ->Fill( trackParticle->vz(), m_eventWeight );

      m_histOriginType->Fill( 1.0 * trackParticle->particleHypothesis(),
                              m_eventWeight );

      /// do track isolation - use the track isolation tools
      const std::vector< xAOD::Iso::IsolationType > isoType = {
         static_cast< xAOD::Iso::IsolationType >( m_isolationCone )
      };
      xAOD::TrackCorrection corrlist;
      corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr));
      xAOD::TrackIsolation isol;
      if( ! m_isolationTool->trackIsolation( isol, *trackParticle, isoType, corrlist ) ) {
         ATH_MSG_WARNING( "Unable to execute track isolation calculation" );
         isol.ptcones.push_back( 0.0 );
      }

      m_histIsolationP->Fill( isol.ptcones[ 0 ], m_eventWeight );
      ATH_MSG_DEBUG( "Track Isolation Momentum = " << isol.ptcones[ 0 ]
                     << " MeV" );

      /// access to the perigee parameters
      const Trk::Perigee& perigee = trackParticle->perigeeParameters();
      const auto& parameters = perigee.parameters();
      ATH_MSG_DEBUG( "Trk::Perigee parameters:" );
      ATH_MSG_DEBUG( " * d_0   : "<< parameters[ Trk::d0 ] );
      ATH_MSG_DEBUG( " * z_0   : "<< parameters[ Trk::z0 ] );
      ATH_MSG_DEBUG( " * phi   : "<< parameters[ Trk::phi ] );
      ATH_MSG_DEBUG( " * Theta : "<< parameters[ Trk::theta ] );
      ATH_MSG_DEBUG( " * q/p   : "<< parameters[ Trk::qOverP ] );

      /// access to TrackSummary information
      ATH_MSG_DEBUG( "Track summary information:" );
      uint8_t ival = 0;
      if( trackParticle->summaryValue( ival, xAOD::numberOfInnermostPixelLayerHits ) ) {
         ATH_MSG_DEBUG( " * Number of B layer hits : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfPixelHits ) ) {
         ATH_MSG_DEBUG( " * Number of pixel hits : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfPixelHoles ) ) {
         ATH_MSG_DEBUG( " * Number of pixel holes : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfSCTHits ) ) {
         ATH_MSG_DEBUG( " * Number of SCT hits : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfSCTHoles ) ) {
         ATH_MSG_DEBUG( " * Number of SCT holes : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfTRTHits ) ) {
         ATH_MSG_DEBUG( " * Number of TRT hits : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival,
                                       xAOD::numberOfTRTHighThresholdHits ) ) {
         ATH_MSG_DEBUG( " * Number of TRT high threshold hits : "
                        << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfTRTHoles ) ) {
         ATH_MSG_DEBUG( " * Number of TRT holes : " << ( int ) ival );
      }
      if( trackParticle->summaryValue( ival, xAOD::numberOfPrecisionLayers ) ) {
         ATH_MSG_DEBUG( " * Number of MDT hits : " << ( int ) ival );
      }
   }

   // Return gracefully:
   return StatusCode::SUCCESS;
}
