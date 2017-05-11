// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tchain_test.cxx 691099 2015-08-25 08:19:08Z krasznaa $

// ROOT include(s):
#include <TChain.h>
#include <TError.h>

// EDM that the package uses anyway:
#include "AthContainers/AuxVectorBase.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Utils.h"

/// Type used in the event/store test
class ClassA {
   
public:
   int m_var1;
   float m_var2;
   
}; // class ClassA

/// Type used in the event/store test
class ClassB : public ClassA {
   
public:
   int m_var3;
   float m_var4;
   
}; // class ClassB

/// Helper function, "processing" a TChain
xAOD::TReturnCode process( xAOD::TEvent& event, xAOD::TStore& store );

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_tchain_test";
   
   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
   
   // Create the tested object(s):
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   xAOD::TStore store;

   // Set up a TChain with some mc14_8TeV input files:
   ::TChain chain1( "CollectionTree" );
   chain1.Add( "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/"
               "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon."
               "AOD.e1727_s1933_s1911_r5591/AOD.01494881._105458.pool.root.1" );
   chain1.Add( "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/"
               "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon."
               "AOD.e1727_s1933_s1911_r5591/AOD.01494881._107292.pool.root.1" );

   // Connect the TEvent object to it:
   RETURN_CHECK( APP_NAME, event.readFrom( &chain1 ) );

   // Run the processing:
   ::Info( APP_NAME, "Processing mc14_8TeV chain..." );
   RETURN_CHECK( APP_NAME, process( event, store ) );

   // Set up a TChain with some mc14_8TeV input files:
   ::TChain chain2( "CollectionTree" );
   chain2.Add( "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
               "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge."
               "AOD.e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
               "AOD.01597980._000098.pool.root.1" );
   chain2.Add( "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/"
               "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge."
               "AOD.e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
               "AOD.01597980._000420.pool.root.1" );

   // Connect the TEvent object to it:
   RETURN_CHECK( APP_NAME, event.readFrom( &chain2 ) );

   // Run the processing:
   ::Info( APP_NAME, "Processing mc14_13TeV chain..." );
   RETURN_CHECK( APP_NAME, process( event, store ) );

   // Return gracefully:
   return 0;
}

xAOD::TReturnCode process( xAOD::TEvent& event, xAOD::TStore& store ) {

   // Loop over all events:
   const ::Long64_t entries = event.getEntries();
   for( ::Long64_t entry = 0; entry < entries; ++entry ) {

      // Clear the store:
      store.clear();

      // Read in the event:
      if( event.getEntry( entry ) < 0 ) {
         ::Error( "process", "Couldn't load entry %i",
                  static_cast< int >( entry ) );
         return xAOD::TReturnCode::kFailure;
      }
      if( ! ( entry % 100 ) ) {
         ::Info( "process", "Processed %i / %i events",
                 static_cast< int >( entry ),
                 static_cast< int >( entries ) );
      }

      // Try to retrieve some objects:
      const xAOD::AuxContainerBase* c = 0;
      RETURN_CHECK( "process", event.retrieve( c, "ElectronCollectionAux." ) );
      RETURN_CHECK( "process", event.retrieve( c, "MuonsAux." ) );
      const SG::AuxVectorBase* b = 0;
      RETURN_CHECK( "process", event.retrieve( b, "ElectronCollection" ) );
      RETURN_CHECK( "process", event.retrieve( b, "Muons" ) );

      // Record some objects into TStore:
      ClassA* objA = new ClassA();
      RETURN_CHECK( "process", store.record( objA, "MyObjA" ) );
      ClassB* objB = new ClassB();
      RETURN_CHECK( "process", store.record( objB, "MyObjB" ) );

      // They should now be accessible through TEvent:
      const ClassA* dummy1 = 0;
      RETURN_CHECK( "process", event.retrieve( dummy1, "MyObjA" ) );
      const ClassB* dummy2 = 0;
      RETURN_CHECK( "process", event.retrieve( dummy2, "MyObjB" ) );
   }

   // Return gracefully:
   return xAOD::TReturnCode::kSuccess;
}
