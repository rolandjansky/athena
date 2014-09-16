/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// a simple testing macro for the MuonSelectorTools_xAOD package
/// shamelessly stolen from CPToolTests.cxx

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

/// Example of how to run the MuonSelectorTools package to obtain information from muons

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
   xAOD::TEvent event;
   CHECK(event.readFrom( ifile.get(), xAOD::TEvent::kClassAccess ));
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }


   CP::MuonSelectionTool m_muonSelection("MuonSelection");

   m_muonSelection.msg().setLevel( MSG::VERBOSE );
   m_muonSelection.setProperty( "MaxEta", 2.4 );
   //m_muonSelection.setProperty( "Author", 12 );
   //m_muonSelection.initialize();
   CHECK (m_muonSelection.initialize().isSuccess());

   int allMuons = 0;
   int allgoodMuons = 0;

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Tell the object which entry to look at:
      event.getEntry( entry );

      int goodMuons = 0;

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

	allMuons++;
	uint8_t PixHits = 0, PixDead = 0, SCTHits = 0, SCTDead = 0, PixHoles = 0, SCTHoles = 0, TRTHits = 0, TRTOut = 0;


	// std::cout << "ENTERING  ACCEPT FUNCTION " << std::endl;
	// if(!m_muonSelection.accept(**mu_itr)) continue;
	// std::cout << "PASSED ACCEPT FUNCTION " << std::endl;

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixHits,xAOD::numberOfPixelHits))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: PixHits %i ", PixHits);

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixDead,xAOD::numberOfPixelDeadSensors))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: PixDead %i ", PixDead);

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTHits,xAOD::numberOfSCTHits))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: SCTHits %i ", SCTHits);

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTDead,xAOD::numberOfSCTDeadSensors))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: SCTDead %i ", SCTDead);


	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixHoles,xAOD::numberOfPixelHoles))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: PixHoles %i ", PixHoles);

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTHoles,xAOD::numberOfSCTHoles))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: SCTHoles %i ", SCTHoles);

	float abseta = std::abs((*mu_itr)->eta());

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(TRTHits,xAOD::numberOfTRTHits))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: TRTHits %i ", TRTHits);

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(TRTOut,xAOD::numberOfTRTOutliers))
	  Info( APP_NAME, "Missing info!");
	Info( APP_NAME, "Primary track: TRTOut %i ", TRTOut);

	uint8_t totTRT = TRTHits+TRTOut;

	Info( APP_NAME, "Muon eta:  %g ", std::abs((*mu_itr)->eta()));
	Info( APP_NAME, "TotTRT %i > 5; OutCond %i < %g ", totTRT, TRTOut, 0.9*totTRT);

	if (!((0.1<abseta && abseta<=1.9 && totTRT>5 && TRTOut<(0.9 * totTRT)) || (abseta <= 0.1 || abseta > 1.9)))
	  std::cout << "didn't pass the TRT cuts! v1 " << std::endl;

	if ((0.1<abseta && abseta<=1.9) && !(totTRT>5 && TRTOut<(0.9 * totTRT)))
	  std::cout << "didn't pass the TRT cuts! v2 " << std::endl;


         // Select "good" muons:
	//	if( ! m_muonSelection.accept( **mu_itr ) ) std::cout << "didn't pass! " << std::endl;
	if( ! m_muonSelection.passedIDCuts( **mu_itr ) ) {
	  Info(APP_NAME, "DIDNT PASS THE ID CUTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	  continue;
	}

	if( (*mu_itr)->quality() == xAOD::Muon_v1::Tight)
	  std::cout << "tight " << std::endl;
	else if( (*mu_itr)->quality() == xAOD::Muon_v1::Medium)
	  std::cout << "medium " << std::endl;
	else if( (*mu_itr)->quality() == xAOD::Muon_v1::Loose)
	  std::cout << "loose " << std::endl;
	else
	  std::cout << "no quality! " << std::endl;

	if( (*mu_itr)->muonType() == xAOD::Muon_v1::Combined)
	  std::cout << "combined " << std::endl;
	else if( (*mu_itr)->muonType() == xAOD::Muon_v1::MuonStandAlone)
	  std::cout << "stand-alone " << std::endl;
	else if( (*mu_itr)->muonType() == xAOD::Muon_v1::SegmentTagged)
	  std::cout << "segment-tagged " << std::endl;
	else if( (*mu_itr)->muonType() == xAOD::Muon_v1::CaloTagged)
	  std::cout << "calo-tagged " << std::endl;
	else if( (*mu_itr)->muonType() == xAOD::Muon_v1::SiliconAssociatedForwardMuon)
	  std::cout << "forward " << std::endl;
	else
	  std::cout << "no type! " << std::endl;

	Info(APP_NAME, "MyVerboseType %i ", (*mu_itr)->muonType());
	Info(APP_NAME, "MyVerboseQuality %i ", (*mu_itr)->quality());
	
	Info(APP_NAME, "MyVerboseGetQuality %i ", m_muonSelection.getQuality( **mu_itr ));

	if(m_muonSelection.getQuality( **mu_itr ) != xAOD::Muon_v1::Medium) {
	  Info(APP_NAME, "DIDNT PASS THE QUALITY CUTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	  continue;
	}

	goodMuons++;
	allgoodMuons++;

         // // Print some info about the selected muon:
         // Info( APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g",
         //       ( *mu_itr )->eta(), ( *mu_itr )->phi(), ( *mu_itr )->pt() );
         // Info( APP_NAME, "    Primary track: eta = %g, phi = %g, pt = %g",
         //       ( *mu_itr )->primaryTrackParticle()->eta(),
         //       ( *mu_itr )->primaryTrackParticle()->phi(),
         //       ( *mu_itr )->primaryTrackParticle()->pt() );

         // const xAOD::TrackParticle* idtrack =
         //    ( *mu_itr )->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
         // if( idtrack ) {
         //    Info( APP_NAME, "         ID track: eta = %g, phi = %g, pt = %g",
         //          idtrack->eta(), idtrack->phi(), idtrack->pt() );
         // }

      }

      Info( APP_NAME, "Number of good muons: %i",
	    goodMuons );

      // Close with a message:
      Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry + 1 ) );
   }

   Info(APP_NAME, "All muons %i and good muons %i " , allMuons, allgoodMuons);

   // Return gracefully:
   return 0;


}
