/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//simple macro to test the xAOD-based calibration tool

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
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCore/ShallowCopy.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"

// for smart slimming
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

//copied from CPAnalysisExamples/errorcheck.h
#define CHECK( ARG )                                  \
do {                                                  \
    const bool result = ARG;                          \
  if( ! result ) {                                    \
      ::Error( APP_NAME, "Failed to execute: \"%s\"", \
#ARG );                                               \
      return 1;                                       \
  }                                                   \
 } while( false )


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

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create the tool
   CP::EgammaCalibrationAndSmearingTool *m_EgammaCalibrationAndSmearingTool = new CP::EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool");
   m_EgammaCalibrationAndSmearingTool->setProperty("ESModel", "es2015PRE");
   m_EgammaCalibrationAndSmearingTool->initialize();


   //===========SYSTEMATICS
   const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
   const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
   std::vector<CP::SystematicSet> sysList;

   std::cout << "SIZE of the systematics set:" << recommendedSystematics.size() << std::endl;

   for (auto sys : recommendedSystematics) { sysList.push_back(CP::SystematicSet({sys})); }

   // Loop over the events:
   for (Long64_t entry = 0; entry < entries; ++entry) {

     // Tell the object which entry to look at:
     event.getEntry( entry );

     std::cout << "=================NEXT EVENT==========================" << std::endl;

     const xAOD::EventInfo* event_info = 0;
     CHECK( event.retrieve( event_info, "EventInfo" ) );

     const xAOD::ElectronContainer* electrons;
     CHECK( event.retrieve(electrons, "Electrons") );

     //Clone
     std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy = xAOD::shallowCopyContainer( *electrons );

     //Iterate over the shallow copy
     for (xAOD::Electron* el : *electrons_shallowCopy.first) {

       std::cout << std::string(80, '-') << std::endl;
       std::cout << "xAOD/raw pt = " << el->pt() << std::endl;

       CHECK (m_EgammaCalibrationAndSmearingTool->applyCorrection(*el));
       std::cout << "Calibrated pt = " << el->pt() << std::endl;

       //systematics
       std::cout << "\n=============SYSTEMATICS CHECK NOW";
       for (auto sys : sysList)	 {
	        // Tell the calibration tool which variation to apply
	         if (m_EgammaCalibrationAndSmearingTool->applySystematicVariation(sys) != CP::SystematicCode::Ok) {
	             Error(APP_NAME, "Cannot configure calibration tool for systematics");
	          }

	        CHECK(m_EgammaCalibrationAndSmearingTool->applyCorrection(*el));
	        std::cout << "\nCalibrated pt with systematic " << sys.name() << " = " << el->pt();
	     }
       std::cout << "\n=============END SYSTEMATICS " << std::endl;

     }

     Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( event_info->eventNumber() ),
            static_cast< int >( event_info->runNumber() ),
            static_cast< int >( entry + 1 ) );

   }

   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

   return 0;
}
