/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TClass.h>
#include <TError.h>
#include <TSystem.h>

// xAOD include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

// Local include(s):
#include "TrigBunchCrossingTool/TrigConfBunchCrossingTool.h"

/// A little helper macro
#define SIMPLE_CHECK( EXP )                                       \
   do {                                                           \
      if( ! EXP.isSuccess() ) {                                   \
         std::cerr << "Failed to execute: " << #EXP << std::endl; \
            return 1;                                             \
      }                                                           \
   } while( 0 )

/// Another helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                            \
   do {                                                                 \
      const bool result = EXP;                                          \
      if( ! result ) {                                                  \
         std::cerr << "Expression \"" << #EXP << "\" failed the evaluation" \
                   << std::endl;                                        \
            return 1;                                                   \
      }                                                                 \
   } while( 0 )

int main() {

   // The name of the application:
   static const char* APP_NAME = "ut_trigconf_bunch_tool_test";

   // Initialise the environment for xAOD reading:
   SIMPLE_CHECK( xAOD::Init() );

   // Open the input file:
   std::unique_ptr< TFile > ifile( TFile::Open( "$ASG_TEST_FILE_DATA",
                                                "READ" ) );
   if( ! ifile.get() ) {
      Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file: %s" ),
             gSystem->Getenv( "ASG_TEST_FILE_DATA" ) );
      return 1;
   }
   Info( APP_NAME, "Opened file: %s", gSystem->Getenv( "ASG_TEST_FILE_DATA" ) );

   // Set up the reading of an example file:
   xAOD::TEvent event;
   SIMPLE_CHECK( event.readFrom( ifile.get() ) );

   // Create a trigger configuration tool. Note that this is a quite fragile
   // implementation, which relies on knowing that the inheritance of the tool
   // allows us to do this.
   TClass* cl = TClass::GetClass( "TrigConf::xAODConfigTool" );
   if( ! cl ) {
      Error( APP_NAME, XAOD_MESSAGE( "No dictionary is available for "
                                     "TrigConf::xAODConfigTool" ) );
      return 1;
   }
   std::unique_ptr< asg::AsgMetadataTool >
      configTool( reinterpret_cast< asg::AsgMetadataTool* >( cl->New() ) );
   if( ! configTool.get() ) {
      Error( APP_NAME,
             XAOD_MESSAGE( "Couldn't create TrigConf::xAODConfigTool" ) );
      return 1;
   }
   SIMPLE_CHECK( configTool->initialize() );

   // Create the tool to be tested:
   Trig::TrigConfBunchCrossingTool tool;
   SIMPLE_CHECK( tool.setProperty( "OutputLevel", MSG::INFO ) );
   SIMPLE_CHECK( tool.initialize() );

   // Loop over the events in the file:
   const ::Long64_t entries = event.getEntries();
   for( ::Long64_t entry = 0; entry < entries; ++entry ) {

      // Load the event:
      if( event.getEntry( entry ) < 0 ) {
         ::Error( APP_NAME, "Couldn't load event %i from the input file",
                  static_cast< int >( entry ) );
         return 1;
      }

      // Print some status info:
      if( ! ( entry % 100 ) ) {
         ::Info( APP_NAME, "Processing event %i / %i",
                 static_cast< int >( entry ),
                 static_cast< int >( entries ) );
      }

      // Now ask some questions about the loaded configuration:
      SIMPLE_ASSERT( tool.isFilled( 1 ) );
      SIMPLE_ASSERT( tool.isInTrain( 41 ) );
      SIMPLE_ASSERT( tool.distanceFromFront( 183 ) == 0 );
      SIMPLE_ASSERT( tool.distanceFromFront( 392 ) == 300 );
      SIMPLE_ASSERT( tool.gapBeforeTrain( 250 ) == 250 );
      SIMPLE_ASSERT( tool.bunchTrainSpacing() == 25 );
   }

   // Return gracefully:
   return 0;
}
