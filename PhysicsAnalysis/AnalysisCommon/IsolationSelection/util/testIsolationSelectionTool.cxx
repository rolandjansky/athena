/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: testIsolationSelectionTool.cxx 800557 2017-03-14 12:59:06Z jpoveda $

// Mindlessly copied from CPAnalysisExamples
#ifndef CPANALYSISEXAMPLES_ERRORCHECK_H
#define CPANALYSISEXAMPLES_ERRORCHECK_H

#define SCHECK( ARG )                                     \
   do {                                                  \
      const bool result = ARG;                           \
      if( ! result ) {                                   \
         ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                  #ARG );                                \
         return 1;                                       \
      }                                                  \
   } while( false )

#endif // CPANALYSISEXAMPLES_ERRORCHECK_H

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
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"

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

using std::cout;
using std::endl;

std::string MuonIso(""), ElectronIso(""), PhotonIso("");

float MuonPt(0),  ElectronPt(0),  PhotonPt(0);
float MuonEta(0), ElectronEta(0), PhotonEta(0);

StatusCode setConfigWP(TString conf){
  TEnv env;
  if(env.ReadFile(conf, kEnvAll) != 0){
    Info("Cannot read config file ", conf);
    return StatusCode::FAILURE;
  }
  Info("Reading config file ", conf);
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

int main( int argc, char* argv[] ) {

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if( argc < 3 ) {
    Error( APP_NAME, "No input file name or WP config specified!" );
    Error( APP_NAME, "Usage: %s <WPconfig> <xAOD file> <NEvents>", APP_NAME );
    return EXIT_FAILURE;
  }
  // Initialize the application:
  SCHECK( xAOD::Init( APP_NAME ) );
  auto start = std::time(nullptr);
  Info(APP_NAME, Form("Initialized %s",std::ctime(&start)));

  // Open the input file:
  const TString fileName = argv[ 2 ];
  Info(APP_NAME, "Opening file: %s", fileName.Data());
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  SCHECK( ifile.get() );

  const TString configFile = argv[ 1 ];
  SCHECK( setConfigWP(configFile) );

  // Create a TEvent object:
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  SCHECK( event.readFrom( ifile.get() ) );
  Info(APP_NAME, "Number of events in the file: %i", static_cast<int>(event.getEntries()));

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

  Info(APP_NAME, "Initialize the standard instance of the tool");
  CP::IsolationSelectionTool IsoSelectionTool("IsoSelectionTool");
  SCHECK( IsoSelectionTool.setProperty("MuonWP",      MuonIso) );
  SCHECK( IsoSelectionTool.setProperty("ElectronWP",  ElectronIso) );
  SCHECK( IsoSelectionTool.setProperty("PhotonWP",    PhotonIso) );

  SCHECK( IsoSelectionTool.setProperty("OutputLevel", MSG::DEBUG) );
  SCHECK( IsoSelectionTool.initialize() );
  
  Info(APP_NAME, "Initialize the low-Pt augmentation (PLV-only)");
  CP::IsolationLowPtPLVTool  IsoSelectionTool_lowPt("IsoSelectionTool_lowPt");
  SCHECK( IsoSelectionTool_lowPt.setProperty("OutputLevel", MSG::DEBUG) );
  SCHECK( IsoSelectionTool_lowPt.initialize() );

  Info(APP_NAME,"Initialize the instance of the tool with custom iso-cuts");
  CP::IsolationSelectionTool IsoSelectionTool_customCuts("IsoSelectionTool_customCuts");
  SCHECK( IsoSelectionTool_customCuts.setProperty("OutputLevel", MSG::DEBUG) );
  SCHECK( IsoSelectionTool_customCuts.initialize() );

  // Use "myTestWP" WP for muons
  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCuts;
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptcone20, "0.1*x+90"));
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.2*x+80"));
  SCHECK( IsoSelectionTool_customCuts.addUserDefinedWP("myTestWP", xAOD::Type::Muon, myCuts));

  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCuts2;
  myCuts2.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptcone30, "0.1*(x+90000)"));
  myCuts2.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone30, "0.02*(x+80000)"));
  SCHECK( IsoSelectionTool_customCuts.addUserDefinedWP("myTestWP2", xAOD::Type::Photon, myCuts2, "", CP::IsolationSelectionTool::Cut));

  strObj strMuon;
  strMuon.isolationValues.resize(xAOD::Iso::numIsolationTypes);

  std::string m_sgKeyPhotons("Photons");
  std::string m_sgKeyElectrons("Electrons");
  std::string m_sgKeyMuons("Muons");

  // Loop over the events:
  for( Long64_t entry(0); entry<entries; entry++ ) {
    Info(APP_NAME, Form("Entry %i ",(int)entry));
    event.getEntry( entry );

    const xAOD::PhotonContainer* photons(nullptr);
    SCHECK( event.retrieve(photons,m_sgKeyPhotons) );
    Info(APP_NAME, Form(" Number of pre-selected photons: %i",(int)photons->size()));

    for (auto ph : *photons) {
      if (ph->caloCluster() == nullptr) continue;
      if (ph->pt() < PhotonPt || std::abs(ph->caloCluster()->eta()) > PhotonEta) continue;

      if (IsoSelectionTool.accept( *ph ))
	Info(APP_NAME, Form(" --> Photon (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",ph->pt(),ph->eta(),ph->phi(),PhotonIso.c_str()));
      else
	Info(APP_NAME, Form(" --> Photon (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",ph->pt(),ph->eta(),ph->phi(),PhotonIso.c_str()));
      continue;
    }

    const xAOD::ElectronContainer* electrons(nullptr);
    SCHECK( event.retrieve(electrons,m_sgKeyElectrons) );
    Info(APP_NAME, Form(" Number of pre-selected electrons: %i",(int)electrons->size()));

    for (auto el : *electrons) {
      if (el->caloCluster() == nullptr) continue;
      if (el->pt() < ElectronPt || std::abs(el->caloCluster()->eta()) > ElectronEta) continue;
      if(ElectronIso.find("PLV") != std::string::npos) SCHECK( IsoSelectionTool_lowPt.augmentPLV(*el) );

      if (IsoSelectionTool.accept( *el ))
	Info(APP_NAME, Form(" --> Electron (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",el->pt(),el->eta(),el->phi(), ElectronIso.c_str()));
      else
	Info(APP_NAME, Form(" --> Electron (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",el->pt(),el->eta(),el->phi(), ElectronIso.c_str()));
      continue;
    }

    const xAOD::MuonContainer* muons(nullptr);
    SCHECK( event.retrieve(muons,m_sgKeyMuons) );
    Info(APP_NAME, Form(" Number of pre-selected muons: %i",(int)muons->size()));

    for (auto mu : *muons) {
      // Make sure the variable needed by your WP is passed
      strMuon.pt = mu->pt(); strMuon.eta = mu->eta();
      mu->isolation(strMuon.isolationValues[xAOD::Iso::topoetcone20], xAOD::Iso::topoetcone20);
      mu->isolation(strMuon.isolationValues[xAOD::Iso::ptvarcone30], xAOD::Iso::ptvarcone30);

      if (mu->pt() < MuonPt || std::abs(mu->eta()) > MuonEta) continue;
      if(MuonIso.find("PLV") != std::string::npos) SCHECK( IsoSelectionTool_lowPt.augmentPLV(*mu) );

      if (IsoSelectionTool.accept( *mu ))
	Info(APP_NAME, Form(" --> Muon (pt=%.1f, eta=%.3f, phi=%.3f) PASSES Isolation %s",mu->pt(),mu->eta(),mu->phi(), MuonIso.c_str()));
      else
	Info(APP_NAME, Form(" --> Muon (pt=%.1f, eta=%.3f, phi=%.3f)  FAILS Isolation %s",mu->pt(),mu->eta(),mu->phi(), MuonIso.c_str()));
      continue;
    }

    continue;
  } // end loop over events

  auto end = std::time(nullptr);
  Info(APP_NAME, Form("Ran on %i event for testing %s",(int)entries, std::ctime(&end)));
  return EXIT_SUCCESS;
}
