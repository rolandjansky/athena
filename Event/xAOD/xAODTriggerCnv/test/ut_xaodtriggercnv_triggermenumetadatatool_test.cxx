/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// $Id: ut_xaodtriggercnv_triggermenumetadatatool_test.cxx 683395 2015-07-16 11:11:56Z krasznaa $
///
/// @file Basic tests for xAODMaker::TriggerMenuMetaDataTool outside of Athena
///
/// This unit test tries to exercise the metadata tool in a few different setups
/// to make sure that it works as expected.
///

// System include(s):
#include <memory>
#include <set>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TChain.h>
#include <TError.h>
#include <TSystem.h>

// xAOD include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"

// EDM include(s):
#include "xAODTrigger/TrigConfKeys.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"

// Local include(s):
#include "xAODTriggerCnv/TriggerMenuMetaDataTool.h"

/// Helper macro for checking return codes
#define R_CHECK( EXP )                                                       \
   do {                                                                      \
      if( ! EXP.isSuccess() ) {                                              \
         ::Error( APP_NAME, XAOD_MESSAGE( "Failed to execute: %s" ), #EXP ); \
         return 1;                                                           \
      }                                                                      \
   } while( 0 )

/// Helper macro for making assertions
#define R_ASSERT( CONTEXT, EXP )                                             \
   do {                                                                      \
      const bool result = EXP;                                               \
      if( ! result ) {                                                       \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to evaluate: %s" ), #EXP ); \
         return xAOD::TReturnCode::kFailure;                                 \
      }                                                                      \
   } while( 0 )

/// Function writing files to test xAOD::TriggerMenuMetaDataTool with
///
/// @param fname   The name of the file to write
/// @param smks    The super master keys to put into the file
/// @param l1psks  The LVL1 prescale keys to put into the file
/// @param hltpsks The HLT prescale keys to put into the file
/// @returns The usual <code>xAOD::TReturnCode</code> types
///
xAOD::TReturnCode writeTestFile( const std::string& fname,
                                 const std::vector< uint32_t >& smks,
                                 const std::vector< uint32_t >& l1psks,
                                 const std::vector< uint32_t >& hltpsks ) {

   // Some sanity checks:
   R_ASSERT( "writeTestFile", smks.size() > 1 );
   R_ASSERT( "writeTestFile", smks.size() == l1psks.size() );
   R_ASSERT( "writeTestFile", smks.size() == hltpsks.size() );

   // Set up the writing of a file:
   xAOD::TEvent event;
   std::unique_ptr< ::TFile > ofile( ::TFile::Open( fname.c_str(),
                                                    "RECREATE" ) );
   if( ! ofile.get() ) {
      ::Error( "writeTestFile", XAOD_MESSAGE( "Couldn't open output file: %s" ),
               fname.c_str() );
      return xAOD::TReturnCode::kFailure;
   }
   RETURN_CHECK( "writeTestFile", event.writeTo( ofile.get() ) );

   // Write some dummy event level info:
   for( Int_t i = 0; i < static_cast< Int_t>( smks.size() - 1 ); ++ i ) {
      for( Int_t j = 0; j < 10; ++j ) {
         // Create the simple object:
         std::unique_ptr< xAOD::TrigConfKeys >
            keys( new xAOD::TrigConfKeys( smks[ i ], l1psks[ i ],
                                          hltpsks[ i ] ) );
         RETURN_CHECK( "writeTestFile", event.record( std::move( keys ),
                                                      "TrigConfKeys" ) );
         // And save the event:
         if( event.fill() <= 0 ) {
            ::Error( "writeTestFile",
                     XAOD_MESSAGE( "Write error for i = %i, j = %i" ),
                     i, j );
            return xAOD::TReturnCode::kFailure;
         }
      }
   }

   // Now construct the metadata for the file:
   std::unique_ptr< xAOD::TriggerMenuContainer >
      menu( new xAOD::TriggerMenuContainer() );
   std::unique_ptr< xAOD::TriggerMenuAuxContainer >
      menuAux( new xAOD::TriggerMenuAuxContainer() );
   menu->setStore( menuAux.get() );
   for( size_t i = 0; i < smks.size(); ++i ) {
      xAOD::TriggerMenu* m = new xAOD::TriggerMenu();
      menu->push_back( m );
      m->setSMK( smks[ i ] );
      m->setL1psk( l1psks[ i ] );
      m->setHLTpsk( hltpsks[ i ] );
   }

   // And record it:
   RETURN_CHECK( "writeTestFile", event.recordMeta( std::move( menu ),
                                                    "TriggerMenu" ) );
   RETURN_CHECK( "writeTestFile", event.recordMeta( std::move( menuAux ),
                                                    "TriggerMenuAux." ) );

   // Close the output file:
   RETURN_CHECK( "writeTestFile", event.finishWritingTo( ofile.get() ) );

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}

/// Type for a trigger key object that can be used in the checks
typedef std::pair< uint32_t, std::pair< uint32_t, uint32_t > > TrigKey_t;

/// Helper function providing trigger key objects for comparison
TrigKey_t makeKey( uint32_t smk, uint32_t l1psk, uint32_t hltpsk ) {

   return TrigKey_t( smk, std::pair< uint32_t, uint32_t >( l1psk, hltpsk ) );
}

/// Function used to check the content of the merged file
///
/// @param fname The name of the file to test
/// @returns The usual <code>xAOD::TReturnCode</code> types
///
xAOD::TReturnCode checkMergedFile( const std::string& fname ) {

   // Set up the reading of the file:
   xAOD::TEvent event;
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( fname.c_str(), "READ" ) );
   if( ! ifile.get() ) {
      ::Error( "checkMergedFile", XAOD_MESSAGE( "Couldn't open file: %s" ),
               fname.c_str() );
      return xAOD::TReturnCode::kFailure;
   }
   RETURN_CHECK( "checkMergedFile", event.readFrom( ifile.get() ) );

   // Retrieve the metadata object:
   const xAOD::TriggerMenuContainer* menu = 0;
   RETURN_CHECK( "checkMergedFile", event.retrieveMetaInput( menu,
                                                             "TriggerMenu" ) );

   // Check that it has the right properties:
   typedef std::set< TrigKey_t > TrigKeySet_t;
   const TrigKeySet_t reference( { makeKey( 1, 1, 1 ), makeKey( 2, 2, 2 ),
                                   makeKey( 3, 3, 3 ), makeKey( 3, 4, 3 ),
                                   makeKey( 4, 5, 6 ) } );
   TrigKeySet_t inFile;
   for( const xAOD::TriggerMenu* m : *menu ) {
      inFile.insert( makeKey( m->smk(), m->l1psk(), m->hltpsk() ) );
   }
   R_ASSERT( "checkMergedFile", reference == inFile );

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}

int main() {

   // The application's name:
   static const char* APP_NAME =
      "ut_xaodtriggercnv_triggermenumetadatatool_test";

   // Initialise the environment:
   R_CHECK( xAOD::Init() );

   //
   // Set up two simple files with some dummy event and metadata information.
   //
   R_CHECK( writeTestFile( "trigMenuFile1.root",
                           std::vector< uint32_t >( { 1, 2, 3 } ),
                           std::vector< uint32_t >( { 1, 2, 3 } ),
                           std::vector< uint32_t >( { 1, 2, 3 } ) ) );
   R_CHECK( writeTestFile( "trigMenuFile2.root",
                           std::vector< uint32_t >( { 2, 3, 4 } ),
                           std::vector< uint32_t >( { 2, 4, 5 } ),
                           std::vector< uint32_t >( { 2, 3, 6 } ) ) );

   //
   // Check the merging in the most ideal setup possible:
   //
   {
      // Set up a chain with the input files:
      ::TChain chain( "CollectionTree" );
      chain.Add( "trigMenuFile1.root" );
      chain.Add( "trigMenuFile2.root" );

      // Set up the event object:
      xAOD::TEvent event;

      // Set up the metadata tool:
      xAODMaker::TriggerMenuMetaDataTool tool( "MergeTool1" );
      R_CHECK( tool.setProperty( "OutputLevel", MSG::VERBOSE ) );
      R_CHECK( tool.initialize() );

      // Start reading from the input files:
      R_CHECK( event.readFrom( &chain ) );

      // Start writing an output file:
      std::unique_ptr< ::TFile >
         ofile( ::TFile::Open( "trigMenuFileMerged.root", "RECREATE" ) );
      if( ! ofile.get() ) {
         ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open merged file" ) );
         return 1;
      }
      R_CHECK( event.writeTo( ofile.get() ) );

      // Copy over all events to the output:
      const ::Long64_t entries = event.getEntries();
      for( ::Long64_t entry = 0; entry < entries; ++entry ) {
         if( event.getEntry( entry ) < 0 ) {
            ::Error( APP_NAME, XAOD_MESSAGE( "Failed to load entry %i" ),
                     static_cast< int >( entry ) );
            return 1;
         }
         R_CHECK( event.copy( "TrigConfKeys" ) );
      }

      // Close the output file:
      R_CHECK( event.finishWritingTo( ofile.get() ) );
      ofile->Close();

      // Test that the output file is healthy:
      R_CHECK( checkMergedFile( "trigMenuFileMerged.root" ) );

      // Remove the merged file:
      gSystem->Unlink( "trigMenuFileMerged.root" );
   }

   //
   // Check the merging in the way that it is done inside EventLoop jobs:
   //
   {
      // Names of the input files:
      const std::vector< std::string > fnames( { "trigMenuFile1.root",
                                                 "trigMenuFile2.root" } );

      // Set up the event object:
      xAOD::TEvent event;

      // Start writing an output file:
      std::unique_ptr< ::TFile >
      ofile( ::TFile::Open( "trigMenuFileMerged.root", "RECREATE" ) );
      if( ! ofile.get() ) {
         ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open merged file" ) );
         return 1;
      }
      R_CHECK( event.writeTo( ofile.get() ) );

      // The merging tool:
      std::unique_ptr< xAODMaker::TriggerMenuMetaDataTool > tool;

      // Loop over the files:
      for( const std::string& fname : fnames ) {

         // Set up the reading from the file:
         std::unique_ptr< ::TFile > ifile( ::TFile::Open( fname.c_str(),
                                                          "READ" ) );
         if( ! ifile.get() ) {
            ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file: %s" ),
                     fname.c_str() );
            return 1;
         }
         R_CHECK( event.readFrom( ifile.get() ) );

         // Create the tool if it doesn't exist yet:
         if( ! tool.get() ) {
            tool.reset( new xAODMaker::TriggerMenuMetaDataTool( "MergeTool2" ) );
            R_CHECK( tool->setProperty( "OutputLevel", MSG::VERBOSE ) );
            R_CHECK( tool->initialize() );
         }

         // Copy over all events to the output:
         const ::Long64_t entries = event.getEntries();
         for( ::Long64_t entry = 0; entry < entries; ++entry ) {
            if( event.getEntry( entry ) < 0 ) {
               ::Error( APP_NAME, XAOD_MESSAGE( "Failed to load entry %i" ),
                        static_cast< int >( entry ) );
               return 1;
            }
            R_CHECK( event.copy( "TrigConfKeys" ) );
            if( event.fill() <= 0 ) {
               ::Error( APP_NAME, XAOD_MESSAGE( "Failed to write entry: %i" ),
                        static_cast< int >( entry ) );
            }
         }
      }

      // Close the output file:
      R_CHECK( event.finishWritingTo( ofile.get() ) );
      ofile->Close();

      // Test that the output file is healthy:
      R_CHECK( checkMergedFile( "trigMenuFileMerged.root" ) );

      // Remove the merged file:
      gSystem->Unlink( "trigMenuFileMerged.root" );
   }

   // Remove the test files:
   gSystem->Unlink( "trigMenuFile1.root" );
   gSystem->Unlink( "trigMenuFile2.root" );

   // Return gracefully:
   return 0;
}
