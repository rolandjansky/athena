/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_stats_test.cxx 646132 2015-02-11 15:17:44Z krasznaa $

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

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_xaodrootaccess_stats_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Create the tested object(s):
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );

   // Connect an input file to the event:
   static const char* FNAME =
      "/afs/cern.ch/atlas/project/PAT/xAODs/r5597/"
      "data12_8TeV.00204158.physics_JetTauEtmiss.recon.AOD.r5597/"
      "AOD.01495682._003054.pool.root.1";
   std::auto_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );
   if( ! ifile.get() ) {
      ::Error( APP_NAME, "File %s couldn't be opened...", FNAME );
      return 1;
   }
   RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

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
      RETURN_CHECK( APP_NAME, event.retrieve( vb, "ElectronCollection" ) );
      RETURN_CHECK( APP_NAME, event.retrieve( vb, "Muons" ) );

      // Load some containers from it and access their "pt" property:
      const xAOD::AuxContainerBase* dummy = 0;
      RETURN_CHECK( APP_NAME, event.retrieve( dummy,
                                              "ElectronCollectionAux." ) );
      if( strcmp( dummy->name(), "ElectronCollectionAux." ) ) {
         ::Error( APP_NAME,
                  "The name of ElectronCollectionAux. was not set correctly" );
         return 1;
      }
      dummy->getData( auxidPt );
      RETURN_CHECK( APP_NAME, event.retrieve( dummy, "MuonsAux." ) );
      if( strcmp( dummy->name(), "MuonsAux." ) ) {
         ::Error( APP_NAME,
                  "The name of ElectronCollectionAux. was not set correctly" );
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
