/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TChain.h>
#include <TError.h>
#include <TTree.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TSystem.h>

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/MakeTransientTree.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"

int main() {

   // The name of the application:
   static const char* APP_NAME = "ut_xaodrootaccess_transtree_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Open it using a TFile:
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( "$ASG_TEST_FILE_MC",
                                                    "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file: %s" ),
               gSystem->Getenv( "ASG_TEST_FILE_MC" ) );
      return 1;
   }

   // Create a transient event tree using the TFile:
   ::TTree* tree = xAOD::MakeTransientTree( ifile.get() );
   if( ! tree ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Couldn't create transient tree from file: %s" ),
               gSystem->Getenv( "ASG_TEST_FILE_MC" ) );
      return 1;
   }

   // Make a test plot:
   tree->Draw( "Electrons.eta()-"
               "Electrons.trackParticle().eta()>>dummyHist1" );
   ::TH1* dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist1" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist1\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );

   // Clean up the event tree memory:
   xAOD::ClearTransientTrees();

   // Set up a chain with this one file:
   ::TChain eventChain( "CollectionTree" );
   eventChain.Add( "$ASG_TEST_FILE_MC" );

   // Create a transient tree using it:
   tree = xAOD::MakeTransientTree( &eventChain );
   if( ! tree ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Couldn't create transient tree from TChain "
                             "input" ) );
      return 1;
   }

   // Make a test plot:
   tree->Draw( "Electrons.eta()-"
               "Electrons.trackParticle().eta()>>dummyHist2" );
   dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist2" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist2\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );

   // Clean up the event tree memory:
   xAOD::ClearTransientTrees();

   // Create a transient metadata tree using the file:
   tree = xAOD::MakeTransientMetaTree( ifile.get() );
   if( ! tree ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Couldn't create transient metadata tree from "
                             "file: %s" ),
               gSystem->Getenv( "ASG_TEST_FILE_MC" ) );
      return 1;
   }

   // Make a test plot:
   tree->Draw( "TriggerMenu.smk()>>dummyHist3" );
   dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist3" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist3\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );

   // Set up a chain with this one file:
   ::TChain metaChain( "MetaData" );
   metaChain.Add( "$ASG_TEST_FILE_MC" );

   // Create a transient tree using it:
   tree = xAOD::MakeTransientMetaTree( &metaChain );
   if( ! tree ) {
      ::Error( APP_NAME,
               XAOD_MESSAGE( "Couldn't create transient metadata tree from "
                             "TChain input" ) );
      return 1;
   }

   // Make a test plot:
   tree->Draw( "TriggerMenu.l1psk()>>dummyHist4" );
   dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist4" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist4\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );

   // Clean up the metadata tree memory:
   xAOD::ClearTransientTrees();

   // Create both transient trees at once:
   xAOD::TTransTrees tt = xAOD::MakeTransientTrees( ifile.get() );

   // And make some test plots:
   tt.eventTree()->Draw( "Electrons.eta()-"
                         "Electrons.trackParticle().eta()"
                         ">>dummyHist5" );
   dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist5" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist5\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );
   tt.metaTree()->Draw( "TriggerMenu.hltpsk()>>dummyHist6" );
   dummyHist = dynamic_cast< ::TH1* >( gDirectory->Get( "dummyHist6" ) );
   if( ! dummyHist ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access \"dummyHist6\"" ) );
      return 1;
   }
   //dummyHist->Print( "all" );

   // Skip the final explicit cleanup. To test that cleanup happens correctly
   // when it runs after ROOT's own cleanup.

   // Return gracefully:
   return 0;
}
