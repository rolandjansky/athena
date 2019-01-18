/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
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
#include "AsgTools/AnaToolHandle.h"

#include "PATInterfaces/SystematicCode.h"

// Local include(s):

// calibration
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

// selection
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"



using namespace std;

int main( int argc, char* argv[] ) {

  bool useCorrectedCopy = false;

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if( argc < 2 ) {
    Error( APP_NAME, "==============================================" );
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "Usage: $> %s [xAOD file name]", APP_NAME );
    Error( APP_NAME, " $> %s [xAOD file name]", APP_NAME );
    Error( APP_NAME, " $> %s [xAOD file name] -n X  |  X = number of events you want to run on", APP_NAME );
    Error( APP_NAME, " $> %s [xAOD file name] -event X  |  X = specific number of the event to run on - for debugging", APP_NAME );
    Error( APP_NAME, "==============================================" );
    return 1;
  }

  ////////////////////////////////////////////////////
  //:::  parse the options
  ////////////////////////////////////////////////////
  string options;
  for( int i=0; i<argc; i++){
    options+=(argv[i]);
  }

  int Ievent=-1;
  if(options.find("-event")!=string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(string(argv[ipos]).compare("-event")==0){
        Ievent = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-event) : Running only on event # %i", Ievent );
        break;
      }
    }
  }

  int nEvents=-1;
  if(options.find("-n")!=string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(string(argv[ipos]).compare("-n")==0){
        nEvents = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-n) : Running on NEvents = %i", nEvents );
        break;
      }
    }
  }


  ////////////////////////////////////////////////////
  //:::  initialize the application and get the event
  ////////////////////////////////////////////////////
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
  xAOD::TEvent event( (TTree*)chain, xAOD::TEvent::kAthenaAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );

  //::: Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  //::: Decide how many events to run over:
  Long64_t entries = event.getEntries();
 
  ////////////////////////////////////////////////////
  //::: MuonCalibrationAndSmearingTool
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  //::: create the tool handle
  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> corrTool; //!
  corrTool.setTypeAndName("CP::MuonCalibrationAndSmearingTool/MuonCorrectionTool");
    //::: set the properties
  corrTool.setProperty("Year",                  "Data17" );
  //   corrTool.setProperty("Algo",                  "muons" );
  //   corrTool.setProperty("SmearingType",          "q_pT" );
  //corrTool.setProperty("Release",               "Recs2017_08_02" );
  corrTool.setProperty("Release",               "Recs2018_05_20" );     
  //   corrTool.setProperty("ToroidOff",             false );
  //   corrTool.setProperty("FilesPath",             "" );
  corrTool.setProperty("StatComb",              false);
  //   corrTool.setProperty("MinCombPt",             300.0);
  corrTool.setProperty("SagittaCorr",           false);
  corrTool.setProperty("SagittaRelease",        "sagittaBiasDataAll_30_07_18");
  corrTool.setProperty("doSagittaMCDistortion", true);
  corrTool.setProperty("SagittaCorrPhaseSpace", true);
  //   corrTool.setProperty("sgItersCB",             11);
  //   corrTool.setProperty("sgItersID",             11);
  //   corrTool.setProperty("sgItersME",             11);
  //   corrTool.setProperty("sgIetrsMamual",         false);
  corrTool.setProperty("fixedRho",              0.0);
  corrTool.setProperty("useFixedRho",           true);
  corrTool.setProperty("noEigenDecor" ,         false);
  //   corrTool.setProperty("useExternalSeed" ,      false);
  //   corrTool.setProperty("externalSeed" ,         0);
  
  //::: retrieve the tool
  corrTool.retrieve();

  ////////////////////////////////////////////////////
  //::: MuonSelectionTool
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  //::: create the tool handle
  asg::AnaToolHandle<CP::IMuonSelectionTool> selTool; //!
  selTool.setTypeAndName("CP::MuonSelectionTool/MuonSelectionTool");

  //::: set the properties
//   selTool.setProperty( "MaxEta",              2.5 );
//   selTool.setProperty( "MuQuality",           1 );   //corresponds to 0=Tight, 1=Medium, 2=Loose, 3=VeryLoose, 4=HighPt, 5=LowPtEfficiency
//   selTool.setProperty( "ToroidOff",           false );
//   selTool.setProperty( "TurnOffMomCorr",      false );
//   selTool.setProperty( "CalibrationRelease",  "PreRec2016_2016-04-13" );
//   selTool.setProperty( "ExpertDevelopMode",   false );
//   selTool.setProperty( "CustomInputFolder",   "");
//   selTool.setProperty( "TrtCutOff",           false );
//   selTool.setProperty( "SctCutOff",           false );
//   selTool.setProperty( "PixCutOff",           false );
//   selTool.setProperty( "SiHolesCutOff",       false );
//   selTool.setProperty( "UseAllAuthors",       false );

  //::: retrieve the tool
  selTool.retrieve();

  ////////////////////////////////////////////////////
  //::: Systematics initialization
  ////////////////////////////////////////////////////
  std::vector< CP::SystematicSet > sysList;
  sysList.push_back( CP::SystematicSet() );

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

  // branches to be stored
  Float_t InitPtCB( 0. ), InitPtID( 0. ), InitPtMS( 0. );
  Float_t CorrPtCB( 0. ), CorrPtID( 0. ), CorrPtMS( 0. );
  Float_t Eta( 0. ), Phi( 0. ), Charge( 0. );
  Float_t ExpResoCB( 0. ), ExpResoID( 0. ), ExpResoMS( 0. );

  // output file
  TFile* outputFile = TFile::Open( "output.root", "recreate" );

  // it contains a single TTree per systematic variation
  boost::unordered_map< CP::SystematicSet, TTree* > sysTreeMap;
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {

    // create new tree for the systematic in question
    std::string treeName = "test_tree_" + (sysListItr->name().size()==0 ? "NOMINAL":sysListItr->name());
    TTree* sysTree = new TTree( treeName.c_str(), "test tree for MCAST" );

    // add branches
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

  ////////////////////////////////////////////////////
  //::: Loop over the events
  ////////////////////////////////////////////////////
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    if( nEvents!=-1 && entry > nEvents ) break;
    // Tell the object which entry to look at:
    event.getEntry( entry );

    // Print some event information
    const xAOD::EventInfo* evtInfo = 0;
    if(event.retrieve( evtInfo, "EventInfo" ) != StatusCode::SUCCESS){
      continue;
    }
    if(Ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=Ievent) {
      continue;
    }

    //::: Get the Muons from the event:
    const xAOD::MuonContainer* muons = 0;
    if( event.retrieve( muons, "Muons" ) != StatusCode::SUCCESS)
      continue ;

    //Info( APP_NAME, "Number of muons: %i", static_cast< int >( muons->size() ) );

    // create a shallow copy of the muons container
    std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muons_shallowCopy = xAOD::shallowCopyContainer( *muons );

    // iterate over our shallow copy
    xAOD::MuonContainer::iterator muonSC_itr = ( muons_shallowCopy.first)->begin();
    xAOD::MuonContainer::iterator muonSC_end = ( muons_shallowCopy.first)->end();

    xAOD::MuonContainer* muonsCorr = muons_shallowCopy.first;

    //::: Loop over systematics
    for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {

      Info( APP_NAME, "Looking at %s systematic", ( sysListItr->name() ).c_str() );

      //::: Check if systematic is applicable to the correction tool
      if( corrTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
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

        //::: Should be using correctedCopy here, testing behaviour of applyCorrection though
        InitPtCB = muon->pt();
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

        // either use the correctedCopy call or correct the muon object itself
        if( useCorrectedCopy ) {
          //::: Create a calibrated muon:
          xAOD::Muon* mu = 0;
          if( !corrTool->correctedCopy( *muon, mu ) ) {
            Error( APP_NAME, "Cannot really apply calibration nor smearing" );
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
        else {
          if( !corrTool->applyCorrection( *muon ) ) {
            Error( APP_NAME, "Cannot really apply calibration nor smearing" );
            continue;
          }
          CorrPtCB = muon->pt();
          CorrPtID = muon->auxdata< float >( "InnerDetectorPt" );
          CorrPtMS = muon->auxdata< float >( "MuonSpectrometerPt" );
          ExpResoCB = corrTool->expectedResolution( "CB", *muon, true );
          ExpResoID = corrTool->expectedResolution( "ID", *muon, true );
          ExpResoMS = corrTool->expectedResolution( "MS", *muon, true );
          Info( APP_NAME, "Calibrated muon: eta = %g, phi = %g, pt(CB) = %g, pt(ID) = %g, pt(MS) = %g", muon->eta(), muon->phi(), muon->pt()/1e3,CorrPtID,CorrPtMS);
          Info( APP_NAME, " expReso : ExpResoCB = %g , ExpResoID = %g , ExpResoMS = %g", ExpResoCB, ExpResoID, ExpResoMS);
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
