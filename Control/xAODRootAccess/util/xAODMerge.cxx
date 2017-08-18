/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// $Id: xAODMerge.cxx 673572 2015-06-09 08:40:47Z krasznaa $
///
/// @brief Executable replacing "hadd" for merging xAOD files in standalone mode
///
/// This executable can be used outside of Athena to conveniently merge xAOD
/// files into a single output file. It is mainly meant to be used for merging
/// files on the grid, or on PROOF. But it can also be used for other simple
/// merging operations.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 673572 $
/// $Date: 2015-06-09 10:40:47 +0200 (Tue, 09 Jun 2015) $

// System include(s):
#include <cstring>
#include <vector>
#include <cstdlib>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TFileMerger.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/Init.h"

/// Helper macro to return proper error codes when failing
#define R_CHECK( EXP )                                                  \
   do {                                                                 \
      if( ! EXP.isSuccess() ) {                                         \
         ::Error( APP_NAME, XAOD_MESSAGE( "Failed to execute \"%s\"" ), \
                  #EXP );                                               \
         return 1;                                                      \
      }                                                                 \
   } while( 0 )

int main( int argc, char* argv[] ) {

   // The name of the application:
   const char* APP_NAME = argv[ 0 ];

   // Provide usage instructions if not enough options were given:
   if( ( argc < 3 ) ||
       ( ! strcmp( argv[ 1 ], "-h" ) ) ||
       ( ! strcmp( argv[ 1 ], "--help" ) ) ) {
      ::Info( APP_NAME, " " );
      ::Info( APP_NAME, "Usage: %s [options] outputFile inputFile1 "
              "[inputFile2 ...]",
              APP_NAME );
      ::Info( APP_NAME, " " );
      ::Info( APP_NAME, "  Options:" );
      ::Info( APP_NAME, "    -s Use slow xAOD tree merging" );
      ::Info( APP_NAME, "    -b Use branch-access for slow and metadata "
              "merging" );
      ::Info( APP_NAME, "    -v NUMBER Use a specific verbosity setting" );
      ::Info( APP_NAME, "    -m Metadata tool type to use during the merge" );
      ::Info( APP_NAME, "    -e NUMBER Number of events to merge in slow "
              "merging mode" );
      ::Info( APP_NAME, " " );
      return 1;
   }

   // Initialise the environment:
   R_CHECK( xAOD::Init( APP_NAME ) );

   // Decode the command line options:
   bool useSlowMerge = false;
   xAOD::TEvent::EAuxMode mode = xAOD::TEvent::kClassAccess;
   const char* outputName = 0;
   std::vector< const char* > inputNames;
   int verbosity = 0;
   std::vector< const char* > metaDataTools;
   ::Long64_t entries = xAOD::TFileMerger::kBigNumber;
   for( int i = 1; i < argc; ++i ) {
      if( ! strcmp( argv[ i ], "-s" ) ) {
         useSlowMerge = true;
      } else if( ! strcmp( argv[ i ], "-b" ) ) {
         mode = xAOD::TEvent::kBranchAccess;
      } else if( ! strcmp( argv[ i ], "-v" ) ) {
         if( i + 1 >= argc ) {
            ::Error( APP_NAME, "No verbosity level was provided after -v" );
         } else {
            const long request = strtol( argv[ i + 1 ], 0, 10 );
            if( ( request < kMaxLong ) && ( request >= 0 ) ) {
               verbosity = static_cast< int >( request );
               ++i;
            } else {
               ::Error( APP_NAME, "Could not parse the verbosity level passed "
                        "after -v: %s", argv[ i + 1 ] );
               ::Error( APP_NAME, "Will use the default value (0)" );
            }
         }
      } else if( ! strcmp( argv[ i ], "-m" ) ) {
         if( i + 1 >= argc ) {
            ::Error( APP_NAME, "No tool type name provided after -m" );
         } else {
            metaDataTools.push_back( argv[ i + 1 ] );
            ++i;
         }
      } else if( ! strcmp( argv[ i ], "-e" ) ) {
         if( i + 1 >= argc ) {
            ::Error( APP_NAME, "No verbosity level was provided after -e" );
         } else {
            const long request = strtol( argv[ i + 1 ], 0, 10 );
            if( ( request < kMaxLong ) && ( request >= 0 ) ) {
               entries = static_cast< ::Long64_t >( request );
               ++i;
            } else {
               ::Error( APP_NAME, "Could not parse the number of entries "
                        "passed after -e: %s", argv[ i + 1 ] );
               ::Error( APP_NAME, "Will use the default value (%i)",
                        static_cast< int >( xAOD::TFileMerger::kBigNumber ) );
            }
         }
      } else if( ! outputName ) {
         outputName = argv[ i ];
      } else {
         inputNames.push_back( argv[ i ] );
      }
   }

   // Set up the file merger object:
   xAOD::TFileMerger merger;
   merger.setAccessMode( mode );
   merger.setVerbosity( verbosity );
   R_CHECK( merger.setOutputFileName( outputName ) );
   for( auto name : inputNames ) {
      R_CHECK( merger.addFile( name ) );
   }
   for( auto typeName : metaDataTools ) {
      R_CHECK( merger.addMetaDataTool( typeName ) );
   }

   // Execute the merge:
   R_CHECK( merger.merge( useSlowMerge ? xAOD::TFileMerger::kSlowMerge :
                          xAOD::TFileMerger::kFastMerge, entries ) );

   return 0;
}
