/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// a testing utility for the track selection tool mostly copied from CPToolTests.cxx

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TH2F.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check if we received a file name:
   if( argc < 2 ) {
      Error( APP_NAME, "No file name received!" );
      Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
      return 1;
   }

   // fail on an unchecked StatusCode
   StatusCode::enableFailure();

   // Initialise the application:
   ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(xAOD::Init( APP_NAME )) );

   // Open the input file:
   const std::string fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.data() );
   std::unique_ptr< TFile > ifile( TFile::Open( fileName.data(), "READ" ) );
   StatusCode gotFile = ifile.get()!=nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE;
   ASG_CHECK_SA( APP_NAME, gotFile );

   // Create a TEvent object:
   xAOD::TEvent event( static_cast<TFile*>(nullptr), xAOD::TEvent::kClassAccess );
   ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(event.readFrom( ifile.get() )) );
   Info( APP_NAME, "Number of events in the file: %llu", event.getEntries() );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create the tool(s) to test:
   InDet::InDetTrackSelectionTool selTool( "TrackSelection", "MinBias" );
   // Configure and initialise the tool(s):
   selTool.msg().setLevel( MSG::INFO );
   //selTool.setCutLevel( InDet::CutLevel::Loose ); // set tool to apply default loose cuts
   //   ASG_CHECK_SA( APP_NAME, selTool.setProperty( "CutLevel", "TightPrimary" ) ); // can set this in the constructor
   //ASG_CHECK_SA( APP_NAME, selTool.setProperty("minPt", 100.0) ); // the cuts can be modified from the pre-defined sets
   //ASG_CHECK_SA( APP_NAME, selTool.setProperty("maxZ0", 10.) );
   //ASG_CHECK_SA( APP_NAME, selTool.setProperty("maxD0", 1.5) );
   //ASG_CHECK_SA( APP_NAME, selTool.setProperty("minNTrtHits", 1); )
   ASG_CHECK_SA( APP_NAME, selTool.setProperty("maxZ0SinTheta", -1.) ); // try to turn off this cut
   ASG_CHECK_SA( APP_NAME, selTool.setProperty("minEProbabilityHT", .01) );
   ASG_CHECK_SA( APP_NAME, selTool.setProperty("eProbHTonlyForXe", true) );   
   ASG_CHECK_SA( APP_NAME, selTool.initialize() );

   std::unique_ptr< TFile > outFile(new TFile("IDTrackSelectionToolTestOut.root", "RECREATE"));

   // Create some histograms to look at results
   TH1F* hist_z0 = new TH1F("z0", "z0", 1000, -200, 200);
   TH1F* hist_z0pvz = new TH1F("z0pvz", "z0pvz", 1000, -200, 200);
   TH1F* hist_vtxz = new TH1F("vtxz", "vtxz", 1000, -200, 200);

   TH1I* hist_si = new TH1I("siHits", "Si Hits", 20, 0, 20);
   
   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     ASG_CHECK_SA( APP_NAME, !event.getEntry( entry ) );
          
     // Get the InDetTrackParticles from the event:
     const xAOD::TrackParticleContainer* tracks = 0;
     ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(event.retrieve( tracks, "InDetTrackParticles" )) );
     //Info( APP_NAME, "Number of tracks: %i",
     //    static_cast< int >( tracks->size() ) );
     
     // Print their properties, using the tools:
     int numGoodTracks = 0;
     for( const xAOD::TrackParticle* track : *tracks ) {
	 uint8_t pix = 0;
	 uint8_t sct = 0;
	 uint8_t sharedPix = 0;
	 uint8_t sharedSct = 0;
	 uint8_t trt = 0;
	 track->summaryValue(pix, xAOD::numberOfPixelHits);
	 track->summaryValue(sct, xAOD::numberOfSCTHits);
	 track->summaryValue(sharedPix, xAOD::numberOfPixelSharedHits);
	 track->summaryValue(sharedSct, xAOD::numberOfSCTSharedHits);
	 track->summaryValue(trt, xAOD::numberOfTRTHits);

       // Select "good" tracks:
	 if( ! selTool.accept( *track/*, track->vertex() */) ) {
       // accept() returns a TAccept object, which allows for more sophisticated
       // selection than a simple all/none, if we want it.
	 // Info( APP_NAME, "pt = %f, eta = %f", track->pt(), track->eta() );
	 // Info( APP_NAME, "pix: %i sct: %i trt: %i shared pix: %i shared sct: %i",
	 //       pix, sct, trt, sharedPix, sharedSct );
	   continue;
	 }
       
       numGoodTracks++;
       
       hist_z0pvz->Fill( track->z0() + track->vz() );
       if (track->vertex())
	 hist_vtxz->Fill( track->vertex()->z() );
       else
	 hist_z0->Fill( track->z0() );

       hist_si->Fill( pix + sct );

       // Print some info about the selected track:
       //Info( APP_NAME, "  Selected track: eta = %g, phi = %g, pt = %g",
       //    track->eta(), track->phi(), track->pt() );
       //Info( APP_NAME, "       ID track: eta = %g, phi = %g, pt = %g",
       //      track->eta(),
       //      track->phi(),
       //      track->pt() );
       
     }
     
     // tell how many tracks were accepted:
     Info( APP_NAME, "%i out of %i tracks were accepted.",
	   numGoodTracks,
	   static_cast< int >( tracks->size() ));
     
     // Close with a message:
     Info( APP_NAME,
	   "===>>>   %i events processed so far  <<<===",
	   static_cast< int >( entry + 1 ) );
   }
   
   ASG_CHECK_SA( APP_NAME, selTool.finalize() );

   // draw histogram
   outFile->Write();

   // Return gracefully:
   return 0;
}
