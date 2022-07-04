/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TSystem.h>
#include <TTree.h>

// EDM that the package uses anyway:
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

/// Helper macro
#define RETURN_CHECK( CONTEXT, EXP )                                 \
   do {                                                              \
      const StatusCode result = EXP;                          \
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

/// Type used in the test
class ClassA {

public:
   int m_var1;
   float m_var2;

}; // class ClassA

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_metadata_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init() );

   // Create the tested object(s):
   xAOD::TEvent event;

   // First, test the reading of metadata from an input file.
   const std::string FNAME = "${ASG_TEST_FILE_DATA}";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME.c_str(),
                                                    "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
               FNAME.c_str() );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );
   Info( APP_NAME, "Opened file: %s", FNAME.c_str() );

   // Make sure that event data is not accessible at this point.
   const SG::AuxElement* ei = nullptr;
   SIMPLE_ASSERT( event.retrieve( ei, "EventInfo" ).isSuccess() == false );

   // Temporarily disabled as it doesn't currently exist in DAOD_PHYS
   // // Try to retrieve some objects:
   // const SG::AuxVectorBase* v = 0;
   // const xAOD::AuxContainerBase* c = 0;
   // RETURN_CHECK( APP_NAME, event.retrieveMetaInput( v, "TriggerMenu" ) );
   // SIMPLE_ASSERT( v->size_v() == 1 );
   // RETURN_CHECK( APP_NAME, event.retrieveMetaInput( c, "TriggerMenuAux." ) );
   // SIMPLE_ASSERT( c->size() == 1 );

   // Open a test output file:
   static const char* const OFNAME = "test.root";
   std::unique_ptr< ::TFile > ofile( ::TFile::Open( OFNAME, "RECREATE" ) );
   RETURN_CHECK( APP_NAME, event.writeTo( ofile.get() ) );

   // The auxiliary type registry:
   SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

   // Try to record some simple store objects into the output:
   std::unique_ptr< xAOD::AuxContainerBase >
      auxOut1( new xAOD::AuxContainerBase() );
   auxOut1->getData( r.getAuxID< int >( "IntVar" ), 1, 1 );
   auxOut1->getData( r.getAuxID< float >( "FloatVar" ), 1, 1 );
   RETURN_CHECK( APP_NAME, event.recordMeta( std::move( auxOut1 ),
                                             "MetadataAux." ) );
   std::unique_ptr< xAOD::AuxContainerBase >
      auxOut2( new xAOD::AuxContainerBase() );
   auxOut2->getData( r.getAuxID< int >( "IntVar" ), 2, 2 );
   auxOut2->getData( r.getAuxID< float >( "FloatVar" ), 2, 2 );
   RETURN_CHECK( APP_NAME, event.recordMeta( std::move( auxOut2 ),
                                             "TriggerMenuAux." ) );

   // Recording an object without a dictionary should fail:
   std::unique_ptr< ClassA > clA( new ClassA() );
   SIMPLE_ASSERT( event.recordMeta( std::move( clA ),
                                    "ClassA" ).isFailure() == true );

   // Finish writing to the output file:
   RETURN_CHECK( APP_NAME, event.finishWritingTo( ofile.get() ) );

   // Now investigate the file a bit:
   ::TTree* metatree = dynamic_cast< ::TTree* >( ofile->Get( "MetaData" ) );
   SIMPLE_ASSERT( metatree != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "MetadataAux." ) != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "MetadataAuxDyn.IntVar" ) != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "MetadataAuxDyn.FloatVar" ) != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "TriggerMenuAux." ) != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "TriggerMenuAuxDyn.IntVar" ) != 0 );
   SIMPLE_ASSERT( metatree->GetBranch( "TriggerMenuAuxDyn.FloatVar" ) != 0 );

   // If everything went well, close and delete the file:
   ofile->Close();
   SIMPLE_ASSERT( gSystem->Unlink( OFNAME ) == 0 );

   // Finish with a clear statement:
   ::Info( APP_NAME, "All tests succeeded" );

   // Return gracefully:
   return 0;
}
