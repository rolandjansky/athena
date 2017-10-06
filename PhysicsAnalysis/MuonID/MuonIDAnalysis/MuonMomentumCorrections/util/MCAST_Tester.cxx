/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include "boost/unordered_map.hpp"

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TTree.h>
#include <TChain.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "AsgTools/Check.h"

#include "PATInterfaces/SystematicCode.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include <string>

using namespace std;

int main( int argc, char* argv[] ) {
  bool do_it_the_right_way = false;
  //::: The application's name:
  const char* APP_NAME = argv[ 0 ];

  //::: Check if we received a file name:
  if( argc < 2 ) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
    return 1;
  }

  string options;
  for( int i=0; i<argc; i++)   { options+=(argv[i]);  }
  
  int Ievent=-1;
  if(options.find("-event")!=string::npos){
  for( int ipos=0; ipos<argc ; ipos++ ) {
    if (string(argv[ipos]).compare("-event")==0)
      {       Ievent=atoi(argv[ipos+1]); break; }
  }}

  int nEvents=-1;
  if(options.find("-n")!=string::npos){
  for( int ipos=0; ipos<argc ; ipos++ ) {
    if (string(argv[ipos]).compare("-n")==0)
      {       nEvents=atoi(argv[ipos+1]); break; }
  }}

  
  //std::cout<<"Checking for event "<<Ievent<<std::endl;  
  //::: Initialise the application:
  //ATH_CHECK( xAOD::Init( APP_NAME ) );
  xAOD::Init( APP_NAME );
  xAOD::TReturnCode::enableFailure();

  //::: Open the input file:
  const TString fileName = argv[ 1 ];
  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  TFile* ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile ) Error( APP_NAME, "Cannot find file %s",fileName.Data() );

  TChain *chain = new TChain ("CollectionTree","CollectionTree");
  chain->Add(fileName);

  //::: Create a TEvent object:
  //xAOD::TEvent event( ifile, xAOD::TEvent::kAthenaAccess );
  xAOD::TEvent event( (TTree*)chain, xAOD::TEvent::kAthenaAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );

  //::: Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  //::: Decide how many events to run over:
  Long64_t entries = event.getEntries();

  //if( argc > 2 ) {
  // const Long64_t e = atoll( argv[ 2 ] );
  //if( e < entries ) {
  //  entries = e;
  //}
  //}

  //::: Create the tool(s) to test:

  //::: Muon Calibration and Smearing
  CP::MuonCalibrationAndSmearingTool corrTool( "MuonCorrectionTool" );
  //corrTool.msg().setLevel( MSG::VERBOSE);
  corrTool.msg().setLevel( MSG::INFO);
  //corrTool.msg().setLevel( MSG::WARNING);
  //ATH_CHECK( corrTool.initialize() );
  //corrTool.setProperty( "Release", "PreRecs" );
  //corrTool.setProperty( "Year", "Data15" );
  corrTool.setProperty( "Year", "Data16" );
  //corrTool.setProperty("Release","Recs2016_08_07");
  corrTool.setProperty("StatComb",true);
  corrTool.setProperty("SagittaCorr",true); 
  //corrTool.setProperty("SagittaRelease","sagittaBiasDataAll_02_08_17"); 
  corrTool.setProperty("SagittaRelease","sagittaBiasDataAll_25_07_17");
  corrTool.setProperty("doSagittaMCDistortion",false);
  corrTool.setProperty("noEigenDecor",false);
  if(corrTool.initialize()!=StatusCode::SUCCESS) return 1;

  

  //::: Muon Selection

  CP::MuonSelectionTool selTool( "MuonSelectionTool" );
  //selTool.setProperty( "MaxEta", 2.5 );
  //ATH_CHECK( selTool.initialize() );
  selTool.initialize();

  //::: Systematics initialization
  std::vector< CP::SystematicSet > sysList;
  //sysList.push_back( CP::SystematicSet() );
  
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  for( CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr ) {
    sysList.push_back( CP::SystematicSet() );
    sysList.back().insert( *sysItr );
  }
  
  std::vector< CP::SystematicSet >::const_iterator sysListItr;

  std::cout<<"Systematics are "<<std::endl;
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr )
    std::cout<<sysListItr->name()<<std::endl;

  //::: Output initialization
  Float_t InitPtCB( 0. ), InitPtID( 0. ), InitPtMS( 0. );
  Float_t CorrPtCB( 0. ), CorrPtID( 0. ), CorrPtMS( 0. );
  Float_t Eta( 0. ), Phi( 0. ), Charge( 0. );
  Float_t ExpResoCB( 0. ), ExpResoID( 0. ), ExpResoMS( 0. );
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
    sysTree->Branch( "ExpResoCB", &ExpResoCB, "ExpResoCB/F" );
    sysTree->Branch( "ExpResoID", &ExpResoID, "ExpResoID/F" );
    sysTree->Branch( "ExpResoMS", &ExpResoMS, "ExpResoMS/F" );

    sysTreeMap[ *sysListItr ] = sysTree;
  }

  //::: Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    if( nEvents!=-1 && entry > nEvents ) break; 
    //::: Tell the object which entry to look at:
    event.getEntry( entry );

    //::: Print some event information for fun:
    const xAOD::EventInfo* evtInfo = 0;
    //ATH_CHECK( event.retrieve( evtInfo, "EventInfo" ) );
    if(event.retrieve( evtInfo, "EventInfo" ) != StatusCode::SUCCESS) continue; 
    if(Ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=Ievent) {
      //std::cout<<"Event "<<evtInfo->eventNumber()<<" Ievent "<<Ievent<<std::endl;
      continue;  
    }


    //Info( APP_NAME, "===>>>  start processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry ) );
    
    //::: Get the Muons from the event:
    const xAOD::MuonContainer* muons = 0;
    //ATH_CHECK( event.retrieve( muons, "Muons" ) );
    if( event.retrieve( muons, "Muons" ) != StatusCode::SUCCESS) continue ;
    //Info( APP_NAME, "Number of muons: %i", static_cast< int >( muons->size() ) );

    // create a shallow copy of the muons container
    std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muons_shallowCopy = xAOD::shallowCopyContainer( *muons );

    // iterate over our shallow copy
    xAOD::MuonContainer::iterator muonSC_itr = ( muons_shallowCopy.first)->begin();
    xAOD::MuonContainer::iterator muonSC_end = ( muons_shallowCopy.first)->end();

    xAOD::MuonContainer* muonsCorr = muons_shallowCopy.first;

    //::: Loop over systematics
    for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
      
      //Info( APP_NAME, "Looking at %s systematic", ( sysListItr->name() ).c_str() );

      //::: Check if systematic is applicable
      if( corrTool.applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) { 
        Error( APP_NAME, "Cannot configure muon calibration tool for systematic" );
      }

      //for( int i=-0; i<1e3 ; i++) { 
      //::: Loop over muon container
      for( auto muon: *muonsCorr ) {

        //::: Select "good" muons:
        // if( ! selTool.passedIDCuts( **mu_itr ) ) {
        //   Info( APP_NAME, "This muon doesn't pass the ID hits quality cuts");
        //   continue;
        // }
        //::: Should be using correctedCopy here, thesting behaviour of applyCorrection though
        InitPtCB = muon->pt();
        //InitPtID = muon->isAvailable< float >( "InnerDetectorPt" ) ? muon->auxdata< float >( "InnerDetectorPt" ) : -999;
        //InitPtMS = muon->isAvailable< float >( "MuonSpectrometerPt" ) ? muon->auxdata< float >( "MuonSpectrometerPt" ) : -999;

        InitPtID = -999;
        if(muon->inDetTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& id_track = muon->inDetTrackParticleLink();
          InitPtID = (!id_track) ?  0:(*id_track)->pt();
        }
        InitPtMS = -999;
        if( muon->extrapolatedMuonSpectrometerTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& ms_track = muon->extrapolatedMuonSpectrometerTrackParticleLink();
          InitPtMS = (!ms_track) ? 0:(*ms_track)->pt();
        }
        
        Eta = muon->eta();
        Phi = muon->phi();
        Charge = muon->charge();
      
        //::: Print some info about the selected muon:
        //Info( APP_NAME, "Selected muon: eta = %g, phi = %g, pt = %g", muon->eta(), muon->phi(), muon->pt()/1e3 );
        
        float ptCB = 0 ;
        if(muon->primaryTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& cb_track = muon->primaryTrackParticleLink();
          ptCB = (!cb_track) ? 0:(*cb_track)->pt();
        }
        else {
          Info( APP_NAME, "Missing primary track particle link for --> CB %g, author: %d, type: %d",ptCB,muon->author(),muon->muonType());
        }
        float ptID = 0; 
        if(muon->inDetTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& id_track = muon->inDetTrackParticleLink();
          ptID = (!id_track) ?  0:(*id_track)->pt();
        }
        float ptME =0 ;
        if( muon->extrapolatedMuonSpectrometerTrackParticleLink().isValid()){
          const ElementLink< xAOD::TrackParticleContainer >& ms_track = muon->extrapolatedMuonSpectrometerTrackParticleLink();
          ptME = (!ms_track) ? 0:(*ms_track)->pt();
        }
       
        //if(entry %  1000 ==0 ) 
        Info( APP_NAME, "--> CB %g, ID %g, ME %g, author: %d, type: %d",ptCB,ptID,ptME,muon->author(),muon->muonType());
        
        //:::
        if( do_it_the_right_way ) {
          //::: Create a calibrated muon:
          xAOD::Muon* mu = 0;
          if( !corrTool.correctedCopy( *muon, mu ) ) {
            Error( APP_NAME, "Cannot really apply calibration nor smearing" );
            continue;
          }
          CorrPtCB = mu->pt();
          CorrPtID = mu->auxdata< float >( "InnerDetectorPt" );
          CorrPtMS = mu->auxdata< float >( "MuonSpectrometerPt" );

          //Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt(CB) = %g, pt(ID) = %g, pt(MS) = %g", mu->eta(), mu->phi(), mu->pt(), mu->auxdata< float >( "InnerDetectorPt" ), mu->auxdata< float >( "MuonSpectrometerPt" ) );
          
          sysTreeMap[ *sysListItr ]->Fill();
          //::: Delete the calibrated muon:
          delete mu;
        }
        else {
          if( !corrTool.applyCorrection( *muon ) ) {
            Error( APP_NAME, "Cannot really apply calibration nor smearing" );
            continue;
          }
          CorrPtCB = muon->pt();
          CorrPtID = muon->auxdata< float >( "InnerDetectorPt" );
          CorrPtMS = muon->auxdata< float >( "MuonSpectrometerPt" );
          ExpResoCB = corrTool.ExpectedResolution( "CB", *muon );
          ExpResoID = corrTool.ExpectedResolution( "ID", *muon );
          ExpResoMS = corrTool.ExpectedResolution( "MS", *muon );
          //if(entry %  1000 ==0 ) 
          Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt(CB) = %g, pt(ID) = %g, pt(MS) = %g", muon->eta(), muon->phi(), muon->pt()/1e3,CorrPtID,CorrPtMS);
          sysTreeMap[ *sysListItr ]->Fill();
        }
      }
      //}
    }
    
    //::: Close with a message:
    //if(entry %  1000 ==0 ) 
      Info( APP_NAME, "===>>>  done processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry + 1 ) );
  }

  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    sysTreeMap[ *sysListItr ]->Write();
  }

  delete chain;

  //::: Close output file
  outputFile->Close();

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  //::: Return gracefully:
  return 0;

}
