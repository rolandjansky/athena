/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxVectorBase.h"

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/IOStats.h"

#include "AsgMessaging/MessageCheck.h"

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Get the name of the application:
   static const char* const APP_NAME = "ut_xaodrootaccess_stats_test";

   // Initialise the environment:
   ANA_CHECK( xAOD::Init( APP_NAME ) );

   // Create the tested object(s):
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );

   // Connect an input file to the event:
   static const char* const FNAME = "${ASG_TEST_FILE_DATA}";
   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, "File %s couldn't be opened...", FNAME );
      return 1;
   }
   ANA_CHECK( event.readFrom( ifile.get() ) );

   // Get the auxiliary ID of the "pt" variable:
   const SG::auxid_t auxidPt =
      SG::AuxTypeRegistry::instance().getAuxID< float >( "pt" );

   // Loop over all events:
   const ::Long64_t entries = event.getEntries();
   for( ::Long64_t entry = 0; entry < entries; ++entry ) {

      // Load the event:
      if( event.getEntry( entry ) < 0 ) {
         ::Error( APP_NAME, "Couldn't load entry %i",
                  static_cast< int >( entry ) );
         return 1;
      }

      // Print some info for the user:
      if( ! ( entry % 100 ) ) {
         ::Info( APP_NAME, "Processing entry %i / %i",
                 static_cast< int >( entry ), static_cast< int >( entries ) );
      }

      // Retrieve some interface containers, to trigger the setup of the
      // auxiliary containers:
      const SG::AuxVectorBase* vb = 0;
      ANA_CHECK( event.retrieve( vb, "Electrons" ) );
      ANA_CHECK( event.retrieve( vb, "Muons" ) );

      // Load some containers from it and access their "pt" property:
      const xAOD::AuxContainerBase* dummy = 0;
      ANA_CHECK( event.retrieve( dummy,
                                              "ElectronsAux." ) );
      if( strcmp( dummy->name(), "ElectronsAux." ) ) {
         ::Error( APP_NAME,
                  "The name of ElectronsAux. was not set correctly" );
         return 1;
      }
      dummy->getData( auxidPt );
      ANA_CHECK( event.retrieve( dummy, "MuonsAux." ) );
      if( strcmp( dummy->name(), "MuonsAux." ) ) {
         ::Error( APP_NAME,
                  "The name of ElectronsAux. was not set correctly" );
         return 1;
      }
      dummy->getData( auxidPt );
   }

   // Print the collected statistics info:
   xAOD::ReadStats& stats = xAOD::IOStats::instance().stats();
   stats.Print( "SmartSlimming" );
   stats.printSmartSlimmingBranchList();

   // Return gracefully:
   return 0;
}
