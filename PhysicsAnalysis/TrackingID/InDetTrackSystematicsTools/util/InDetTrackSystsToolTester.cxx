/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System includes:
#include <memory>
#include <cstdlib>

// ROOT includes:
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TH1.h>

// Infrastructure includes:
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"

// Smart slim testing includes:
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

// Other includes:
#include "xAODCore/ShallowCopy.h"
#include "CxxUtils/make_unique.h"

using std::string;
using CxxUtils::make_unique;


#define CHECK( ARG )                                     \
   do {                                                  \
      const bool result = ARG;                           \
      if( ! result ) {                                   \
         ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                  #ARG );                                \
         return 1;                                       \
      }                                                  \
   } while( false )


int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];
   string fileName;
   // Check if we received a file name:
   if( argc < 2 ) {
     fileName = std::getenv("ROOTCORE_TEST_FILE");
     if (fileName.empty()) Error( APP_NAME, "Could not open $ROOTCORE_TEST_FILE" );
   } else {
     fileName = argv[ 1 ];
   }

   StatusCode::enableFailure();
   CP::CorrectionCode::enableFailure();
   CP::SystematicCode::enableFailure();

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   Info( APP_NAME, "Opening file: %s", fileName.data() );
   std::unique_ptr< TFile > ifile( TFile::Open( fileName.data(), "READ" ) );
   CHECK( ifile.get() );

   // Create a TEvent object:
   //   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   xAOD::TEvent event( xAOD::TEvent::kAthenaAccess ); // at time of writing this is required for the rootcore test file
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
   } else {
     // set maximum to 500
     if (entries > 500) entries = 500;
   }

   // Create the tool(s) to test:
   InDet::InDetTrackSmearingTool smearingTool( "InDetTrackSmearingTool" );

   InDet::InDetTrackBiasingTool biasingTool( "InDetTrackBiasingTool" );
   ///  comment / uncomment these lines to test
   // CHECK( biasingTool.setProperty( "isData", true ) ); // override to test, since ROOTCORE_TEST_FILE is simluation
   // uint32_t testRunNumber = 287000;
   // CHECK( biasingTool.setProperty( "runNumber", testRunNumber ) ); // this line is only for testing that other files are properly accessible

   InDet::InDetTrackTruthFilterTool filterTool( "InDetTrackFilterTool" );
   auto originTool = make_unique<InDet::InDetTrackTruthOriginTool> ( "InDetTrackTruthOriginTool" );
   CHECK( originTool->initialize() );
   ToolHandle< InDet::IInDetTrackTruthOriginTool > trackTruthOriginToolHandle( originTool.get() );
   CHECK( filterTool.setProperty("trackOriginTool", trackTruthOriginToolHandle) );

   InDet::JetTrackFilterTool jetFilterTool( "JetTrackFilterTool" );
   
   // Not a realistic set of systematics - we just want to make sure they can all be applied without breaking the tools
   CP::SystematicSet systSet = {
     // CP::SystematicVariation("TRK_RES_D0_MEAS"),
     // CP::SystematicVariation("TRK_RES_Z0_MEAS"),
     // CP::SystematicVariation("TRK_RES_D0_MEAS", 1), // these options are recommended for advanced users only
     // CP::SystematicVariation("TRK_RES_Z0_MEAS", 1),
     CP::SystematicVariation("TRK_RES_D0_MEAS", -1),
     CP::SystematicVariation("TRK_RES_Z0_MEAS", -1),
     CP::SystematicVariation("TRK_RES_D0_DEAD"),
     CP::SystematicVariation("TRK_RES_Z0_DEAD"),
     CP::SystematicVariation("TRK_BIAS_D0_WM"),
     CP::SystematicVariation("TRK_BIAS_Z0_WM"),
     CP::SystematicVariation("TRK_BIAS_QOVERP_SAGITTA_WM"),
     CP::SystematicVariation("TRK_FAKE_RATE_LOOSE"),
     // CP::SystematicVariation("TRK_FAKE_RATE_TIGHT"), // have to pick one of these, and TIGHT isn't implemented yet
     CP::SystematicVariation("TRK_EFF_LOOSE_GLOBAL"),
     CP::SystematicVariation("TRK_EFF_LOOSE_IBL"),
     CP::SystematicVariation("TRK_EFF_LOOSE_PP0"),
     CP::SystematicVariation("TRK_EFF_LOOSE_PHYSMODEL"),
     // CP::SystematicVariation("TRK_EFF_TIGHT_GLOBAL"),
     // CP::SystematicVariation("TRK_EFF_TIGHT_IBL"),
     // CP::SystematicVariation("TRK_EFF_TIGHT_PP0"),
     // CP::SystematicVariation("TRK_EFF_TIGHT_PHYSMODEL"),
     CP::SystematicVariation("TRK_EFF_LOOSE_TIDE")
   };
   std::vector< InDet::InDetTrackSystematicsTool* > tools = {&smearingTool, &biasingTool, &filterTool, &jetFilterTool};

   /// Configure and initialise the tool(s):
   for (InDet::InDetTrackSystematicsTool* tool : tools) {
     tool->msg().setLevel( MSG::DEBUG );
     CHECK( tool->initialize() );
     auto systCode = tool->applySystematicVariation(systSet);
     if (systCode != CP::SystematicCode::Ok) {
       Error( APP_NAME, "Failed to apply systematic variation on tool %s", tool->name().data() );
       return 2;
     }
   }

   bool doTIDE = true;
   string jetCollectionName = "AntiKt4EMTopoJets";
   if ( !event.contains< xAOD::JetContainer >( jetCollectionName ) ) doTIDE = false;

   auto outfile = make_unique<TFile>("InDetTrackSystsToolTester.out.root","RECREATE");

   TH1* d0_before = new TH1F("d0_before","original d_{0}",100,-5,5);
   TH1* z0_before = new TH1F("z0_before","original z_{0}",100,-200,200);
   TH1* qOverP_before = new TH1F("qOverP_before", "original q/p", 200, -0.05, 0.05);
   TH1* d0_after = new TH1F("d0_after","d_{0} after",100,-5,5);
   TH1* z0_after = new TH1F("z0_after","z_{0} after",100,-200,200);
   TH1* qOverP_after = new TH1F("qOverP_after", "q/p after", 200, -0.05, 0.05);
   TH1* d0_diff = new TH1F("d0_diff","#delta d_{0}",100,-1.0,1.0);
   TH1* z0_diff = new TH1F("z0_diff","#delta z_{0}",100,-10.0,10.0);
   TH1* qOverP_diff = new TH1F("qOverP_diff", "#delta q/p", 200, -1.e-5, 1.e-5);

   bool isSim = true;
   const xAOD::EventInfo* ei = nullptr;
   auto sc = event.retrieve( ei, "EventInfo" );
   if ( ! sc.isSuccess() ) {
     Error( __func__, "Could not retreive event ahead of time to detect data/MC" );
   } else {
     isSim = ei->eventType( xAOD::EventInfo::IS_SIMULATION );
   }
   

   int debugN = 8; // number of tracks to print debug output for
   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Tell the object which entry to look at:
      event.getEntry( entry );
    
      // Create a shallow copy of InDetTrackParticles:
      const xAOD::TrackParticleContainer *ParticlesID = nullptr;
      CHECK( event.retrieve( ParticlesID , "InDetTrackParticles" ) );
   
      const xAOD::JetContainer* jets = nullptr;
      if (doTIDE) {
	CHECK( event.retrieve( jets, jetCollectionName ) );
      }

      std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* > ParticlesID_shallowCopy = xAOD::shallowCopyContainer( *ParticlesID );
      for ( xAOD::TrackParticle* trkCpy : *(ParticlesID_shallowCopy.first) ) {

	if ( isSim ) {
	  if ( !filterTool.accept(trkCpy) ) continue;
	}
	if ( doTIDE && !jetFilterTool.accept( trkCpy, jets ) ) continue;

	auto d0b = trkCpy->d0();
	auto z0b = trkCpy->z0();
	auto qOverPb = trkCpy->qOverP();

	if (biasingTool.applyCorrection(*trkCpy) == CP::CorrectionCode::Error) {
	  Error(__func__, "Could not apply bias correction!" );
	}
	if (smearingTool.applyCorrection(*trkCpy) == CP::CorrectionCode::Error) {
	  Error(__func__, "Could not apply smearing correction!" );
	}

	auto d0a = trkCpy->d0();
	auto z0a = trkCpy->z0();
	auto qOverPa = trkCpy->qOverP();
	auto d0d = d0a - d0b;
	auto z0d = z0a - z0b;
	auto qOverPd = qOverPa - qOverPb;

	if (debugN > 0) {
	  std::cout << "d0, before/after/diff:\t" << d0b
		    << "/" << d0a << "/" << d0d << std::endl;
	  std::cout << "z0, before/after/diff:\t" << z0b
		    << "/" << z0a << "/" << z0d << std::endl;
	  std::cout << "q/p, before/after/diff:\t" << qOverPb
		    << "/" << qOverPa << "/" << qOverPd << std::endl;
	  --debugN;
	}
	if (debugN == 0) {
	  std::cout << "supressing debug output..." << std::endl;
	  --debugN;
	}

	d0_before->Fill( d0b );
	z0_before->Fill( z0b );
	qOverP_before->Fill( qOverPb );
	d0_after->Fill( d0a );
	z0_after->Fill( z0a );
	qOverP_after->Fill( qOverPa );
	d0_diff->Fill( d0d );
	z0_diff->Fill( z0d );
	qOverP_diff->Fill( qOverPd );
      }

      delete ParticlesID_shallowCopy.first;
      delete ParticlesID_shallowCopy.second; 

   }
   
   outfile->Write();

   // testing smart slimming feature:
   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

   // Return gracefully:
   return 0;
}
