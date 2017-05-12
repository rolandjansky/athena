/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPToolTester.cxx 300804 2014-06-04 16:49:29Z krasznaa $

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"
#include "CPAnalysisExamples/MuonEfficiencyToolExample.h"
#include "CPAnalysisExamples/MuonSelectionToolExample.h"
#include "CPAnalysisExamples/MuonSmearingToolExample.h"

// Other include(s):
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicRegistry.h"

int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check if we received a file name:
   if( argc < 2 ) {
      Error( APP_NAME, "No file name received!" );
      Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
      return 1;
   }

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );

   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   CHECK( event.readFrom( ifile.get() ) );
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );

   // Create a transient object store. Needed for the tools.
   xAOD::TStore store;

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create the tool(s) to test:
   CP::MuonSelectionToolExample selTool( "MuonSelector" );
   CP::MuonSmearingToolExample corrTool( "MuonCorrector" );
   CP::MuonEfficiencyToolExample effTool( "MuonEfficiency" );

   // Configure and initialise the tool(s):
   selTool.msg().setLevel( MSG::DEBUG );
   selTool.setProperty( "MaxEta", 2.4 );
   selTool.setProperty( "Author", 12 );
   selTool.initialize();

   corrTool.msg().setLevel( MSG::DEBUG );
   corrTool.initialize();

   effTool.msg().setLevel( MSG::DEBUG );
   effTool.setProperty( "DummyProperty", 5.5 );
   effTool.initialize();

   // Print the recommended systematics
   const CP::SystematicRegistry& reg = CP::SystematicRegistry::getInstance();
   const CP::SystematicSet& recommendedSysts = reg.recommendedSystematics();
   Info( APP_NAME, "Recommended systematics:" );
   for(auto sysItr = recommendedSysts.begin();
       sysItr != recommendedSysts.end(); ++sysItr){
      Info( APP_NAME, "  %s", sysItr->name().c_str() );
   }

   // Build a dummy list of systematics (for MuonSmearingTool).
   // A proper approach would either use the recommened systematics
   // above or query the systematics for each sys tool and build
   // the list of NP-points from that.
   std::vector<CP::SystematicSet> sysList;
   sysList.push_back(CP::SystematicSet()); // nominal
   std::vector<std::string> baseVariations = { "mu_id", "mu_ms", "mu_cb" };
   for(auto& s : baseVariations){
      CP::SystematicSet sysSet_p = { CP::SystematicVariation(s, 1) };
      CP::SystematicSet sysSet_n = { CP::SystematicVariation(s, -1) };
      sysList.push_back(sysSet_p);
      sysList.push_back(sysSet_n);
   }

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Tell the object which entry to look at:
      event.getEntry( entry );

      // Print some event information for fun:
      const xAOD::EventInfo* ei = 0;
      CHECK( event.retrieve( ei, "EventInfo" ) );
      Info( APP_NAME,
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry ) );

      // Get the Muons from the event:
      const xAOD::MuonContainer* muons = 0;
      CHECK( event.retrieve( muons, "Muons" ) );
      Info( APP_NAME, "Number of muons: %i",
            static_cast< int >( muons->size() ) );

      // Print their properties, using the tools:
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {

         // Select "good" muons:
         if( ! selTool.accept( **mu_itr ) ) continue;

         // Print some info about the selected muon:
         Info( APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g",
               ( *mu_itr )->eta(), ( *mu_itr )->phi(), ( *mu_itr )->pt() );
         Info( APP_NAME, "    Primary track: eta = %g, phi = %g, pt = %g",
               ( *mu_itr )->primaryTrackParticle()->eta(),
               ( *mu_itr )->primaryTrackParticle()->phi(),
               ( *mu_itr )->primaryTrackParticle()->pt() );
         const xAOD::TrackParticle* idtrack =
            ( *mu_itr )->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
         if( idtrack ) {
            Info( APP_NAME, "         ID track: eta = %g, phi = %g, pt = %g",
                  idtrack->eta(), idtrack->phi(), idtrack->pt() );
         }

         // Loop over systematics to apply.
         // NOTE: this is not a good example of how to properly apply systematics,
         // because I'm looping over systematics WITHIN the muon loop!
         // It's really just a test of the SystematicsTool machinery.
         for( auto sysSet : sysList ){

            Info( APP_NAME, "Now applying systematics: %s",
                  sysSet.name().c_str() );

            // Apply the systematics configuration to the smearing tool:
            if( corrTool.applySystematicVariation(sysSet) !=
                CP::SystematicCode::Ok){
              Error( APP_NAME, "Error setting systematic: %s",
                     sysSet.name().c_str() );
              return 1;
            }

            // Create a calibrared muon:
            xAOD::Muon* mu = 0;
            CHECK( corrTool.correctedCopy( **mu_itr, mu ) );
            Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt = %g",
                  mu->eta(), mu->phi(), mu->pt() );

            // Use the "simple interface" of the tool(s):
            float eff = 0.0, sf = 0.0;
            CHECK( effTool.getEfficiency( *mu, eff ) );
            Info( APP_NAME, "       efficiency = %g", eff );
            CHECK( effTool.getScaleFactor( *mu, sf ) );
            Info( APP_NAME, "       scaleFactor = %g", sf );

            // Call the tool on it in "decrating mode":
            CHECK( effTool.applyEfficiency( *mu ) );
            Info( APP_NAME, "       efficiency = %g",
                  mu->auxdata< float >( "MuonEfficiency" ) );

            // Delete the calibrated muon:
            delete mu;

         }
      }

      // Close with a message:
      Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry + 1 ) );
   }

   // Return gracefully:
   return 0;
}
