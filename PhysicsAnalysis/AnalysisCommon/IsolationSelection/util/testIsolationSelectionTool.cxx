/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <utility>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TEnv.h>

// Infrastructure include(s):
#include "AsgMessaging/MessageCheck.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODPrimitives/IsolationType.h"

#include <iostream>
#include <cmath>
#include <ctime>

#include "IsolationSelection/IsolationSelectionTool.h"
#include "IsolationSelection/IsolationLowPtPLVTool.h"

ANA_MSG_HEADER(Test)
ANA_MSG_SOURCE(Test, "testIsolationSelectionTool")
using namespace Test;

std::string MuonIso(""), ElectronIso(""), PhotonIso("");

float MuonPt(0),  ElectronPt(0),  PhotonPt(0);
float MuonEta(0), ElectronEta(0), PhotonEta(0);

StatusCode setConfigWP(TString conf){
  TEnv env;
  if(env.ReadFile(conf, kEnvAll) != 0){
    ANA_MSG_INFO("Cannot read config file " << conf);
    return StatusCode::FAILURE;
  }
  ANA_MSG_INFO("Reading config file " << conf);
  MuonIso     = env.GetValue("MuonIso",     "PflowTight_FixedRad");
  MuonPt      = env.GetValue("MuonPt",      7000.);
  MuonEta     = env.GetValue("MuonEta",     2.5);

  ElectronIso = env.GetValue("ElectronIso", "PLImprovedTight");
  ElectronPt  = env.GetValue("ElectronPt",  7000.);
  ElectronEta = env.GetValue("ElectronEta", 2.47);

  PhotonIso   = env.GetValue("PhotonIso",   "FixedCutTight");
  PhotonPt    = env.GetValue("PhotonPt",    7000.);
  PhotonEta   = env.GetValue("PhotonEta",   2.47);

  env.PrintEnv();
  return StatusCode::SUCCESS;
}

int main( int argc, char* argv[] ){
  ANA_CHECK_SET_TYPE (int);

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if( argc < 3 ) {
    ANA_MSG_ERROR("No input file name or WP config specified!");
    ANA_MSG_ERROR("Usage: %s <WPconfig> <xAOD file> <NEvents>");
    return EXIT_FAILURE;
  }
  // Initialize the application:
  ANA_CHECK( xAOD::Init( APP_NAME ) );
  auto start = std::time(nullptr);
  ANA_MSG_INFO("Initialized " << std::ctime(&start));

  // Open the input file:
  const TString fileName = argv[ 2 ];
  ANA_MSG_INFO("Opening file: " << fileName.Data());
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile.get() ) return EXIT_FAILURE;

  const TString configFile = argv[ 1 ];
  ANA_CHECK( setConfigWP(configFile) );

  // Create a TEvent object:
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  ANA_CHECK( event.readFrom( ifile.get() ) );
  ANA_MSG_INFO("Number of events in the file" << static_cast<int>(event.getEntries()));

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // Decide how many events to run over:
  Long64_t entries = event.getEntries();
  if( argc > 3 ) {
    const Long64_t e = atoll( argv[ 3 ] );
    if( e < entries ) entries = e;
  }

  // This is a testing file, lets fail whenever we can
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
#endif

  ANA_MSG_INFO("Initialize the standard instance of the tool");
  CP::IsolationSelectionTool IsoSelectionTool("IsoSelectionTool");
  ANA_CHECK( IsoSelectionTool.setProperty("MuonWP",      MuonIso) );
  ANA_CHECK( IsoSelectionTool.setProperty("ElectronWP",  ElectronIso) );
  ANA_CHECK( IsoSelectionTool.setProperty("PhotonWP",    PhotonIso) );

  ANA_CHECK( IsoSelectionTool.setProperty("OutputLevel", MSG::DEBUG) );
  ANA_CHECK( IsoSelectionTool.initialize() );
  
  ANA_MSG_INFO("Initialize the low-Pt augmentation (PLV-only)");
  CP::IsolationLowPtPLVTool  IsoSelectionTool_lowPt("IsoSelectionTool_lowPt");
  ANA_CHECK( IsoSelectionTool_lowPt.setProperty("OutputLevel", MSG::DEBUG) );
  ANA_CHECK( IsoSelectionTool_lowPt.initialize() );

  std::string m_sgKeyPhotons("Photons");
  std::string m_sgKeyElectrons("Electrons");
  std::string m_sgKeyMuons("Muons");

  // Loop over the events:
  for( Long64_t entry(0); entry<entries; entry++ ) {
    ANA_MSG_INFO("Entry " << (int)entry);
    event.getEntry( entry );

    const xAOD::PhotonContainer* photons(nullptr);
    ANA_CHECK( event.retrieve(photons,m_sgKeyPhotons) );
    ANA_MSG_INFO(" Number of pre-selected photons: " << (int)photons->size());

    for (auto ph : *photons) {
      if (ph->caloCluster() == nullptr) continue;
      if (ph->pt() < PhotonPt || std::abs(ph->caloCluster()->eta()) > PhotonEta) continue;

      if (IsoSelectionTool.accept( *ph ))
	ANA_MSG_INFO(Form(" --> Photon (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",ph->pt(),ph->eta(),ph->phi(),PhotonIso.c_str()));
      else
	ANA_MSG_INFO(Form(" --> Photon (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",ph->pt(),ph->eta(),ph->phi(),PhotonIso.c_str()));
      continue;
    }

    const xAOD::ElectronContainer* electrons(nullptr);
    ANA_CHECK( event.retrieve(electrons,m_sgKeyElectrons) );
    ANA_MSG_INFO(" Number of pre-selected electrons: " << (int)electrons->size());

    for (auto el : *electrons) {
      if (el->caloCluster() == nullptr) continue;
      if (el->pt() < ElectronPt || std::abs(el->caloCluster()->eta()) > ElectronEta) continue;
      if(ElectronIso.find("PLV") != std::string::npos) ANA_CHECK( IsoSelectionTool_lowPt.augmentPLV(*el) );

      if (IsoSelectionTool.accept( *el ))
	ANA_MSG_INFO(Form(" --> Electron (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",el->pt(),el->eta(),el->phi(), ElectronIso.c_str()));
      else
	ANA_MSG_INFO(Form(" --> Electron (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",el->pt(),el->eta(),el->phi(), ElectronIso.c_str()));
      continue;
    }

    const xAOD::MuonContainer* muons(nullptr);
    ANA_CHECK( event.retrieve(muons,m_sgKeyMuons) );
    ANA_MSG_INFO(" Number of pre-selected muons: " << (int)muons->size());

    for (auto mu : *muons) {
      if (mu->pt() < MuonPt || std::abs(mu->eta()) > MuonEta) continue;
      if(MuonIso.find("PLV") != std::string::npos) ANA_CHECK( IsoSelectionTool_lowPt.augmentPLV(*mu) );

      if (IsoSelectionTool.accept( *mu ))
	ANA_MSG_INFO(Form(" --> Muon (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",mu->pt(),mu->eta(),mu->phi(), MuonIso.c_str()));
      else
	ANA_MSG_INFO(Form(" --> Muon (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",mu->pt(),mu->eta(),mu->phi(), MuonIso.c_str()));
      continue;
    }

    continue;
  } // end loop over events

  auto end = std::time(nullptr);
  ANA_MSG_INFO(Form("Ran on %i event for testing %s",(int)entries, std::ctime(&end)));

  return EXIT_SUCCESS;
}
