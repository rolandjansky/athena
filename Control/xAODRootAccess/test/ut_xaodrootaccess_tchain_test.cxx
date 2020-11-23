/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TChain.h>
#include <TError.h>

// EDM that the package uses anyway:
#include "AthContainers/AuxVectorBase.h"
#include "xAODCore/AuxContainerBase.h"

#include "AsgMessaging/MessageCheck.h"

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
StatusCode process( xAOD::TEvent& event, xAOD::TStore& store );

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_tchain_test";

   // Initialise the environment:
   ANA_CHECK( xAOD::Init( APP_NAME ) );

   // Create the tested object(s):
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   xAOD::TStore store;

   const char* ref = getenv( "ATLAS_REFERENCE_DATA" );
   const std::string FPATH =
      ref ? ref : "/afs/cern.ch/atlas/project/PAT";
   const std::string FNAME1 = FPATH + "/xAODs/r5591/"
      "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon."
      "AOD.e1727_s1933_s1911_r5591/AOD.01494881._105458.pool.root.1";
   const std::string FNAME2 = FPATH + "/xAODs/r5591/"
      "mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon."
      "AOD.e1727_s1933_s1911_r5591/AOD.01494881._107292.pool.root.1";

   // Set up a TChain with some mc14_8TeV input files:
   ::TChain chain1( "CollectionTree" );
   chain1.Add( FNAME1.c_str() );
   chain1.Add( FNAME2.c_str() );

   // Connect the TEvent object to it:
   ANA_CHECK( event.readFrom( &chain1 ) );

   // Run the processing:
   ::Info( APP_NAME, "Processing mc14_8TeV chain..." );
   ANA_CHECK( process( event, store ) );

   // Set up a TChain with some mc14_8TeV input files:
   ::TChain chain2( "CollectionTree" );
   const std::string FNAME3 = FPATH + "/xAODs/r5787/"
      "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge."
      "AOD.e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
      "AOD.01597980._000098.pool.root.1";
   const std::string FNAME4 = FPATH + "/xAODs/r5787/"
      "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge."
      "AOD.e2928_s1982_s2008_r5787_r5853_tid01597980_00/"
      "AOD.01597980._000420.pool.root.1";
   chain2.Add( FNAME3.c_str() );
   chain2.Add( FNAME4.c_str() );

   // Connect the TEvent object to it:
   ANA_CHECK( event.readFrom( &chain2 ) );

   // Run the processing:
   ::Info( APP_NAME, "Processing mc14_13TeV chain..." );
   ANA_CHECK( process( event, store ) );

   // Return gracefully:
   return 0;
}

StatusCode process( xAOD::TEvent& event, xAOD::TStore& store ) {

   // Loop over all events:
   const ::Long64_t entries = event.getEntries();
   for( ::Long64_t entry = 0; entry < entries; ++entry ) {

      // Clear the store:
      store.clear();

      // Read in the event:
      if( event.getEntry( entry ) < 0 ) {
         ::Error( "process", "Couldn't load entry %i",
                  static_cast< int >( entry ) );
         return StatusCode::FAILURE;
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
   return StatusCode::SUCCESS;
}
