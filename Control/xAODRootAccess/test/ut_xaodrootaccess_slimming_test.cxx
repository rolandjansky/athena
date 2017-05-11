// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_slimming_test.cxx 649835 2015-02-26 08:19:01Z krasznaa $

// System include(s):
#include <memory>
#include <set>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include <TSystem.h>

// Core EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"

/// A little helper macro
#define SIMPLE_ASSERT( EXP )                                                \
   do {                                                                     \
      if( ! ( EXP ) ) {                                                     \
         ::Error( APP_NAME, XAOD_MESSAGE( "Error evaluating: %s" ), #EXP ); \
         return 1;                                                          \
      }                                                                     \
   } while( 0 )

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_slimming_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create the first tested object(s):
   xAOD::TEvent event1( xAOD::TEvent::kBranchAccess );

   // Set up slimming rules for some containers:
   event1.setAuxItemList( "ElectronCollectionAux.", "eta.phi.pt.m" );
   event1.setAuxItemList( "MuonsAux.", "eta.phi" );

   // Connect an input file to the event:
   static const char* FNAME =
      "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
      "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD."
      "e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
      "AOD.01597980._000098.pool.root.1";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
               FNAME );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event1.readFrom( ifile.get() ) );

   // Open a dummy output file, used in the slimming test:
   static const char* OFNAME = "dummy1.root";
   std::unique_ptr< ::TFile > ofile1( ::TFile::Open( OFNAME, "RECREATE" ) );
   if( ! ofile1.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Output file %s could not be opened" ),
               OFNAME );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event1.writeTo( ofile1.get() ) );

   // Read in the first event:
   if( event1.getEntry( 0 ) < 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't load entry 0 from file %s" ),
               FNAME );
      return 1;
   }

   // Copy over the containers:
   RETURN_CHECK( APP_NAME, event1.copy( "ElectronCollection" ) );
   RETURN_CHECK( APP_NAME, event1.copy( "Muons" ) );

   // Write out this first event:
   if( event1.fill() <= 0 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Failed to write event to output" ) );
      return 1;
   }

   // Close the output file:
   RETURN_CHECK( APP_NAME, event1.finishWritingTo( ofile1.get() ) );
   ofile1->Close();

   // Now check that it has exactly the branches that it should:
   std::unique_ptr< ::TFile > testFile1( ::TFile::Open( OFNAME, "READ" ) );
   if( ! testFile1.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't open file %s" ), OFNAME );
      return 1;
   }
   ::TTree* tree =
      dynamic_cast< ::TTree* >( testFile1->Get( "CollectionTree" ) );
   if( ! tree ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access CollectionTree" ) );
      return 1;
   }
   ::TObjArray* branches = tree->GetListOfBranches();
   const std::set< std::string > expectedBranches1{
      "ElectronCollection", "ElectronCollectionAuxDyn.pt",
      "ElectronCollectionAuxDyn.eta", "ElectronCollectionAuxDyn.phi",
      "ElectronCollectionAuxDyn.m",
      "Muons", "MuonsAuxDyn.eta", "MuonsAuxDyn.phi" };
   if( static_cast< size_t >( branches->GetEntries() ) !=
       expectedBranches1.size() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Wrong number (%i) of branches seen in "
                                       "the output" ), branches->GetEntries() );
      return 1;
   }
   std::set< std::string > seenBranches1;
   for( ::Int_t i = 0; i < branches->GetEntries(); ++i ) {
      seenBranches1.insert( branches->At( i )->GetName() );
   }
   if( seenBranches1 != expectedBranches1 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Wrong set of branches seen in the "
                                       "output" ) );
      return 1;
   }

   // Close the test file:
   testFile1->Close();

   // Remove the first test file:
   gSystem->Unlink( OFNAME );

   // Let the user know what happened so far:
   ::Info( APP_NAME, "Slimming in xAOD::TEvent::copy(...) seems correct" );

   // Create another event object to test slimming on newly created objects:
   xAOD::TEvent event2;

   // Configure what to write:
   event2.setAuxItemList( "TestAux.", "foo.bar" );

   // Connect the event to an output file:
   std::unique_ptr< ::TFile > ofile2( ::TFile::Open( OFNAME, "RECREATE" ) );
   if( ! ofile2.get() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Output file %s could not be opened" ),
              OFNAME );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event2.writeTo( ofile2.get() ) );

   // Create a generic container in the output. Notice that we only record the
   // auxiliary store, and not the DV. That's mainly because we don't have a
   // proper collection proxy for this type.
   DataVector< SG::AuxElement > dv;
   SG::IAuxStore* aux = event2.recordAux( "TestAux." );
   if( ! aux ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't create auxiliary store" ) );
      return 1;
   }
   dv.setStore( aux );

   // Add an object to it, and set a bunch of variables on it:
   SG::AuxElement* el = new SG::AuxElement();
   dv.push_back( el );
   el->auxdata< int >( "foo" ) = 2;
   el->auxdata< float >( "bar" ) = 3.14;
   el->auxdata< unsigned long long >( "dummy1" ) = 325;
   el->auxdata< unsigned char >( "dummy2" ) = 6;
   el->auxdata< double >( "dummy3" ) = 2.123456;

   // Make sure that the variables can be read out, so they are sitting pretty
   // in memory somewhere:
   SIMPLE_ASSERT( el->auxdataConst< int >( "foo" ) == 2 );
   SIMPLE_ASSERT( el->auxdataConst< unsigned long long >( "dummy1" ) == 325 );
   SIMPLE_ASSERT( el->auxdataConst< unsigned char >( "dummy2" ) == 6 );

   // Close the output file:
   RETURN_CHECK( APP_NAME, event2.finishWritingTo( ofile2.get() ) );
   ofile2->Close();

   // Check the contents of the produced file:
   std::unique_ptr< ::TFile > testFile2( ::TFile::Open( OFNAME, "READ" ) );
   if( ! testFile2.get() ) {
      ::Error( APP_NAME, "Couldn't open file: %s", OFNAME );
      return 1;
   }
   tree = dynamic_cast< ::TTree* >( testFile2->Get( "CollectionTree" ) );
   if( ! tree ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Couldn't access CollectionTree" ) );
      return 1;
   }
   const std::set< std::string > expectedBranches2{ "TestAuxDyn.foo",
                                                    "TestAuxDyn.bar" };
   branches = tree->GetListOfBranches();
   if( static_cast< size_t >( branches->GetEntries() ) !=
       expectedBranches2.size() ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Wrong number (%i) of branches seen in "
                                       "the output" ), branches->GetEntries() );
      return 1;
   }
   std::set< std::string > seenBranches2;
   for( ::Int_t i = 0; i < branches->GetEntries(); ++i ) {
      seenBranches2.insert( branches->At( i )->GetName() );
   }
   if( seenBranches2 != expectedBranches2 ) {
      ::Error( APP_NAME, XAOD_MESSAGE( "Wrong set of branches seen in the "
                                       "output" ) );
      return 1;
   }

   // Close the test file:
   testFile2->Close();

   // Remove the second test file:
   gSystem->Unlink( OFNAME );

   // Let the user know what happened so far:
   ::Info( APP_NAME, "Slimming in xAOD::TEvent::recordAux(...) seems correct" );

   // Return gracefully:
   return 0;
}
