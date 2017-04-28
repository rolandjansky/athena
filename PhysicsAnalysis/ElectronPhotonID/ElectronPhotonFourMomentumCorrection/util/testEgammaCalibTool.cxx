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
#   include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
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


const double MIN_PT = 20E3;

int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[0];

   // Check if we received a file name:
   if (argc < 2) {
     Error( APP_NAME, "No file name received!" );
     Error( APP_NAME, "  Usage: %s (xAOD file name)", APP_NAME );
     return 1;
   }

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[1];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::unique_ptr<TFile> ifile(TFile::Open( fileName, "READ"));
   CHECK( ifile.get() );
   if ((!ifile.get() ) || ifile->IsZombie()) {
     Error(APP_NAME, "Couldn't open file: %s", argv[1]);
     return 1;
   }

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
   std::unique_ptr<CP::IEgammaCalibrationAndSmearingTool> tool(new CP::EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool"));
   asg::setProperty(tool.get(), "ESModel", "es2016data_mc15c");
   asg::setProperty(tool.get(), "randomRunNumber", 123456);
   RETURN_CHECK(APP_NAME, tool->initialize());


   //===========SYSTEMATICS
   const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
   const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
   std::vector<CP::SystematicSet> sysList;

   std::cout << "SIZE of the systematics set:" << recommendedSystematics.size() << std::endl;

   for (auto sys : recommendedSystematics) { sysList.push_back(CP::SystematicSet({sys})); }


   // Loop over the events:
   for (Long64_t entry = 0; entry < entries; ++entry) {

     // Tell the object which entry to look at:
     event.getEntry(entry);

     std::cout << "=================NEXT EVENT==========================" << std::endl;

     const xAOD::EventInfo* event_info = 0;
     CHECK( event.retrieve( event_info, "EventInfo" ) );


     const xAOD::PhotonContainer* photons;
     CHECK( event.retrieve(photons, "Photons") );

     //Clone
     std::pair<xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer(*photons);

     //Iterate over the shallow copy
     for (xAOD::Photon* ph : *photons_shallowCopy.first) {
       if (ph->pt() < MIN_PT) { continue; }
       std::cout << std::string(80, '-') << std::endl;
       std::cout << "eta|phi|pt|xAOD e = " << ph->eta() << "|" << ph->phi() << "|" << ph->pt() << "|" << ph->e() << std::endl;
       if (not ph->caloCluster()) {
	 std::cout << "ERROR: particle has no calocluster" << std::endl;
	 continue;
       }
       std::cout << "raw E = " << ph->caloCluster()->energyBE(1) + ph->caloCluster()->energyBE(2) + ph->caloCluster()->energyBE(3) << std::endl;
       if (!(xAOD::EgammaHelpers::isElectron(ph) or xAOD::EgammaHelpers::isPhoton(ph))) {
	 std::cout << "particle is not electron of photon" << std::endl;
	 continue;
       }

       CHECK(tool->applyCorrection(*ph));
       std::cout << "Calibrated e = " << ph->e() << std::endl;

       //systematics
       std::cout << "\n=============SYSTEMATICS CHECK NOW";
       for (auto sys : sysList)	 {
	 // Tell the calibration tool which variation to apply
	 if (tool->applySystematicVariation(sys) != CP::SystematicCode::Ok) {
	   Error(APP_NAME, "Cannot configure calibration tool for systematics");
	 }
	 
	 CHECK(tool->applyCorrection(*ph));
	 std::cout << "\nCalibrated pt with systematic " << sys.name() << " = " << ph->pt();
       }
       std::cout << "\n=============END SYSTEMATICS " << std::endl;
     }



     const xAOD::ElectronContainer* electrons;
     CHECK( event.retrieve(electrons, "Electrons") );

     //Clone
     std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy = xAOD::shallowCopyContainer( *electrons );

     //Iterate over the shallow copy
     for (xAOD::Electron* el : *electrons_shallowCopy.first) {
       if (el->pt() < MIN_PT) { continue; }
       std::cout << std::string(80, '-') << std::endl;
       std::cout << "eta|phi|pt|xAOD e = " << el->eta() << "|" << el->phi() << "|" << el->pt() << "|" << el->e() << std::endl;
       if (not el->caloCluster()) {
	 std::cout << "ERROR: particle has no calocluster" << std::endl;
	 continue;
       }
       std::cout << "raw E = " << el->caloCluster()->energyBE(1) + el->caloCluster()->energyBE(2) + el->caloCluster()->energyBE(3) << std::endl;
       if (!(xAOD::EgammaHelpers::isElectron(el) or xAOD::EgammaHelpers::isPhoton(el))) {
	 std::cout << "particle is not electron of photon" << std::endl;
	 continue;
       }

       CHECK (tool->applyCorrection(*el));
       std::cout << "Calibrated e = " << el->e() << std::endl;

       //systematics
       std::cout << "\n=============SYSTEMATICS CHECK NOW";
       for (auto sys : sysList)	 {
	 // Tell the calibration tool which variation to apply
	 if (tool->applySystematicVariation(sys) != CP::SystematicCode::Ok) {
	   Error(APP_NAME, "Cannot configure calibration tool for systematics");
	 }
	 
	 CHECK(tool->applyCorrection(*el));
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
