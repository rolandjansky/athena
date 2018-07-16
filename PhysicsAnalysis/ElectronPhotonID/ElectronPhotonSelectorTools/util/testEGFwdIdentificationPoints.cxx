/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h" 
#include "AsgTools/AnaToolHandle.h"

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
//
#include "AsgTools/MessageCheck.h"
#include "AsgTools/MsgStream.h"
// Derivation include
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "PATCore/TResult.h"            // for TResult

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

namespace asg{
  ANA_MSG_HEADER (msgSelectorCheck)
  ANA_MSG_SOURCE (msgSelectorCheck, "EgammaSelectorCheck") 
}

//main test code
int main( int argc, char* argv[] ) {
  
  //
  using namespace asg::msgSelectorCheck;
  ANA_CHECK_SET_TYPE (int);
  MSG::Level mylevel=MSG::INFO;
  setMsgLevel(mylevel);
  //
  
  // The application's name:
  const char* APP_NAME = argv[ 0 ];
  
  // Check if we received a file name:
  if( argc < 2 ) {
    ANA_MSG_ERROR("No file name received!" );
    ANA_MSG_ERROR( "  Usage: %s [xAOD file name]");
    return EXIT_FAILURE;
  }
  
  // Initialise the application:
  ANA_CHECK( xAOD::Init( APP_NAME ) );
  
  // Open the input file:
  const TString fileName = argv[ 1 ];
  ANA_MSG_INFO( "Opening file: " << fileName.Data() );
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  ANA_CHECK( ifile.get() );
  
  // Check if we want to process Electron or Photon
   bool isElectron = true;   
   // if( argc < 4 ) {
   //   Info (APP_NAME, "By default lookign to Electron" );
   // } else {  
   //  int  argv1 = atoi(argv[ 3 ]);
   //  if(argv1 == 0)   isElectron = false; 
   //  if(isElectron)  Info( APP_NAME, "We are lookign to Electron-ID: %i", isElectron );
   //  else  Info( APP_NAME, "We are lookign to Photon-ID" );
   // }
   // Create a TEvent object (persistent store)

   xAOD::TEvent pers( xAOD::TEvent::kAthenaAccess );

   // Create a TStore object (transient store)
   xAOD::TStore trans;
   ANA_CHECK( pers.readFrom( ifile.get() ) );
   //
   ANA_MSG_INFO("Number of events in the file: "<< pers.getEntries());

   // Decide how many events to run over:
   Long64_t entries = pers.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }


   if (isElectron) {
     //Medium cut based Electrons
     
     asg::AnaToolHandle<IAsgForwardElectronIsEMSelector> electronForwardMedium ("AsgFwdElectronIsEMSelector/electronMediumIsEMSelector");
     ANA_CHECK(electronForwardMedium.setProperty("OperatingPoint", "Medium"));
     // ANA_CHECK(electronForwardMedium.setProperty("ConfigFile", "ElectronPhotonSelectorTools/offline/mc15_20150812/ForwardElectronIsEMMediumSelectorCutDefs.conf"));
     ANA_CHECK(electronForwardMedium.setProperty("OutputLevel",mylevel));
     ANA_CHECK(electronForwardMedium.initialize());
     
     //Medium Likelihood electron
     asg::AnaToolHandle<IAsgForwardElectronLikelihoodTool> electronForwardMediumLH ("AsgFwdElectronLikelihoodTool/electronMediumLHSelector");
     ANA_CHECK(electronForwardMediumLH.setProperty("primaryVertexContainer","PrimaryVertices" ));
     ANA_CHECK(electronForwardMediumLH.setProperty("ConfigFile", "/afs/cern.ch/user/p/psommer/EgammaTools_git/athena/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools/data/offline/mc16_20180716/FwdLHMediumConf.conf"));
     ANA_CHECK(electronForwardMediumLH.setProperty("OutputLevel",mylevel));
     ANA_CHECK(electronForwardMediumLH.initialize());
   
   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     pers.getEntry( entry );
     ANA_MSG_INFO("============================");
     ANA_MSG_INFO("Event: " <<entry);
     
     const xAOD::ElectronContainer* electrons;
     ANA_CHECK(pers.retrieve(electrons, "ForwardElectrons"));
     
     unsigned int nVtx(0);
     const xAOD::VertexContainer* vxContainer(0);
     if ( StatusCode::SUCCESS != pers.retrieve( vxContainer, "PrimaryVertices" ) ) {
       ANA_MSG_WARNING ( "Vertex container not found with name: " << "PrimaryVertices" );
     } else {
       for ( unsigned int i=0; i<vxContainer->size(); i++ )
     	 {
     	   const xAOD::Vertex* vxcand = vxContainer->at(i);
     	   if ( vxcand->nTrackParticles() >= 3 ) nVtx++;
     	 }
     }
     // if ( nVtx >= 3 )
     //   continue;
     // else 
     //   MSG_WARNING ( "Found : " << nVtx << " primary vertices" );

     unsigned int counter=0;
     for (const xAOD::Electron* el : *electrons) {

       // MSG_INFO("---------------------------");
       ANA_MSG_INFO("Electron: " << counter << ", pt " << el->pt() << ", " << el->eta() << ", nPV " << nVtx);
       ANA_MSG_INFO("Electron LH Medium accept result: " <<electronForwardMediumLH->accept(el));
       // ANA_MSG_INFO("Electron stored LH Medium: "  << el->passSelection("LHMedium") );
       ANA_MSG_INFO("Electron Cut Medium accept result: " <<electronForwardMedium->accept(el));
       ANA_MSG_INFO("Electron stored Medium: "  << el->passSelection("Medium") );
       ++counter;
     }
   } // loop entries
   
   } // is electron

   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();
   return 0;
}



