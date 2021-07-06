/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

// EDM include(s).
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "SGTools/DataProxy.h"

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
   const char* APP_NAME = "ut_xaodrootaccess_proxydict_test";

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
   xAOD::TEvent event;
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

   // Access the IProxyDict interface of the event object.
   IProxyDict& pdict = event;

   // The auxiliary variable to access later on. Tried to select something
   // that should be a dynamic variable in any type of input file.
   static const SG::auxid_t auxid =
      SG::AuxTypeRegistry::instance().getAuxID< float >( "ptvarcone20" );
   // The clid for the auxiliary stores.
   static const CLID auxClid = ClassID_traits< SG::IConstAuxStore >::ID();

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

      // Read some auxiliary stores through the IProxyDict interface.
      SG::DataProxy* proxy = pdict.proxy( auxClid, "ElectronsAux." );
      SIMPLE_ASSERT( proxy != nullptr );
      const SG::IConstAuxStore* auxPtr =
         SG::DataProxy_cast< SG::IConstAuxStore >( proxy );
      SIMPLE_ASSERT( auxPtr != nullptr );
      SIMPLE_ASSERT( auxPtr->getAuxIDs().count( auxid ) == 1 );

      proxy = pdict.proxy( auxClid, "MuonsAux." );
      SIMPLE_ASSERT( proxy != nullptr );
      auxPtr = SG::DataProxy_cast< SG::IConstAuxStore >( proxy );
      SIMPLE_ASSERT( auxPtr != nullptr );
      SIMPLE_ASSERT( auxPtr->getAuxIDs().count( auxid ) == 1 );
   }

   // Return gracefully.
   return 0;
}
