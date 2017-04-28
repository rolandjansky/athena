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
   // uint32_t testRunNumber = 310000;
   // CHECK( biasingTool.setProperty( "runNumber", testRunNumber ) ); // this line is only for testing that other files are properly accessibl

   InDet::InDetTrackTruthFilterTool filterTool( "InDetTrackFilterTool" );
   auto originTool = make_unique<InDet::InDetTrackTruthOriginTool> ( "InDetTrackTruthOriginTool" );
   CHECK( originTool->initialize() );
   ToolHandle< InDet::IInDetTrackTruthOriginTool > trackTruthOriginToolHandle( originTool.get() );
   CHECK( filterTool.setProperty("trackOriginTool", trackTruthOriginToolHandle) );

   InDet::JetTrackFilterTool jetFilterTool( "JetTrackFilterTool" );

   // Not a realistic set of systematics - we just want to make sure they can all be applied without breaking the tools
   CP::SystematicSet systSet = {
     //     InDet::TrackSystematicMap[InDet::TRK_RES_D0_MEAS], // could use this map, but the CP::SystematicVaration 
     //     InDet::TrackSystematicMap[InDet::TRK_RES_Z0_MEAS], // interface is more universal:
     CP::SystematicVariation("TRK_RES_D0_MEAS"),
     CP::SystematicVariation("TRK_RES_Z0_MEAS"),
     // CP::SystematicVariation("TRK_RES_D0_MEAS", 1), // these options are recommended for advanced users only
     // CP::SystematicVariation("TRK_RES_Z0_MEAS", 1),
     // CP::SystematicVariation("TRK_RES_D0_MEAS", -1),
     // CP::SystematicVariation("TRK_RES_Z0_MEAS", -1),
     CP::SystematicVariation("TRK_RES_D0_DEAD"),
     CP::SystematicVariation("TRK_RES_Z0_DEAD"),
     CP::SystematicVariation("TRK_BIAS_D0_WM"),
     CP::SystematicVariation("TRK_BIAS_Z0_WM"),
     CP::SystematicVariation("TRK_BIAS_QOVERP_SAGITTA_WM"),
     CP::SystematicVariation("TRK_FAKE_RATE_LOOSE"),
     // CP::SystematicVariation("TRK_FAKE_RATE_TIGHT"), // have to pick one of these, and TIGHT isn't implmeneted yet
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

   // Configure and initialise the tool(s):
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

   auto outfile = make_unique<TFile>("InDetTrackSystsToolTester.out.root","RECREATE");

   TH1* d0_B = new TH1F("d0_B","original d_{0}",100,-5,5);
   TH1* z0_B = new TH1F("z0_B","original z_{0}",100,-200,200);
   TH1* d0sm = new TH1F("d0sm","d_{0} after smearing",100,-5,5);
   TH1* z0sm = new TH1F("z0sm","z_{0} after smearing",100,-200,200);
   TH1* subtract_d0 = new TH1F("subtract_d0","#delta d_{0}",100,-1.0,1.0);
   TH1* subtract_z0 = new TH1F("subtract_z0","#delta z_{0}",100,-10.0,10.0);


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

	if ( !filterTool.accept(trkCpy) ) continue;
	if ( doTIDE && !jetFilterTool.accept( trkCpy, jets ) ) continue;

	double d0_1=0,d0_2=0,z0_1=0,z0_2=0;
	
	d0_1 = trkCpy->d0();
	z0_1 = trkCpy->z0();

	if (biasingTool.applyCorrection(*trkCpy) == CP::CorrectionCode::Error) {
	  Error(__func__, "Could not apply bias correction!" );
	}
	if (smearingTool.applyCorrection(*trkCpy) == CP::CorrectionCode::Error) {
	  Error(__func__, "Could not apply smearing correction!" );
	}

	d0_B->Fill( d0_1 );
	z0_B->Fill( z0_1 );
	d0_2 = trkCpy->d0();
	z0_2 = trkCpy->z0();
	d0sm->Fill( d0_2 );
	z0sm->Fill( z0_2 );
	subtract_d0->Fill( d0_2-d0_1 );
	subtract_z0->Fill( z0_2-z0_1 );
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
