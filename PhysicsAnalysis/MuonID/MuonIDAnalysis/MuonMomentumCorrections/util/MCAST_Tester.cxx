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
#include <TTree.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "MuonMomentumCorrections/errorcheck.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "boost/unordered_map.hpp"

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
  TFile* ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile ) Error( APP_NAME, "Cannot find file " + fileName );
  // Create a TEvent object:
  xAOD::TEvent event( ifile, xAOD::TEvent::kClassAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );
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
  // Create the tool(s) to test:
  CP::MuonCalibrationAndSmearingTool corrTool( "MuonCorrectionTool" );
  corrTool.msg().setLevel( MSG::DEBUG );
  //corrTool.setProperty( "FilesPath", "/afs/cern.ch/work/g/gartoni/work/ParallelTest/MuonCalibrationAndSmearing/share/" );
  corrTool.initialize();
  // Systematics initialization
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  std::vector< CP::SystematicSet > sysList;
  sysList.push_back( CP::SystematicSet() );
  for( CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr ) {
    sysList.push_back( CP::SystematicSet() );
    sysList.back().insert( *sysItr );
  }
  std::vector< CP::SystematicSet >::const_iterator sysListItr;
  // Output initialization
  float ptCB( 0. ), ptID( 0. ), ptMS( 0. ), eta( 0. ), phi( 0. ), charge( 0. ), ptCBcorr( 0. );
  //
  TFile* outputFile = TFile::Open( "output.root", "recreate" );
  boost::unordered_map< CP::SystematicSet, TTree* > sysTreeMap;
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    std::string treeName = "l_tree_" + sysListItr->name();
    TTree* sysTree = new TTree( treeName.c_str(), "test tree for MCAST" );
    sysTree->Branch( "ptCB", &ptCB, "ptCB/F" );
    sysTree->Branch( "ptID", &ptID, "ptID/F" );
    sysTree->Branch( "ptMS", &ptMS, "ptMS/F" );
    sysTree->Branch( "eta", &eta, "eta/F" );
    sysTree->Branch( "phi", &phi, "phi/F" );
    sysTree->Branch( "charge", &charge, "charge/F" );
    sysTree->Branch( "ptCBcorr", &ptCBcorr, "ptCBcorr/F" );
    sysTreeMap[ *sysListItr ] = sysTree;
  }
  // Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {
    // Tell the object which entry to look at:
    event.getEntry( entry );
    // Print some event information for fun:
    const xAOD::EventInfo* ei = 0;
    CHECK( event.retrieve( ei, "EventInfo" ) );
    Info( APP_NAME, "===>>>  start processing event #%i, " "run #%i %i events processed so far  <<<===", static_cast< int >( ei->eventNumber() ), static_cast< int >( ei->runNumber() ), static_cast< int >( entry ) );
    // Get the Muons from the event:
    const xAOD::MuonContainer* muons = 0;
    CHECK( event.retrieve( muons, "Muons" ) );
    Info( APP_NAME, "Number of muons: %i", static_cast< int >( muons->size() ) );
    for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
      //
      if( corrTool.applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) { 
        Error(APP_NAME, "Cannot configure muon calibration tool for systematic" );
      }
      // Print their properties, using the tools:
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {
        Info( APP_NAME, "After Calibrated muon: 0" );
        // Select "good" muons:
        if( fabs( ( *mu_itr )->eta() ) > 2.5 ) continue;
        if( ( *mu_itr )->pt() < 6000 ) continue;
        if( ( *mu_itr )->author() != 12 ) continue;
        //
        // ptCB = ( *mu_itr )->pt();
        // const ElementLink< xAOD::TrackParticleContainer >& id_track = ( *mu_itr )->inDetTrackParticleLink();
        // ptID = ( id_track ) ? ( *id_track )->pt() : -999.;
        // const ElementLink< xAOD::TrackParticleContainer >& ms_track = ( *mu_itr )->muonSpectrometerTrackParticleLink();
        // ptMS = ( ms_track ) ? ( *ms_track )->pt() : -999.;
        // eta = ( *mu_itr )->eta();
        // phi = ( *mu_itr )->phi();
        // charge = ( *( *mu_itr )->combinedTrackParticleLink() )->charge();
        // Print some info about the selected muon:
        Info( APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g", ( *mu_itr )->eta(), ( *mu_itr )->phi(), ( *mu_itr )->pt() );
        // Create a calibrated muon:
        xAOD::Muon* mu = 0;
        if( !corrTool.correctedCopy( **mu_itr, mu ) ) {
          Error(APP_NAME, "Cannot really apply calibration nor smearing");
          continue;
        }
        ptCBcorr = mu->pt();
        Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt = %g", mu->eta(), mu->phi(), mu->pt() );
        sysTreeMap[ *sysListItr ]->Fill();
        Info( APP_NAME, "After Calibrated muon: 1" );
        // Delete the calibrated muon:
        delete mu;
        Info( APP_NAME, "After Calibrated muon: 2" );
      }
    }
    // Close with a message:
    Info( APP_NAME,
          "===>>>  done processing event #%i, "
          "run #%i %i events processed so far  <<<===",
          static_cast< int >( ei->eventNumber() ),
          static_cast< int >( ei->runNumber() ),
          static_cast< int >( entry + 1 ) );
  }
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    sysTreeMap[ *sysListItr ]->Write();
  }
  // Close output file
  outputFile->Close();
  // Return gracefully:
  return 0;
}
