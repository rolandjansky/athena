/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODFileReadTest.cxx 778334 2016-10-13 17:31:15Z krasznaa $

// System include(s):
#include <cstring>
#include <memory>
#include <vector>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TClass.h>

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

/// The name of the application
static const char* APP_NAME = "xAODFileReadTest";

/// Helper macro
#define R_CHECK( EXP )                                                     \
   do {                                                                    \
      const auto result = EXP;                                             \
      if( ! result.isSuccess() ) {                                         \
         Error( APP_NAME, XAOD_MESSAGE( "Failed to execute: %s" ), #EXP ); \
         return 1;                                                         \
      }                                                                    \
   } while( 0 )

/// Class used for the test
class TEventClass : public xAOD::TEvent {
public:
   /// Constructor
   TEventClass( xAOD::TEvent::EAuxMode mode ) : xAOD::TEvent( mode ) {}

   /// Function loading all interface objects of the event
   xAOD::TReturnCode loadInputObjects() {
      // Get the event format object:
      const xAOD::EventFormat* ef = this->inputEventFormat();
      if( ! ef ) {
         return xAOD::TReturnCode::kFailure;
      }
      // Loop over the objects of the file:
      for( auto ef_itr : *ef ) {
         // A helper object:
         const xAOD::EventFormatElement& efe = ef_itr.second;
         // Skip auxiliary objects:
         if( efe.branchName().rfind( "Aux." ) ==
             ( efe.branchName().size() - 4 ) ) {
            continue;
         }
         // And dynamic variables:
         if( efe.parentName() != "" ) {
            continue;
         }
         // Skip auxiliary stores with non-standard names:
         ::TClass* cl = ::TClass::GetClass( efe.className().c_str(), kTRUE,
                                            kTRUE );
         if( ! cl ) continue;
         if( cl->InheritsFrom( "SG::IConstAuxStore" ) ) {
            continue;
         }
         // Get the type of the branch:
         const std::type_info* ti = cl->GetTypeInfo();
         if( ! ti ) {
            ::Warning( "TEventClass::loadInputObjects",
                       "Couldn't find std::type_info object for type %s "
                       "(key=%s)", cl->GetName(), efe.branchName().c_str() );
            continue;
         }
         // Check if the branch exists in the file:
         if( ! this->contains( efe.branchName(), *ti ) ) {
            continue;
         }
         // Try to load the object/container:
         if( ! this->getInputObject( efe.branchName(), *ti ) ) {
            Error( "TEventClass::loadInputObjects",
                   XAOD_MESSAGE( "Couldnt load object: %s" ),
                   efe.branchName().c_str() );
            return xAOD::TReturnCode::kFailure;
         }
      }
      // Return gracefully:
      return xAOD::TReturnCode::kSuccess;
   }
}; // class TEventClass

int main( int argc, char* argv[] ) {

   // Initialise the environment:
   R_CHECK( xAOD::Init() );

   // Provide usage instructions if not enough options were given:
   if( ( argc < 2 ) ||
       ( ! strcmp( argv[ 1 ], "-h" ) ) ||
       ( ! strcmp( argv[ 1 ], "--help" ) ) ) {
      Info( APP_NAME, "Usage: %s [options] <file1> [file2]...", APP_NAME );
      Info( APP_NAME, "Options:" );
      Info( APP_NAME, "   -m <access mode>" );
      Info( APP_NAME, "      0: Branch access" );
      Info( APP_NAME, "      1: Class access (default)" );
      Info( APP_NAME, "      2: Athena access" );
      return 1;
   }

   // Decode the command line options:
   std::vector< std::string > fileNames;
   xAOD::TEvent::EAuxMode auxMode = xAOD::TEvent::kClassAccess;
   for( int i = 1; i < argc; ++i ) {
      if( ! strcmp( argv[ i ], "-m" ) ) {
         if( i + 1 >= argc ) {
            ::Error( APP_NAME,
                     XAOD_MESSAGE( "No access mode specified after -m" ) );
            return 1;
         } else {
            const long mode = ::strtol( argv[ i + 1 ], 0, 10 );
            auxMode = static_cast< xAOD::TEvent::EAuxMode >( mode );
            ++i;
         }
      } else {
         fileNames.push_back( argv[ i ] );
      }
   }

   // Create the "TEvent" object used for the test:
   TEventClass event( auxMode );

   // Loop over the specified input files:
   for( const std::string& fileName : fileNames ) {

      // Open the file:
      std::unique_ptr< TFile > ifile( TFile::Open( fileName.c_str(), "READ" ) );
      if( ( ! ifile.get() ) || ifile->IsZombie() ) {
         Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file: %s" ),
                fileName.c_str() );
         return 1;
      }
      Info( APP_NAME, "Opened file: %s", fileName.c_str() );

      // Give it to TEvent:
      R_CHECK( event.readFrom( ifile.get() ) );

      // Loop over the file's events:
      const Long64_t entries = event.getEntries();
      for( Long64_t entry = 0; entry < entries; ++entry ) {

         // Load the event:
         if( event.getEntry( entry ) < 0 ) {
            Error( APP_NAME,
                   XAOD_MESSAGE( "Failed to load entry %i from file: %s" ),
                   static_cast< int >( entry ), fileName.c_str() );
            return 1;
         }

         // Load the event:
         R_CHECK( event.loadInputObjects() );

         // Tell the user where we are:
         if( ! ( entry % 1000 ) ) {
            Info( APP_NAME, "===>>> Loaded entry %i / %i <<<===",
                  static_cast< int >( entry ), static_cast< int >( entries ) );
         }
      }
   }

   // Return gracefully:
   return 0;
}
