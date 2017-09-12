/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TSystem.h>

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

/// Helper macro
#define CHECK( CONTEXT, EXP )                                        \
   do {                                                              \
      const xAOD::TReturnCode result = EXP;                          \
      if( ! result.isSuccess() ) {                                   \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to execute: %s" ),  \
                  #EXP );                                            \
         return 1;                                                   \
      }                                                              \
   } while( false )

/// Function testing the copying of a few objects
xAOD::TReturnCode copyObjects( xAOD::TEvent::EAuxMode mode );

int main() {

   // The name of the application
   static const char* APP_NAME = "ut_xaodrootaccess_tevent_copy_test";

   // Initialise the environment:
   CHECK( APP_NAME, xAOD::Init() );

   // Test the copying in class access mode:
   CHECK( APP_NAME, copyObjects( xAOD::TEvent::kClassAccess ) );
   // Test the copying in athena access mode:
   CHECK( APP_NAME, copyObjects( xAOD::TEvent::kAthenaAccess ) );

   // Return gracefully:
   return 0;
}

xAOD::TReturnCode copyObjects( xAOD::TEvent::EAuxMode mode ) {

   // Construct a "mode name" for the printed messages:
   TString modeName;
   switch( mode ) {
   case xAOD::TEvent::kBranchAccess:
      modeName = "kBranchAccess";
      break;
   case xAOD::TEvent::kClassAccess:
      modeName = "kClassAccess";
      break;
   case xAOD::TEvent::kAthenaAccess:
      modeName = "kAthenaAccess";
      break;
   default:
      modeName = "<unknown>";
      break;
   }

   // Create the object that will be tested:
   xAOD::TEvent event( mode );

   // Connect it to the test input file:
   std::unique_ptr< TFile > ifile( TFile::Open( "$ASG_TEST_FILE_DATA",
                                                "READ" ) );
   if( ! ifile ) {
      Error( "copyObjects", XAOD_MESSAGE( "Couldn't open input file: %s" ),
             gSystem->Getenv( "ASG_TEST_FILE_DATA" ) );
      return xAOD::TReturnCode::kFailure;
   }
   RETURN_CHECK( "copyObjects", event.readFrom( ifile.get() ) );
   Info( "copyObjects", "Opened input file %s in mode %s",
         gSystem->Getenv( "ASG_TEST_FILE_DATA" ), modeName.Data() );

   // Connect it to an output file:
   std::unique_ptr< TFile > ofile( TFile::Open( "dummy_output.root",
                                                "RECREATE" ) );
   if( ! ofile.get() ) {
      Error( "copyObjects", XAOD_MESSAGE( "Couldn't open the output file" ) );
      return xAOD::TReturnCode::kFailure;
   }
   RETURN_CHECK( "copyObjects", event.writeTo( ofile.get() ) );
   Info( "copyObjects", "Opened the output file" );

   // Loop over a maximum of 50 events:
   const Long64_t entries = event.getEntries() > 50 ? 50 : event.getEntries();
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Get the entry from the input:
      if( event.getEntry( entry ) < 0 ) {
         Error( "copyObjects",
                XAOD_MESSAGE( "Couldn't load entry %i from the input file" ),
                static_cast< int >( entry ) );
         return xAOD::TReturnCode::kFailure;
      }

      // Copy a standalone object:
      RETURN_CHECK( "copyObjects", event.copy( "EventInfo" ) );
      // Copy a container:
      RETURN_CHECK( "copyObjects", event.copy( "Electrons" ) );

      // Write the event to the output file:
      if( event.fill() <= 0 ) {
         Error( "copyObjects", XAOD_MESSAGE( "Failed to write event %i" ),
                static_cast< int >( entry ) );
         return xAOD::TReturnCode::kFailure;
      }
   }

   // Let the user know what happened:
   Info( "copyObjects", "Copying objects from %i events in mode %s succeeded",
         static_cast< int >( entries ), modeName.Data() );

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}
