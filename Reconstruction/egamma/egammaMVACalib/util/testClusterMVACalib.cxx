/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */


// System include(s):
#include <memory>
#include <cstdlib>
// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "egammaMVACalib/egammaMVATool.h"

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

namespace asg{
ANA_MSG_HEADER (msg_egammaMVACalib_Check)
  ANA_MSG_SOURCE (msg_egammaMVACalib_Check, "msg_egammaMVACalib_Check") 
}
#include "xAODRootAccess/tools/TFileAccessTracer.h"
//main test code
int main( int argc, char* argv[] ) {

  //ANA MSG
  using namespace asg::msg_egammaMVACalib_Check;
  ANA_CHECK_SET_TYPE (int);
  setMsgLevel(MSG::INFO);

  //Disable data submission for the test 
  xAOD::TFileAccessTracer::enableDataSubmission( false );
  
  // The application's name:
  const char* APP_NAME = argv[ 0 ];
   // Initialise the application:
  ANA_CHECK( xAOD::Init( APP_NAME ) );
 
 
 // Check if we received a file name:
  if( argc < 2 ) {
    ANA_MSG_ERROR("No file name received!" );
    ANA_MSG_ERROR( "  Usage:  [xAOD file name]  [Optional: Number of Events]");
    return EXIT_FAILURE;
  }

  // Open the input file:
  const TString fileName = argv[ 1 ];
  ANA_MSG_INFO( "Opening file: " << fileName.Data() );
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  ANA_CHECK( ifile.get() );

  //TEven
  xAOD::TEvent event;
  ANA_CHECK( event.readFrom( ifile.get() ) );
  ANA_MSG_INFO("Number of events in the file: "<< event.getEntries());
 
  // Decide how many events to run over:
  Long64_t entries = event.getEntries();
  if( argc > 2 ) {
    const Long64_t e = atoll( argv[ 2 ] );
    if( e < entries ) {
      entries = e;
    }
  }
  //The egammaMVATool 
  std::unique_ptr<egammaMVATool> myMVATool =std::make_unique<egammaMVATool>("myMVATool");
  ANA_CHECK(myMVATool->setProperty("folder", "egammaMVACalib/offline/v7"));
  ANA_CHECK(myMVATool->initialize());

  //
  // Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    // Tell the object which entry to look at:
    event.getEntry( entry );
    ANA_MSG_INFO("========= Event: " <<entry<< " ========");

    const xAOD::ElectronContainer* electrons;
    ANA_CHECK(event.retrieve(electrons, "Electrons"));

    const xAOD::PhotonContainer* photons;
    ANA_CHECK(event.retrieve(photons, "Photons"));

    int countEl{0};
    for (const xAOD::Electron* el : *electrons) {
      ANA_MSG_INFO("Electron # " << countEl<< " Energy at input :" << el->e()<<"."
                   << " After re-calibration : " << myMVATool->getEnergy(el));
        ++countEl;
    }

    int countPh{0};
    for (const xAOD::Photon* ph : *photons) {
      ANA_MSG_INFO("Photon # " << countPh << " Energy at input :" << ph->e()<<"."
                   << " After re-calibration: " << myMVATool->getEnergy(ph) );
        ++countPh;
    }
      ANA_MSG_INFO("=============================\n");
  }
  return 0;
}
