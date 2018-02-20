/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <cmath>
#include <iomanip>
#include <array>

// EDM include(s):
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"

// Local include(s):
#include "xAODTruthReader.h"

namespace xAODReader {


   xAODTruthReader::xAODTruthReader( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      /// Declare the algorithm properties:
      declareProperty( "xAODTruthEventContainerName",
                       m_xaodTruthEventContainerName = "TruthEvents" );
      declareProperty( "xAODTruthPileupEventContainerName",
                       m_xaodTruthPUEventContainerName = "TruthPileupEvents" );
   }

   StatusCode xAODTruthReader::initialize() {

      ATH_MSG_INFO( "Will be reading:" );
      ATH_MSG_INFO( "  xAOD TruthEventContainer = "
                    << m_xaodTruthEventContainerName );
      ATH_MSG_INFO( "  xAOD TruthPileupEventContainer = "
                    << m_xaodTruthPUEventContainerName );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODTruthReader::execute() {

      // Retrieve the xAOD truth:
      const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
      ATH_CHECK( evtStore()->retrieve( xTruthEventContainer,
                                       m_xaodTruthEventContainerName));
      const xAOD::TruthPileupEventContainer* xTruthPUEventContainer = nullptr;
      if( evtStore()->contains< xAOD::TruthPileupEventContainer >(
                                           m_xaodTruthPUEventContainerName ) ) {
         ATH_CHECK( evtStore()->retrieve( xTruthPUEventContainer,
                                          m_xaodTruthPUEventContainerName ) );
      }

      ATH_MSG_INFO( "Number of signal events in this Athena event: " <<
                    xTruthEventContainer->size());
      if( xTruthPUEventContainer ) {
         ATH_MSG_INFO( "Number of pile-up events in this Athena event: "
                       << xTruthPUEventContainer->size() );
      }

      // Signal process loop
      ATH_MSG_INFO("Printing signal event...");
      for( const xAOD::TruthEvent* evt : *xTruthEventContainer ) {

         ATH_MSG_INFO( "" );

         // Print hard-scattering info
         const xAOD::TruthVertex* vtx = evt->signalProcessVertex();
         ATH_MSG_INFO( "Signal process vertex: " << vtx );
         if( vtx ) {
            ATH_MSG_INFO( "Poistion = (" << vtx->x() << ", " << vtx->y()
                          << ", " << vtx->z() << ")" );
         } else {
            ATH_MSG_INFO( "Position n.a." );
         }

         // Print the event particle/vtx contents
         printEvent( *evt );
      }

      // Pile-up loop
      ATH_MSG_INFO( "Printing pileup events..." );
      if( xTruthPUEventContainer ) {
         for( const xAOD::TruthPileupEvent* evt : *xTruthPUEventContainer ) {
            ATH_MSG_INFO( "" );
            printEvent( *evt );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   // Print method for event - mimics the HepMC dump.
   // Vertex print method called within here
   void xAODTruthReader::printEvent( const xAOD::TruthEventBase& event ) {

      ATH_MSG_INFO( "----------------------------------------------------------"
                    "----------------------" );

      ATH_MSG_INFO( " Entries this event: " << event.nTruthVertices()
                    << " vertices, " << event.nTruthParticles()
                    << " particles." );

      // Particles and vertices
      ATH_MSG_INFO( "                                    GenParticle Legend" );
      ATH_MSG_INFO( "        Barcode   PDG ID      ( Px,       Py,       Pz,    "
                    " E ) Stat  DecayVtx" );
      ATH_MSG_INFO( "----------------------------------------------------------"
                    "----------------------" );
      for( size_t iv = 0; iv < event.nTruthVertices(); ++iv ) {
         const xAOD::TruthVertex* v = event.truthVertex( iv );
         if( v ) {
            printVertex( *v );
         }
      }
      ATH_MSG_INFO( "----------------------------------------------------------"
                    "----------------------" );

      return;
   }

   // Print method for vertex - mimics the HepMC dump.
   // Particle print method called within here
   void xAODTruthReader::printVertex( const xAOD::TruthVertex& vertex ) {

      // Remember the message stream's flags:
      const auto f = msg().flags();

      msg( MSG::INFO ) << "TruthVertex:";
      msg().width( 9 );

      if( vertex.barcode() ) {
         msg() << vertex.barcode();
      } else {
         msg() << ( void* )( &vertex );
      }

      msg() << " ID:";
      msg().width( 5 );
      msg() << vertex.id();
      msg() << " (X,cT)=";

      if( ( std::abs( vertex.x() > 0.0001 ) ) &&
          ( std::abs( vertex.y() > 0.0001 ) ) &&
          ( std::abs( vertex.z() > 0.0001 ) ) ) {

         msg().setf( std::ios::scientific, std::ios::floatfield );
         msg().setf( std::ios_base::showpos );
         for( float pos : std::array< float, 3 >( { vertex.x(),
                     vertex.y(),
                     vertex.z() } ) ) {
            msg().width( 9 );
            msg().precision( 2 );
            msg() << pos << ",";
         }
         msg().width( 9 );
         msg().precision( 2 );
         msg() << vertex.t();
         msg().setf( std::ios::fmtflags( 0 ), std::ios::floatfield );
         msg().unsetf( std::ios_base::showpos );

      } else {
         msg() << " 0";
      }

      msg() << endmsg;

      // Print out all the incoming, then outgoing particles
      for( size_t iPIn = 0; iPIn < vertex.nIncomingParticles(); ++iPIn ) {
         if( iPIn == 0 ) {
            msg() << " I: ";
            msg().width( 2 );
            msg() << vertex.nIncomingParticles();
         } else {
            msg() << "      ";
         }
         const xAOD::TruthParticle* p = vertex.incomingParticle( iPIn );
         if( p ) {
            printParticle( *p );
         }
      }
      for( size_t iPOut = 0; iPOut < vertex.nOutgoingParticles(); ++iPOut ) {
         if( iPOut == 0 ) {
            msg() << " O: ";
            msg().width( 2 );
            msg() << vertex.nOutgoingParticles();
         } else {
            msg() << "      ";
         }
         const xAOD::TruthParticle* p = vertex.outgoingParticle( iPOut );
         if( p ) {
            printParticle( *p );
         }
      }
  
      // Restore the stream's original flags:
      msg().flags( std::ios::fmtflags( f ) );

      return;
   }

   // Print method for particle - mimics the HepMC dump.
   void xAODTruthReader::printParticle( const xAOD::TruthParticle& particle ) {

      // Remember the message stream's flags:
      const auto f = msg().flags();

      msg() << " ";
      msg().width( 9 );
      msg() << particle.barcode();
      msg().width( 9 );
      msg() << particle.pdgId() << " ";
      msg().setf( std::ios::scientific, std::ios::floatfield );
      msg().setf( std::ios_base::showpos );
      for( float mom : std::array< float, 3 >( { particle.px(),
                  particle.py(), particle.pz() } ) ) {
         msg().width( 0 );
         msg().precision( 2 );
         msg() << mom << ",";
      }
      msg().width( 9 );
      msg().precision( 2 );
      msg() << particle.e() << " ";
      msg().setf( std::ios::fmtflags( 0 ), std::ios::floatfield );
      msg().unsetf( std::ios_base::showpos );
      if( particle.hasDecayVtx() ) {
         if( particle.decayVtx()->barcode() != 0 ) {
            msg().width( 3 );
            msg() << particle.status() << " ";
            msg().width( 9 );
            msg() << particle.decayVtx()->barcode();
         }
      } else {
         msg().width( 3 );
         msg() << particle.status();
      }
      msg() << endmsg;

      // Restore the stream's original flags:
      msg().flags( std::ios::fmtflags( f ) );

      return;
   }

} // namespace xAODReader
