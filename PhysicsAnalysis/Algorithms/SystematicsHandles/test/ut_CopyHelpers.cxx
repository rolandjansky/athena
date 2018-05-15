/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <cstdlib>
#include <cmath>
#include <memory>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/Init.h"
#include "AsgTools/MsgStream.h"
#include "AsgTools/MessageCheck.h"
#include "AsgTools/SgTEvent.h"

// EDM include(s):
#include "xAODBase/IParticleHelpers.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

// Local include(s):
#include "SystematicsHandles/CopyHelpers.h"

/// Function generating a random number in a specific range
double randomInRange( double min, double max ) {
   return static_cast< double >( rand() ) / RAND_MAX * ( max - min ) + min;
}

int main() {

   // Set up the usage of the ANA_CHECK(...) macro.
   using namespace asg::msgUserCode;
   ANA_CHECK_SET_TYPE( int );

   // Set up the environment.
   ANA_CHECK( xAOD::Init() );

   // Create the transient store objects.
   xAOD::TEvent event;
   xAOD::TStore store;
   asg::SgTEvent sgEvent( &event, &store );

   // Create a message stream, to be used later on in the code.
   MsgStream msgStream( "ut_CopyHelpers" );

   // Create a primary container, fill it with some random content, and
   // record it into the transient store.
   auto originalJets = std::make_unique< xAOD::JetContainer >();
   auto originalAux  = std::make_unique< xAOD::JetAuxContainer >();
   originalJets->setStore( originalAux.get() );

   for( int i = 0; i < 10; ++i ) {
      xAOD::Jet* jet = new xAOD::Jet();
      originalJets->push_back( jet );
      jet->setJetP4( xAOD::JetFourMom_t( randomInRange( 10000.0, 50000.0 ), // Pt
                                         randomInRange( -2.5, 2.5 ),        // Eta
                                         randomInRange( -M_PI, M_PI ),      // Phi
                                         randomInRange( 0.0, 500.0 ) ) );   // M
   }

   const xAOD::JetContainer* originalJetsPtr = originalJets.get();
   ANA_CHECK( store.record( std::move( originalJets ), "OriginalJets" ) );
   ANA_CHECK( store.record( std::move( originalAux ),  "OriginalJetsAux." ) );

   // Make it easier to use CP::detail::ShallowCopy.
   using CP::detail::ShallowCopy;

   // Make a copy of this simple container.
   xAOD::JetContainer* copiedJets = nullptr;
   ANA_CHECK( ShallowCopy< xAOD::JetContainer >::getCopy( msgStream,
                                                          sgEvent,
                                                          copiedJets,
                                                          originalJetsPtr,
                                                          "CopiedJets",
                                                          "CopiedJetsAux." ) );

   // Make sure that the copied objects point to the originals.
   ANA_CHECK( copiedJets->size() == originalJetsPtr->size() );
   for( size_t i = 0; i < copiedJets->size(); ++i ) {
      ANA_CHECK( xAOD::getOriginalObject( *( copiedJets->at( i ) ) ) ==
                 originalJetsPtr->at( i ) );
   }
   msgStream << MSG::INFO << "Simple copy succeeded" << endmsg;

   // Make a view copy of the copied container, which selects only every second
   // object from it.
   auto viewCopy = std::make_unique< xAOD::JetContainer >( SG::VIEW_ELEMENTS );
   for( size_t i = 0; i < copiedJets->size(); i = i + 2 ) {
      viewCopy->push_back( copiedJets->at( i ) );
   }

   const xAOD::JetContainer* viewCopyPtr = viewCopy.get();
   ANA_CHECK( store.record( std::move( viewCopy ), "ViewOfCopiedJets" ) );

   // Now make a shallow copy of this view copy.
   xAOD::JetContainer* copiedViewJets = nullptr;
   ANA_CHECK( ShallowCopy< xAOD::JetContainer >::getCopy( msgStream,
                                                          sgEvent,
                                                          copiedViewJets,
                                                          viewCopyPtr,
                                                          "CopyOfViewJets",
                                                          "CopyOfViewJetsAux." ) );

   // Check the copy. The logic here is a bit complicated, since the original
   // objects of the copies of the view container are still the objects in the
   // original container. And not the objects in the view container.
   ANA_CHECK( copiedViewJets->size() == viewCopyPtr->size() );
   for( size_t i = 0; i < viewCopyPtr->size(); ++i ) {
      ANA_CHECK( xAOD::getOriginalObject( *( copiedViewJets->at( i ) ) ) ==
                 originalJetsPtr->at( copiedViewJets->at( i )->index() ) );
   }
   msgStream << MSG::INFO << "Copy of view vector succeeded" << endmsg;

   return 0;
}
