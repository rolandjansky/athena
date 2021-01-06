/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

// EDM include(s).
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/AuxVectorBase.h"

// ROOT include(s).
#include <TFile.h>
#include <TSystem.h>
#include <TError.h>

// System include(s).
#include <memory>

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

int main() {

   // Set the name of the application.
   const char* APP_NAME = "ut_xaodrootaccess_athenaMode_test";

   // Set up the environment.
   RETURN_CHECK( APP_NAME, xAOD::Init() );

   // Open the input file.
   std::unique_ptr< TFile > ifile( TFile::Open( "$ASG_TEST_FILE_DATA",
                                                "READ" ) );
   if( ! ifile ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
               gSystem->Getenv( "ASG_TEST_FILE_DATA" ) );
      return 1;
   }
   xAOD::TEvent event( xAOD::TEvent::kAthenaAccess );
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

   // Loop over the file.
   const Long64_t entries = event.getEntries();
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Load the event.
      if( event.getEntry( entry ) < 0 ) {
         ::Error( APP_NAME, XAOD_MESSAGE( "There was a problem loading entry "
                                          "%lld from file: %s" ),
                  entry, gSystem->Getenv( "ASG_TEST_FILE_DATA" ) );
         return 1;
      }

      // Load some interface containers.
      const SG::AuxVectorBase* vecPtr= nullptr;
      RETURN_CHECK( APP_NAME, event.retrieve( vecPtr, "Photons" ) );
      SIMPLE_ASSERT( vecPtr->getAuxIDs().size() != 0 );
      RETURN_CHECK( APP_NAME, event.retrieve( vecPtr, "TauJets" ) );
      SIMPLE_ASSERT( vecPtr->getAuxIDs().size() != 0 );

      // Load some auxiliary containers directly.
      const SG::IConstAuxStore* auxPtr = nullptr;
      RETURN_CHECK( APP_NAME, event.retrieve( auxPtr, "ElectronsAux." ) );
      SIMPLE_ASSERT( auxPtr->getAuxIDs().size() != 0 );
      RETURN_CHECK( APP_NAME, event.retrieve( auxPtr, "MuonsAux." ) );
      SIMPLE_ASSERT( auxPtr->getAuxIDs().size() != 0 );
   }

   // Return gracefully.
   return 0;
}
