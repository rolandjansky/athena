/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <cmath>
#include <iomanip>
#include <array>

// EDM include(s):
#include "GeneratorObjects/McEventCollection.h"

// Local include(s):
#include "HepMCTruthReader.h"


HepMCTruthReader::HepMCTruthReader( const std::string& name,
                                    ISvcLocator* svcLoc )
   : AthAlgorithm(name, svcLoc) {

   /// Declare the algorithm properties:
   declareProperty( "HepMCContainerName", m_hepMCContainerName = "GEN_EVENT" );
}


StatusCode HepMCTruthReader::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Will be printing the properties of container: "
                 << m_hepMCContainerName );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode HepMCTruthReader::execute() {

   // Retrieve the HepMC truth:
   const McEventCollection* mcColl = nullptr;
   ATH_CHECK( evtStore()->retrieve( mcColl, m_hepMCContainerName ) );
   ATH_MSG_INFO( "Number of pile-up events in this Athena event: "
                 << ( mcColl->size() - 1 ) );

   // Loop over the events:
   for( size_t i = 0; i < mcColl->size(); ++i ) {

      // Access the event:
      const HepMC::GenEvent* genEvt = ( *mcColl )[ i ]; 

      // Tell the user what sort of event it is:
      switch( i ) {
      case 0:
         {
            ATH_MSG_INFO( "Printing signal event..." );
            const HepMC::GenVertex* spv = genEvt->signal_process_vertex();
            ATH_MSG_INFO( "Signal process vertex position: ("
                          << spv->position().x() << ", "
                          << spv->position().y() << ", "
                          << spv->position().z() << "). Pointer: " << spv );
         }
         break;
      case 1:
         ATH_MSG_INFO( "Printing pileup events..." );
         break;
      default:
         break;
      }

      // Print the event:
      printEvent( *genEvt );
   }

   // Return gracefully:
   return StatusCode::SUCCESS;
}


/// Print method for event - mimics the HepMC dump.
/// Vertex print method called within here
void HepMCTruthReader::printEvent( const HepMC::GenEvent& event ) {

   // Print a header:
   ATH_MSG_INFO( "-------------------------------------------------------------"
                 "-------------------" );
   ATH_MSG_INFO( "GenEvent: #" << event.event_number() );
   ATH_MSG_INFO( " Entries this event: " << event.vertices_size()
                 << " vertices, " << event.particles_size() << " particles." );

   // Print the particles and vertices:
   ATH_MSG_INFO( "                                    GenParticle Legend" );
   ATH_MSG_INFO( "        Barcode   PDG ID      ( Px,       Py,       Pz,     E"
                 " ) Stat  DecayVtx" );
   ATH_MSG_INFO( "-------------------------------------------------------------"
                 "-------------------" );

   auto vtx_itr = event.vertices_begin();
   auto vtx_end = event.vertices_end();
   for( ; vtx_itr != vtx_end; ++vtx_itr ) {
      printVertex( **vtx_itr );
   }

   ATH_MSG_INFO( "-------------------------------------------------------------"
                 "-------------------" );

   return;
}

// Print method for vertex - mimics the HepMC dump.
// Particle print method called within here
void HepMCTruthReader::printVertex( const HepMC::GenVertex& vertex ) {

   // Remember the message stream's flags:
   const auto f = msg().flags();

   msg( MSG::INFO ) << "GenVertex (" << &vertex << "):";
   msg().width( 9 );

   if( vertex.barcode() != 0 ) {
      msg() << vertex.barcode();
   } else {
      msg() << ( void* )( &vertex );
   }

   msg() << " ID:";
   msg().width( 5 );
   msg() << vertex.id();
   msg() << " (X,cT)=";

   if( ( std::abs( vertex.position().x() ) > 0.0001 ) &&
       ( std::abs( vertex.position().y() ) > 0.0001 ) &&
       ( std::abs( vertex.position().z() ) > 0.0001 ) ) {

      msg().setf( std::ios::scientific, std::ios::floatfield );
      msg().setf( std::ios_base::showpos );
      for( double pos : std::array< double, 3 >( {
                  vertex.position().x(),
                  vertex.position().y(),
                  vertex.position().z() } ) ) {
         msg().width( 9 );
         msg().precision( 2 );
         msg() << pos << ",";
      }
      msg().width( 9 );
      msg().precision( 2 );
      msg() << vertex.position().t();
      msg().setf( std::ios::fmtflags( 0 ), std::ios::floatfield );
      msg().unsetf( std::ios_base::showpos );

   } else {
      msg() << " 0";
   }

   msg() << endmsg;

   // Print out all the incoming, then outgoing particles
   auto pin_itr = vertex.particles_in_const_begin();
   auto pin_end = vertex.particles_in_const_end();
   for( ; pin_itr != pin_end; ++pin_itr ) {       
      if( pin_itr == vertex.particles_in_const_begin() ) {
         msg() << " I: ";
         msg().width( 2 );
         msg() << vertex.particles_in_size();
      } else {
         msg() << "      ";
      }
      printParticle( **pin_itr );
   }

   auto pout_itr = vertex.particles_out_const_begin();
   auto pout_end = vertex.particles_out_const_end();
   for( ; pout_itr != pout_end; ++pout_itr ) {
      if( pout_itr == vertex.particles_out_const_begin() ) {
         msg() << " O: ";
         msg().width( 2 );
         msg() << vertex.particles_out_size();
      } else {
         msg() << "      ";
      }
      printParticle( **pout_itr );
   }

   // Restore the stream's original flags:
   msg().flags( std::ios::fmtflags( f ) );

   return;
}


// Print method for particle - mimics the HepMC dump.
void HepMCTruthReader::printParticle( const HepMC::GenParticle& particle ) {

   // Remember the message stream's flags:
   const auto f = msg().flags();

   msg() << " ";
   msg().width( 9 );
   msg() << particle.barcode();
   msg().width( 9 );
   msg() << particle.pdg_id() << " ";
   msg().setf( std::ios::scientific, std::ios::floatfield );
   msg().setf( std::ios_base::showpos );
   for( double mom : std::array< double, 3 >( { particle.momentum().px(),
               particle.momentum().py(), particle.momentum().pz() } ) ) {
      msg().width( 9 );
      msg().precision( 2 );
      msg() << mom << ",";
   }
   msg().width( 9 );
   msg().precision( 2 );
   msg() << particle.momentum().e() << " ";
   msg().setf( std::ios::fmtflags( 0 ), std::ios::floatfield );
   msg().unsetf( std::ios_base::showpos );
   if( particle.has_decayed() ) {
      if( particle.end_vertex()->barcode() != 0 ) {
         msg().width( 3 );
         msg() << particle.status() << " ";
         msg().width( 9 );
         msg() << particle.end_vertex()->barcode();
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
