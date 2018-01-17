// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tevent_test.cxx 796448 2017-02-09 18:28:08Z ssnyder $

// System include(s):
#include <memory>
#include <vector>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TSystem.h>

// EDM that the package uses anyway:
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TAuxStore.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/Utils.h"
#include "xAODRootAccessInterfaces/TActiveEvent.h"
#include "xAODRootAccessInterfaces/TVirtualEvent.h"

/// Helper macro
#define RETURN_CHECK( CONTEXT, EXP )                                 \
   do {                                                              \
      const xAOD::TReturnCode result = EXP;                          \
      if( ! result.isSuccess() ) {                                   \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to execute: %s" ),  \
                  #EXP );                                            \
         return 1;                                                   \
      }                                                              \
   } while( false )

/// Another little helper macro
#define SIMPLE_ASSERT( EXP )                                                \
   do {                                                                     \
      if( ! ( EXP ) ) {                                                     \
         ::Error( APP_NAME, XAOD_MESSAGE( "Error evaluating: %s" ), #EXP ); \
         return 1;                                                          \
      }                                                                     \
   } while( 0 )

/// Type used in the event/store test
class ClassA {
   
public:
   int m_var1;
   float m_var2;
   
}; // class ClassA

/// Type used in the event/store test
class ClassB : public ClassA {
   
public:
   int m_var3;
   float m_var4;
   
}; // class ClassB

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_tevent_test";
   
   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
   
   // Create the tested object(s):
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   xAOD::TStore store;

   // Connect an input file to the event:
   const char* ref = getenv ("ATLAS_REFERENCE_DATA");
   std::string FPATH =
     ref ? ref : "/afs/cern.ch/atlas/project/PAT";
   std::string FNAME = FPATH + 
      "/xAODs/r5597/"
      "data12_8TeV.00204158.physics_JetTauEtmiss.recon.AOD.r5597/"
      "AOD.01495682._003054.pool.root.1";
   std::auto_ptr< ::TFile > ifile( ::TFile::Open( FNAME.c_str(), "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
               FNAME.c_str() );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

   // Read in the first event:
   if( event.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load entry 0 from file %s" ),
               FNAME.c_str() );
      return 1;
   }

   // Try to retrieve some objects:
   const xAOD::AuxContainerBase* c = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( c, "ElectronCollectionAux." ) );
   RETURN_CHECK( APP_NAME, event.retrieve( c, "MuonsAux." ) );

   // Check if the second one was assigned the correct type of internal store:
   const SG::IAuxStore* dynStore = c->getStore();
   if( ! dynamic_cast< const xAOD::TAuxStore* >( dynStore ) ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Dynamic store not updated correctly!" ) );
      return 1;
   }

   // Record some objects into TStore:
   ClassA* objA = new ClassA();
   RETURN_CHECK( APP_NAME, store.record( objA, "MyObjA" ) );
   ClassB* objB = new ClassB();
   RETURN_CHECK( APP_NAME, store.record( objB, "MyObjB" ) );

   // Print what's in the event now:
   Info( APP_NAME, "TEvent contents:\n\n%s\n\n", event.dump().c_str() );

   // They should now be accessible through TEvent:
   const ClassA* dummy1 = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( dummy1, "MyObjA" ) );
   const ClassB* dummy2 = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( dummy2, "MyObjB" ) );

   // Try to get them through the TVirtualEvent interface:
   xAOD::TVirtualEvent* vevent = xAOD::TActiveEvent::event();
   SIMPLE_ASSERT( vevent != 0 );
   SIMPLE_ASSERT( vevent->retrieve( dummy1,
                                    xAOD::Utils::hash( "MyObjA" ) ) == true );
   SIMPLE_ASSERT( vevent->retrieve( dummy2,
                                    xAOD::Utils::hash( "MyObjB" ) ) == true );
   SIMPLE_ASSERT( vevent->retrieve( dummy1, 0x1234 ) == false );

   // Test the functions used by the smart pointers:
   if( event.getName( objA ) != "MyObjA" ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "getName(objA) = \"%s\" (!=\"MyObjA\")" ),
               event.getName( objA ).c_str() );
      return 1;
   }
   if( event.getName( xAOD::Utils::hash( "MyObjB" ) ) != "MyObjB" ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Couldn't retrieve the name for hashed "
                             "\"MyObjB\"" ) );
      return 1;
   }
   if( event.getName( ( void* ) 0x12345678 ) != "" ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Found a name for an imaginary pointer?!?" ) );
      return 1;
   }
   if( event.getName( 0x12345678 ) != "" ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Found a name for an imaginary hash?!?" ) );
      return 1;
   }

   // This is not supposed to work:
   if( event.retrieve( dummy1, "MyObjB" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Something strange happened" ) );
      return 1;
   }

   // Clear the container:
   store.clear();

   // Now this is not supposed to work either:
   if( event.retrieve( dummy2, "MyObjB" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Something strange happened" ) );
      return 1;
   }

   // Record a constant DV into the store:
   DataVector< ClassA > dv;
   dv.push_back( new ClassA() );
   dv.push_back( new ClassA() );
   ConstDataVector< DataVector< ClassA > >* cdv =
      new ConstDataVector< DataVector< ClassA > >( SG::VIEW_ELEMENTS );
   RETURN_CHECK( APP_NAME, store.record( cdv, "ConstDataVector" ) );

   // Try to retrieve it in all possible ways:
   ConstDataVector< DataVector< ClassA > >* cdv1 = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( cdv1, "ConstDataVector" ) );
   const ConstDataVector< DataVector< ClassA > >* cdv2 = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( cdv2, "ConstDataVector" ) );
   const DataVector< ClassA >* cdv3 = 0;
   RETURN_CHECK( APP_NAME, event.retrieve( cdv3, "ConstDataVector" ) );

   // But this should not work:
   DataVector< ClassA >* cdv4 = 0;
   if( event.retrieve( cdv4, "ConstDataVector" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Problem detected" ) );
      return 1;
   }

   // Create another TEvent instance to test the file writing capabilities of
   // the code:
   xAOD::TEvent wevent;

   // And connect it to an output file:
   static const char* OFNAME = "test.xAOD.root";
   std::unique_ptr< ::TFile > ofile( ::TFile::Open( OFNAME, "RECREATE" ) );
   if( ! ofile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't create test output file: %s" ),
               OFNAME );
      return 1;
   }
   RETURN_CHECK( APP_NAME, wevent.writeTo( ofile.get() ) );

   // Construct some xAOD store objects:
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
   std::unique_ptr< xAOD::AuxContainerBase > c1( new xAOD::AuxContainerBase() );
   c1->resize( 5 );
   c1->getData( reg.getAuxID< int >( "IntVar" ), 5, 5 );
   c1->getData( reg.getAuxID< float >( "FloatVar" ), 5, 5 );
   std::unique_ptr< xAOD::AuxContainerBase > c2( new xAOD::AuxContainerBase() );
   c2->resize( 10 );
   c2->getData( reg.getAuxID< int >( "IntVar" ), 10, 10 );
   c2->getData( reg.getAuxID< float >( "FloatVar" ), 10, 10 );

   // And some STL ones as well:
   std::unique_ptr< std::vector< float > >
      fvector( new std::vector< float >() );
   fvector->insert( fvector->begin(), 5, 1.23 );
   std::unique_ptr< std::vector< std::string > >
      svector( new std::vector< std::string >() );
   svector->insert( svector->begin(), 3, "foo" );

   // It should be possible to record these into the output:
   RETURN_CHECK( APP_NAME, wevent.record( std::move( c1 ), "Container1Aux." ) );
   RETURN_CHECK( APP_NAME, wevent.record( std::move( c2 ), "Container2Aux." ) );
   RETURN_CHECK( APP_NAME, wevent.record( std::move( fvector ), "FloatVec" ) );
   RETURN_CHECK( APP_NAME, wevent.record( std::move( svector ), "StringVec" ) );
   if( wevent.fill() <= 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't write event to output" ) );
      return 1;
   }

   // Now update just some of them, and see what happens. Remember that the old
   // objects are now owned by TEvent, so we can re-use these pointers.
   c1.reset( new xAOD::AuxContainerBase() );
   c1->resize( 5 );
   c1->getData( reg.getAuxID< int >( "IntVar" ), 5, 5 );
   c1->getData( reg.getAuxID< float >( "FloatVar" ), 5, 5 );
   svector.reset( new std::vector< std::string >() );
   svector->insert( svector->begin(), 4, "bar" );
   RETURN_CHECK( APP_NAME, wevent.record( std::move( c1 ), "Container1Aux." ) );
   RETURN_CHECK( APP_NAME, wevent.record( std::move( svector ), "StringVec" ) );
   if( wevent.fill() > 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "This writing should've failed. o.O" ) );
      return 1;
   }

   // And now try to record an object without a dictionary. Which should
   // of course fail.
   std::unique_ptr< ClassA > c3( new ClassA() );
   if( wevent.record( std::move( c3 ), "ClassA" ).isSuccess() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "This record should've failed. o.O" ) );
      return 1;
   }

   // Finish writing to the file:
   RETURN_CHECK( APP_NAME, wevent.finishWritingTo( ofile.get() ) );
   ofile->Close();

   // And delete the file from disk:
   if( gSystem->Unlink( OFNAME ) ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't remove temporary file: %s" ),
               OFNAME );
      return 1;
   }

   // Return gracefully:
   return 0;
}
