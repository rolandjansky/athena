/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tfilemerger_test.cxx 675811 2015-06-17 05:28:22Z krasznaa $

// System include(s):
#include <memory>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TError.h>
#include <TSystem.h>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/AuxInfoBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/TFileMerger.h"

/// Helper macro
#define R_CHECK( CONTEXT, EXP )                                 \
   do {                                                              \
      const xAOD::TReturnCode result = EXP;                          \
      if( ! result.isSuccess() ) {                                   \
         ::Error( CONTEXT, XAOD_MESSAGE( "Failed to execute: %s" ),  \
                  #EXP );                                            \
         return 1;                                                   \
      }                                                              \
   } while( false )

/// Function checking the payload of the merged file
xAOD::TReturnCode checkMergedFile( const std::string& fileName,
                                   xAOD::TFileMerger::EMergeMode mode );
/// Function checking just one auxiliary branch in the merged file
xAOD::TReturnCode checkMergedBranch( ::TTree& tree, const std::string& name );

int main() {

   // The name of the application:
   static const char* APP_NAME = "ut_xaodrootaccess_tfilemerger_test";

   // Initialise the environment:
   R_CHECK( APP_NAME, xAOD::Init() );

   // Create the first output file:
   {
      // Open the file:
      std::unique_ptr< ::TFile > ofile( ::TFile::Open( "test_file1.root",
                                                       "RECREATE" ) );
      if( ! ofile.get() ) {
         ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open test_file1.root" ) );
         return 1;
      }

      // Connect a TEvent object to it:
      xAOD::TEvent event;
      R_CHECK( APP_NAME, event.writeTo( ofile.get() ) );

      // Fill it with some events:
      for( Int_t i = 0; i < 10; ++i ) {

         // Create a dummy container:
         DataVector< SG::AuxElement > dv;
         std::unique_ptr< xAOD::AuxContainerBase >
            auxC( new xAOD::AuxContainerBase() );
         dv.setStore( auxC.get() );
         for( Int_t j = 0; j < i; ++j ) {
            dv.push_back( new SG::AuxElement() );
            dv.back()->auxdata< float >( "FloatVar" ) =
               static_cast< float >( i + j );
            dv.back()->auxdata< int >( "IntVar" ) = i + j;
         }
         R_CHECK( APP_NAME, event.record( std::move( auxC ),
                                          "ContainerAux." ) );

         // Create a dummy object:
         std::unique_ptr< SG::AuxElement > el( new SG::AuxElement() );
         std::unique_ptr< xAOD::AuxInfoBase > auxI( new xAOD::AuxInfoBase() );
         el->setStore( auxI.get() );
         el->auxdata< float >( "FloatVar" ) = static_cast< float >( i );
         el->auxdata< int >( "IntVar" ) = i;
         R_CHECK( APP_NAME, event.record( std::move( el ), "Info" ) );
         R_CHECK( APP_NAME, event.record( std::move( auxI ), "InfoAux." ) );

         // Fill the event:
         if( event.fill() < 0 ) {
            ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't fill entry %i into "
                                             "test_file1.root" ), i );
            return 1;
         }
      }

      // Finish writing to the file:
      R_CHECK( APP_NAME, event.finishWritingTo( ofile.get() ) );
   }

   // Create the second output file:
   {
      // Open the file:
      std::unique_ptr< ::TFile > ofile( ::TFile::Open( "test_file2.root",
                                                       "RECREATE" ) );
      if( ! ofile.get() ) {
         ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open test_file2.root" ) );
         return 1;
      }

      // Connect a TEvent object to it:
      xAOD::TEvent event;
      R_CHECK( APP_NAME, event.writeTo( ofile.get() ) );

      // Fill it with some events:
      for( Int_t i = 0; i < 10; ++i ) {

         // Create a dummy container:
         DataVector< SG::AuxElement > dv;
         std::unique_ptr< xAOD::AuxContainerBase >
            auxC( new xAOD::AuxContainerBase() );
         dv.setStore( auxC.get() );
         for( Int_t j = 0; j < i; ++j ) {
            dv.push_back( new SG::AuxElement() );
            dv.back()->auxdata< float >( "FloatVar" ) =
               static_cast< float >( i + j );
            dv.back()->auxdata< int >( "IntVar" ) = i + j;
            dv.back()->auxdata< int >( "IntVar2" ) = i + j + 1;
         }
         R_CHECK( APP_NAME, event.record( std::move( auxC ),
                                          "ContainerAux." ) );

         // Create a dummy object:
         std::unique_ptr< SG::AuxElement > el( new SG::AuxElement() );
         std::unique_ptr< xAOD::AuxInfoBase > auxI( new xAOD::AuxInfoBase() );
         el->setStore( auxI.get() );
         el->auxdata< float >( "FloatVar" ) = static_cast< float >( i );
         el->auxdata< int >( "IntVar" ) = i;
         el->auxdata< int >( "IntVar2" ) = i + 1;
         R_CHECK( APP_NAME, event.record( std::move( el ), "Info" ) );
         R_CHECK( APP_NAME, event.record( std::move( auxI ), "InfoAux." ) );

         // Fill the event:
         if( event.fill() < 0 ) {
            ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't fill entry %i into "
                                             "test_file2.root" ), i );
            return 1;
         }
      }

      // Finish writing to the file:
      R_CHECK( APP_NAME, event.finishWritingTo( ofile.get() ) );
   }

   // Exercise slow file merging:
   xAOD::TFileMerger merger1;
   R_CHECK( APP_NAME, merger1.setOutputFileName( "merged.root" ) );
   R_CHECK( APP_NAME, merger1.addFile( "test_file1.root" ) );
   R_CHECK( APP_NAME, merger1.addFile( "test_file2.root" ) );
   R_CHECK( APP_NAME, merger1.merge( xAOD::TFileMerger::kSlowMerge ) );

   // Now check the output:
   R_CHECK( APP_NAME, checkMergedFile( "merged.root",
                                       xAOD::TFileMerger::kSlowMerge ) );
   ::Info( APP_NAME, "Test 1 passed" );

   // Exercise slow file merging:
   xAOD::TFileMerger merger2;
   R_CHECK( APP_NAME, merger2.setOutputFileName( "merged.root" ) );
   R_CHECK( APP_NAME, merger2.addFile( "test_file2.root" ) );
   R_CHECK( APP_NAME, merger2.addFile( "test_file1.root" ) );
   R_CHECK( APP_NAME, merger2.merge( xAOD::TFileMerger::kSlowMerge ) );

   // Now check the output:
   R_CHECK( APP_NAME, checkMergedFile( "merged.root",
                                       xAOD::TFileMerger::kSlowMerge ) );
   ::Info( APP_NAME, "Test 2 passed" );

   // Exercise fast file merging:
   xAOD::TFileMerger merger3;
   R_CHECK( APP_NAME, merger3.setOutputFileName( "merged.root" ) );
   R_CHECK( APP_NAME, merger3.addFile( "test_file1.root" ) );
   R_CHECK( APP_NAME, merger3.addFile( "test_file2.root" ) );
   R_CHECK( APP_NAME, merger3.merge( xAOD::TFileMerger::kFastMerge ) );

   // Now check the output:
   R_CHECK( APP_NAME, checkMergedFile( "merged.root",
                                       xAOD::TFileMerger::kFastMerge ) );
   ::Info( APP_NAME, "Test 3 passed" );

   // Exercise fast file merging:
   xAOD::TFileMerger merger4;
   R_CHECK( APP_NAME, merger4.setOutputFileName( "merged.root" ) );
   R_CHECK( APP_NAME, merger4.addFile( "test_file2.root" ) );
   R_CHECK( APP_NAME, merger4.addFile( "test_file1.root" ) );
   R_CHECK( APP_NAME, merger4.merge( xAOD::TFileMerger::kFastMerge ) );

   // Now check the output:
   R_CHECK( APP_NAME, checkMergedFile( "merged.root",
                                       xAOD::TFileMerger::kFastMerge ) );
   ::Info( APP_NAME, "Test 4 passed" );

   // Finally, remove the temporary files:
   gSystem->Unlink( "merged.root" );
   gSystem->Unlink( "test_file1.root" );
   gSystem->Unlink( "test_file2.root" );

   // Return gracefully:
   return 0;
}

/// This function is used to check that the file merging produced a reasonable
/// looking output.
///
/// @param fileName The merged file's name to test
/// @param mode The mode with which the file was merged
/// @returns The usual <code>xAOD::TReturnCode</code> types
///
xAOD::TReturnCode checkMergedFile( const std::string& fileName,
                                   xAOD::TFileMerger::EMergeMode mode ) {

   // Open the file:
   std::unique_ptr< ::TFile > merged( ::TFile::Open( fileName.c_str(),
                                                     "READ" ) );
   if( ! merged.get() ) {
      ::Error( "checkMergedFile", XAOD_MESSAGE( "Couldn't open %s" ),
               fileName.c_str() );
      return xAOD::TReturnCode::kFailure;
   }

   // Access the TTree in it:
   ::TTree* tree = dynamic_cast< ::TTree* >( merged->Get( "CollectionTree" ) );
   if( ! tree ) {
      ::Error( "checkMergedFile",
               XAOD_MESSAGE( "Couldn't file \"CollectionTree\" in merged "
                             "file" ) );
      return xAOD::TReturnCode::kFailure;
   }

   // Check the "InfoAux." branches:
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "Info" ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "InfoAux." ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "InfoAuxDyn.IntVar" ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "InfoAuxDyn.IntVar2" ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "InfoAuxDyn.FloatVar" ) );

   // In slow merging mode those are the only ones to check...
   if( mode == xAOD::TFileMerger::kSlowMerge ) {
      return xAOD::TReturnCode::kSuccess;
   }

   // Check the "ContainerAux." branches:
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "ContainerAux." ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "ContainerAuxDyn.IntVar" ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "ContainerAuxDyn.IntVar2" ) );
   RETURN_CHECK( "checkMergedFile",
                 checkMergedBranch( *tree, "ContainerAuxDyn.FloatVar" ) );

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}

xAOD::TReturnCode checkMergedBranch( ::TTree& tree, const std::string& name ) {

   // Try to access the branch:
   ::TBranch* br = tree.GetBranch( name.c_str() );
   if( ! br ) {
      ::Error( "checkedMergedBranch",
               XAOD_MESSAGE( "Couldn't find branch \"%s\" in output file" ),
               name.c_str() );
      return xAOD::TReturnCode::kFailure;
   }

   // Make sure that it has the right number of entries:
   if( br->GetEntries() != 20 ) {
      ::Error( "checkedMergedBranch",
               XAOD_MESSAGE( "Number of entries in branch \"%s\" is: %i "
                             "(!=20)" ),
               name.c_str(), static_cast< int >( br->GetEntries() ) );
      return xAOD::TReturnCode::kFailure;
   }

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}
