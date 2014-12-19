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
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "boost/unordered_map.hpp"

// Framework include(s):
#include "AsgTools/Check.h"

int main( int argc, char* argv[] ) {

  //::: The application's name:
  const char* APP_NAME = argv[ 0 ];

  //::: Check if we received a file name:
  if( argc < 2 ) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
    return 1;
  }

  //::: Initialise the application:
  //ATH_CHECK( xAOD::Init( APP_NAME ) );
  xAOD::Init( APP_NAME );

  //::: Open the input file:
  const TString fileName = argv[ 1 ];
  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  TFile* ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile ) Error( APP_NAME, "Cannot find file " + fileName );

  //::: Create a TEvent object:
  xAOD::TEvent event( ifile, xAOD::TEvent::kClassAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );

  //::: Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  //::: Decide how many events to run over:
  Long64_t entries = event.getEntries();
  if( argc > 2 ) {
    const Long64_t e = atoll( argv[ 2 ] );
    if( e < entries ) {
      entries = e;
    }
  }

  //::: Create the tool(s) to test:

  //::: Muon Calibration and Smearing
  CP::MuonCalibrationAndSmearingTool corrTool( "MuonCorrectionTool" );
  corrTool.msg().setLevel( MSG::DEBUG );
  //ATH_CHECK( corrTool.initialize() );
  corrTool.initialize();

  //::: Muon Selection
  CP::MuonSelectionTool selTool( "MuonSelectionTool" );
  //selTool.setProperty( "MaxEta", 2.5 );
  //ATH_CHECK( selTool.initialize() );
  selTool.initialize();

  //::: Systematics initialization
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  std::vector< CP::SystematicSet > sysList;
  sysList.push_back( CP::SystematicSet() );
  for( CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr ) {
    sysList.push_back( CP::SystematicSet() );
    sysList.back().insert( *sysItr );
  }
  std::vector< CP::SystematicSet >::const_iterator sysListItr;

  //::: Output initialization
  Float_t InitPtCB( 0. ), InitPtID( 0. ), InitPtMS( 0. );
  Float_t CorrPtCB( 0. ), CorrPtID( 0. ), CorrPtMS( 0. );
  Float_t Eta( 0. ), Phi( 0. ), Charge( 0. );
  TFile* outputFile = TFile::Open( "output.root", "recreate" );
  boost::unordered_map< CP::SystematicSet, TTree* > sysTreeMap;
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    std::string treeName = "test_tree_" + sysListItr->name();
    TTree* sysTree = new TTree( treeName.c_str(), "test tree for MCAST" );

    sysTree->Branch( "InitPtCB", &InitPtCB, "InitPtCB/F" );
    sysTree->Branch( "InitPtID", &InitPtID, "InitPtID/F" );
    sysTree->Branch( "InitPtMS", &InitPtMS, "InitPtMS/F" );
    sysTree->Branch( "CorrPtCB", &CorrPtCB, "CorrPtCB/F" );
    sysTree->Branch( "CorrPtID", &CorrPtID, "CorrPtID/F" );
    sysTree->Branch( "CorrPtMS", &CorrPtMS, "CorrPtMS/F" );
    sysTree->Branch( "Eta", &Eta, "Eta/F" );
    sysTree->Branch( "Phi", &Phi, "Phi/F" );
    sysTree->Branch( "Charge", &Charge, "Charge/F" );

    sysTreeMap[ *sysListItr ] = sysTree;
  }

  //::: Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    //::: Tell the object which entry to look at:
    event.getEntry( entry );

    //::: Print some event information for fun:
    const xAOD::EventInfo* evtInfo = 0;
    //ATH_CHECK( event.retrieve( evtInfo, "EventInfo" ) );
    event.retrieve( evtInfo, "EventInfo" );
    Info( APP_NAME, "===>>>  start processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry ) );

    //::: Get the Muons from the event:
    const xAOD::MuonContainer* muons = 0;
    //ATH_CHECK( event.retrieve( muons, "Muons" ) );
    event.retrieve( muons, "Muons" );
    Info( APP_NAME, "Number of muons: %i", static_cast< int >( muons->size() ) );

    //::: Loop over systematics
    for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
      
      //::: Check if systematic is applicable
      if( corrTool.applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) { 
        Error( APP_NAME, "Cannot configure muon calibration tool for systematic" );
      }

      //::: Print their properties, using the tools:
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();

      //::: Loop over muon container
      for( ; mu_itr != mu_end; ++mu_itr ) {

        //::: Select "good" muons:
        if( ! selTool.passedIDCuts( **mu_itr ) ) {
          Info( APP_NAME, "This muon doesn't pass the ID hits quality cuts");
          continue;
        }
        InitPtCB = ( *mu_itr )->pt();
        InitPtID = ( *mu_itr )->isAvailable< float >( "InnerDetectorPt" ) ? ( *mu_itr )->auxdata< float >( "InnerDetectorPt" ) : -999;
        InitPtMS = ( *mu_itr )->isAvailable< float >( "MuonSpectrometerPt" ) ? ( *mu_itr )->auxdata< float >( "MuonSpectrometerPt" ) : -999;
        Eta = ( *mu_itr )->eta();
        Phi = ( *mu_itr )->phi();
        Charge = ( *mu_itr )->charge();
        //::: Print some info about the selected muon:
        Info( APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g", ( *mu_itr )->eta(), ( *mu_itr )->phi(), ( *mu_itr )->pt() );
        //::: Create a calibrated muon:
        xAOD::Muon* mu = 0;
        if( !corrTool.correctedCopy( **mu_itr, mu ) ) {
          Error(APP_NAME, "Cannot really apply calibration nor smearing");
          continue;
        }
        CorrPtCB = mu->pt();
        CorrPtID = mu->auxdata< float >( "InnerDetectorPt" );
        CorrPtMS = mu->auxdata< float >( "MuonSpectrometerPt" );

        Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt(CB) = %g, pt(ID) = %g, pt(MS) = %g", mu->eta(), mu->phi(), mu->pt(), mu->auxdata< float >( "InnerDetectorPt" ), mu->auxdata< float >( "MuonSpectrometerPt" ) );

        sysTreeMap[ *sysListItr ]->Fill();

        //::: Delete the calibrated muon:
        delete mu;

      }
    }

    //::: Close with a message:
    Info( APP_NAME, "===>>>  done processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry + 1 ) );
  }

  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    sysTreeMap[ *sysListItr ]->Write();
  }

  //::: Close output file
  outputFile->Close();

  //::: Return gracefully:
  return 0;

}
