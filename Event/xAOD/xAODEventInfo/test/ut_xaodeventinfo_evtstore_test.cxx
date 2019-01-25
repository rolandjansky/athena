/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodeventinfo_evtstore_test.cxx 727083 2016-03-01 15:20:50Z krasznaa $

// System include(s):
#include <memory>
#include <iostream>
#include <unistd.h>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

// Local include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

/// Dummy implementation useful for simple testing
class StoreGateSvc {

public:
   /// Constructor
   StoreGateSvc( int number ) : m_number( number ) {}

   /// The identifier of the object
   int number() const { return m_number; }

private:
   /// The identifier of the object
   int m_number;

}; // class StoreGateSvc

/// Convenience macro for checking the code
#define SIMPLE_ASSERT( EXP )                                   \
   do {                                                        \
      const bool result = EXP;                                 \
      if( ! result ) {                                         \
         std::cerr << "Failed in test: " << #EXP << std::endl; \
         return 1;                                             \
      }                                                        \
   } while( 0 )

int main() {

   // Create objects that we'll write out:
   std::unique_ptr< xAOD::EventInfo > ei( new xAOD::EventInfo() );
   std::unique_ptr< xAOD::EventInfoContainer >
      eic( new xAOD::EventInfoContainer );
   xAOD::EventInfoAuxContainer aux1;
   eic->setStore( &aux1 );

   // Create a vector of StoreGateSvc instances that the EventInfo objects can
   // point at:
   std::vector< StoreGateSvc > evtStores;
   evtStores.reserve( 5 );
   for( int i = 0; i < 5; ++i ) {
      evtStores.push_back( StoreGateSvc( i ) );
   }

   // Set up the test objects:
   ei->setEvtStore( &evtStores[ 2 ] );
   for( int i = 0; i < 5; ++i ) {
      xAOD::EventInfo* e = new xAOD::EventInfo();
      eic->push_back( e );
      e->setEvtStore( &evtStores[ i ] );
      e->setEventNumber( i );
   }

   // Test that the pointers were kept correctly:
   SIMPLE_ASSERT( ei->evtStore() == ( &evtStores[ 2 ] ) );
   for( int i = 0; i < 5; ++i ) {
      SIMPLE_ASSERT( ( *eic )[ i ]->evtStore() == ( &evtStores[ i ] ) );
      SIMPLE_ASSERT( ( *eic )[ i ]->evtStore()->number() == i );
   }

   // Copy the container:
   xAOD::EventInfoContainer eicCopy;
   xAOD::EventInfoAuxContainer aux2;
   eicCopy.setStore( &aux2 );
   for( const xAOD::EventInfo* ptr : *eic ) {
      eicCopy.push_back( new xAOD::EventInfo( *ptr ) );
      SIMPLE_ASSERT( ptr->evtStore() == eicCopy.back()->evtStore() );
      SIMPLE_ASSERT( ptr->eventNumber() == eicCopy.back()->eventNumber() );
   }

   // Copy it again, this time testing the assignment operator:
   eicCopy.clear();
   for( const xAOD::EventInfo* ptr : *eic ) {
      eicCopy.push_back( new xAOD::EventInfo() );
      *( eicCopy.back() ) = *ptr;
      SIMPLE_ASSERT( ptr->evtStore() == eicCopy.back()->evtStore() );
      SIMPLE_ASSERT( ptr->eventNumber() == eicCopy.back()->eventNumber() );
   }

   //
   // Now, let's write the objects to a test file:
   //
   std::unique_ptr< ::TFile > ofile( ::TFile::Open( "eiTest.root",
                                                    "RECREATE" ) );
   SIMPLE_ASSERT( ofile.get() );
   ofile->cd();

   std::unique_ptr< ::TTree > otree( new ::TTree( "TestTree", "Test Tree" ) );
   otree->SetDirectory( ofile.get() );

   xAOD::EventInfo* eiP = ei.get();
   SIMPLE_ASSERT( otree->Branch( "EventInfo", "xAOD::EventInfo_v1", &eiP,
                                 32000, 0 ) );
   xAOD::EventInfoContainer* eicP = eic.get();
   SIMPLE_ASSERT( otree->Branch( "EventInfoContainer",
                                 "xAOD::EventInfoContainer_v1", &eicP,
                                 32000, 0 ) );

   SIMPLE_ASSERT( otree->Fill() > 0 );

   otree->Print();

   otree->Write();
   otree->SetDirectory( 0 );
   ofile->Close();

   //
   // Read the objects back to test that the reading rule is applied correctly:
   //
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( "eiTest.root", "READ" ) );
   SIMPLE_ASSERT( ifile.get() );

   ::TTree* itree = dynamic_cast< ::TTree* >( ifile->Get( "TestTree" ) );
   SIMPLE_ASSERT( itree != 0 );

   eiP = 0;
   SIMPLE_ASSERT( itree->SetBranchAddress( "EventInfo", &eiP ) >= 0 );
   eicP = 0;
   SIMPLE_ASSERT( itree->SetBranchAddress( "EventInfoContainer", &eicP ) >= 0 );

   SIMPLE_ASSERT( itree->GetEntry( 0 ) > 0 );

   SIMPLE_ASSERT( eiP->evtStore() == 0 );
   SIMPLE_ASSERT( eicP->size() == 5 );
   for( int i = 0; i < 5; ++i ) {
      SIMPLE_ASSERT( ( *eicP )[ i ]->evtStore() == 0 );
   }

   // Now set the pointers to some values, trigger the reading from the branches
   // again, and check that the values are reset to zero.
   eiP->setEvtStore( &evtStores[ 2 ] );
   for( int i = 0; i < 5; ++i ) {
      ( *eicP )[ i ]->setEvtStore( &evtStores[ i ] );
   }

   SIMPLE_ASSERT( itree->GetEntry( 0 ) > 0 );

   SIMPLE_ASSERT( eiP->evtStore() == 0 );
   SIMPLE_ASSERT( eicP->size() == 5 );
   for( int i = 0; i < 5; ++i ) {
      SIMPLE_ASSERT( ( *eicP )[ i ]->evtStore() == 0 );
   }

   // Close and delete the file:
   ifile->Close();
   SIMPLE_ASSERT( unlink( "eiTest.root" ) == 0 );

   // Return gracefully:
   return 0;
}
