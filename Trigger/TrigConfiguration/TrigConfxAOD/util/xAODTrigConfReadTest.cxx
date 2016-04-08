/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigConfReadTest.cxx 718020 2016-01-15 15:52:44Z tbold $

// System include(s):
#include <memory>

// ROOT include(s):
#ifdef ROOTCORE
#   include <TFile.h>
#   include <TError.h>
#endif // ROOTCORE

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// ASG include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

// Local include(s):
#include "TrigConfxAOD/xAODConfigTool.h"

int main( int argc, char* argv[] ) {

   // Get the application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check that we received at least one file name:
   if( argc < 2 ) {
      ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
      return 1;
   }

   // Initialise the application:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );

   // Create the trigger configuration tool:
   TrigConf::xAODConfigTool configTool( "xAODConfigTool" );
   //   configTool.setProperty( "MetaObjectName", "Bla" );
   configTool.setProperty( "OutputLevel", MSG::VERBOSE ).ignore();
   if( configTool.initialize().isFailure() ) {
      ::Error( APP_NAME, "Couldn't initialise the trigger configuration tool" );
      return 1;
   }

   // Try creating ToolHandles for this tool:
   ToolHandle< TrigConf::ITrigConfigTool > handle( "xAODConfigTool" );
   ToolHandleArray< TrigConf::ITrigConfigTool > handleArray{ "xAODConfigTool" };

   // Check if they can be retrieved:
   if( handle.retrieve().isFailure() || handleArray.retrieve().isFailure() ) {
      ::Error( APP_NAME,
               "Couldn't retrieve the tested tool through tool handles" );
      return 1;
   }

   // Loop over the files:
   for( int i = 1; i < argc; ++i ) {

      // Open the input file:
      std::unique_ptr< ::TFile > ifile( ::TFile::Open( argv[ i ], "READ" ) );
      if( ! ifile.get() ) {
         ::Error( APP_NAME, "Couldn't open file: %s", argv[ i ] );
         return 1;
      }
      ::Info( APP_NAME, "Opened file: %s", argv[ i ] );

      // Connect the TEvent object to it:
      RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

      // Loop over a few events:
      const ::Long64_t entries = event.getEntries();
      for( ::Long64_t entry = 0; entry < entries; ++entry ) {

         // Get the current entry:
         event.getEntry( entry );

         // Print some additional info for the first 10 events:
         if( entry < 10 ) {
            ::Info( APP_NAME, "Processing entry %i", static_cast< int >( entry ) );

            // Get some information out of the tested tool:
            ::Info( APP_NAME, "SMK: %i, L1PSK: %i, HLTPSK: %i",
                    static_cast< int >( configTool.masterKey() ),
                    static_cast< int >( configTool.lvl1PrescaleKey() ),
                    static_cast< int >( configTool.hltPrescaleKey() ) );
            ::Info( APP_NAME, "  Number of L1 items: %i",
                    static_cast< int >( configTool.ctpConfig()->menu().items().size() ) );
            ::Info( APP_NAME, "  Number of HLT chains: %i",
                    static_cast< int >( configTool.chains().size() ) );
            ::Info( APP_NAME, " L1 menu " );
	   configTool.ctpConfig()->print("", 100);
         }

         // Give some feedback of where we are:
         if( ! ( entry % 100 ) ) {
            ::Info( APP_NAME, "Processed %i/%i events",
                    static_cast< int >( entry ),
                    static_cast< int >( entries ) );
         }
      }
   }

   // Return gracefully:
   return 0;
}
