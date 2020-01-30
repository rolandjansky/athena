/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
using std::cout;
using std::endl;
#include <cmath>

#include "IsolationSelection/IsolationSelectionTool.h"


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
  SCHECK( xAOD::Init( APP_NAME ) );

  // Open the input file:
  const TString fileName = argv[ 1 ];
  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  SCHECK( ifile.get() );

  // Create a TEvent object:
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  SCHECK( event.readFrom( ifile.get() ) );
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
  }

  // This is a testing file, lets fail whenever we can
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
#endif

  // The most simple case, just select electron and muon
  CP::IsolationSelectionTool iso_1( "iso_1" );
  SCHECK( iso_1.setProperty("MuonWP","Gradient") );
  SCHECK( iso_1.setProperty("ElectronWP","Gradient") );
  SCHECK( iso_1.setProperty("PhotonWP","Cone40") );
//   SCHECK( iso_1.setProperty("doCutInterpolation",true) );
  SCHECK( iso_1.initialize() );

  // use a user configured Muon WP?
  CP::IsolationSelectionTool iso_2( "iso_2" );
  SCHECK( iso_2.initialize() );

  /// use "myTestWP" WP for muon
  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCuts;
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptcone20, "0.1*x+90"));
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.2*x+80"));
  SCHECK( iso_2.addUserDefinedWP("myTestWP", xAOD::Type::Muon, myCuts));

  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCuts2;
  myCuts2.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptcone30, "0.1*(x+90000)"));
  myCuts2.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone30, "0.02*(x+80000)"));
  SCHECK( iso_2.addUserDefinedWP("myTestWP2", xAOD::Type::Photon, myCuts2, "", CP::IsolationSelectionTool::Cut));

  strObj strMuon;
  strMuon.isolationValues.resize(xAOD::Iso::numIsolationTypes);

  std::string m_sgKeyPhotons("Photons");
  std::string m_sgKeyElectrons("Electrons");
  std::string m_sgKeyMuons("Muons");

  // Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {
    Info(APP_NAME,"-----------");
    // Tell the object which entry to look at:
    event.getEntry( entry );

    const xAOD::PhotonContainer* photons(nullptr);
    SCHECK( event.retrieve(photons,m_sgKeyPhotons) );
    for (auto x : *photons) {
      if (x->pt() > 7000.) {
        if (x->caloCluster() != nullptr) {
          if (fabs(x->caloCluster()->eta()) < 2.47) {
            if (iso_1.accept( *x )) {
              Info(APP_NAME," Photon passes Isolation");
            }else{
              Info(APP_NAME," Photon FAILS Isolation");
            }
          }
        }
      }
    }


    const xAOD::ElectronContainer* electrons(nullptr);
    SCHECK( event.retrieve(electrons,m_sgKeyElectrons) );
    for (auto x : *electrons) {
      if (x->pt() > 7000.) {
        if (x->caloCluster() != nullptr) {
          if (fabs(x->caloCluster()->eta()) < 2.47) {
            if (iso_1.accept( *x )) {
              Info(APP_NAME," Electron passes Isolation");
            }
          }
        }
      }
    }


    const xAOD::MuonContainer* muons(nullptr);
    SCHECK( event.retrieve(muons,m_sgKeyMuons) );
    for (auto x : *muons) {
      strMuon.pt = x->pt();
      strMuon.eta = x->eta();
      /// make sure the variable needed by your WP is passed
      x->isolation(strMuon.isolationValues[xAOD::Iso::topoetcone20], xAOD::Iso::topoetcone20);
      x->isolation(strMuon.isolationValues[xAOD::Iso::ptvarcone30], xAOD::Iso::ptvarcone30);

      if (x->pt() > 7000.) {
        if (fabs(x->eta()) < 2.5) {
          if (iso_1.accept( *x )) {
            Info(APP_NAME," Muon passes Isolation");
          }
        }
      }
    }



  } // end loop over events
  return 0;
}
