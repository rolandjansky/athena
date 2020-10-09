/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <unordered_map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include <TChain.h>

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "AsgMessaging/Check.h"
#include "AsgTools/AnaToolHandle.h"
#include "PATInterfaces/SystematicCode.h"

#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

#include "AsgMessaging/MessageCheck.h"

ANA_MSG_HEADER(msgMMC)

int main( int argc, char* argv[] ) {

  // setup for ANA_CHECK()
  using namespace msgMMC;
  ANA_CHECK_SET_TYPE (int);

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
  std::string options;
  for( int i=0; i<argc; i++){
    options+=(argv[i]);
  }

  int Ievent=-1;
  if(options.find("-event")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-event")==0){
        Ievent = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-event) : Running only on event # %i", Ievent );
        break;
      }
    }
  }

  int nEvents=-1;
  if(options.find("-n")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-n")==0){
        nEvents = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-n) : Running on NEvents = %i", nEvents );
        break;
      }
    }
  }


  ////////////////////////////////////////////////////
  //:::  initialize the application and get the event
  ////////////////////////////////////////////////////
  ANA_CHECK (xAOD::Init( APP_NAME ));
  xAOD::TReturnCode::enableFailure();

  //::: Open the input file:
  std::string fileName = argv[1];
  Info( APP_NAME, "Opening file: %s", fileName.c_str());
  std::unique_ptr<TFile> ifile(TFile::Open(fileName.c_str(), "READ"));
  if( !ifile ) Error( APP_NAME, "Cannot find file %s",fileName.c_str());

  std::unique_ptr<TChain> chain = std::make_unique<TChain>("CollectionTree","CollectionTree");
  chain->Add(fileName.c_str());

  //::: Create a TEvent object:
  xAOD::TEvent event( (TTree*)chain.get(), xAOD::TEvent::kAthenaAccess );
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
  StatusCode sc;
  sc &= corrTool.setProperty("Year",                  "Data17" );
  sc &= corrTool.setProperty("Release",               "Recs2018_05_20" );
  sc &= corrTool.setProperty("StatComb",              false);
  sc &= corrTool.setProperty("SagittaCorr",           false);
  sc &= corrTool.setProperty("SagittaRelease",        "sagittaBiasDataAll_30_07_18");
  sc &= corrTool.setProperty("doSagittaMCDistortion", true);
  sc &= corrTool.setProperty("SagittaCorrPhaseSpace", true);
  sc &= corrTool.setProperty("fixedRho",              0.0);
  sc &= corrTool.setProperty("useFixedRho",           true);
  sc &= corrTool.setProperty("noEigenDecor" ,         false);

  //::: retrieve the tool
  sc &= corrTool.retrieve();
  if (sc.isFailure()) {
    Error( APP_NAME, "Cannot retrieve MuonCorrectionTool" );
    return 1;
  }

  ////////////////////////////////////////////////////
  //::: MuonSelectionTool
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  //::: create the tool handle
  asg::AnaToolHandle<CP::IMuonSelectionTool> selTool; //!
  selTool.setTypeAndName("CP::MuonSelectionTool/MuonSelectionTool");

  //::: retrieve the tool
  if (selTool.retrieve().isFailure()) {
    Error( APP_NAME, "Cannot retrieve MuonSelectionTool" );
    return 1;
  }

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

  msgMMC::ANA_MSG_INFO("main() - Systematics are ");
  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr )
    msgMMC::ANA_MSG_INFO(sysListItr->name());

  // branches to be stored
  Float_t InitPtCB( 0. ), InitPtID( 0. ), InitPtMS( 0. );
  Float_t CorrPtCB( 0. ), CorrPtID( 0. ), CorrPtMS( 0. );
  Float_t Eta( 0. ), Phi( 0. ), Charge( 0. );
  Float_t ExpResoCB( 0. ), ExpResoID( 0. ), ExpResoMS( 0. );

  // output file
  TFile* outputFile = TFile::Open( "output.root", "recreate" );

  // it contains a single TTree per systematic variation
  std::unordered_map< CP::SystematicSet, TTree* > sysTreeMap;
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
    ANA_CHECK( event.retrieve( evtInfo, "EventInfo" ));
    if(Ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=Ievent) {
      continue;
    }

    //::: Get the Muons from the event:
    const xAOD::MuonContainer* muons = 0;
    ANA_CHECK( event.retrieve( muons, "Muons" ));

    // create a shallow copy of the muons container
    std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muons_shallowCopy = xAOD::shallowCopyContainer( *muons );

    xAOD::MuonContainer* muonsCorr = muons_shallowCopy.first;

    //::: Loop over systematics
    for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {

      Info( APP_NAME, "Looking at %s systematic", ( sysListItr->name() ).c_str() );

      //::: Check if systematic is applicable to the correction tool
      if( corrTool->applySystematicVariation( *sysListItr ) != CP::SystematicCode::Ok ) {
        Error( APP_NAME, "Cannot configure muon calibration tool for systematic" );
      }

      //::: Loop over muon container
      for( auto muon: *muonsCorr ) {

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
    }

    //::: Close with a message:
      Info( APP_NAME, "===>>>  done processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry + 1 ) );
  }

  for( sysListItr = sysList.begin(); sysListItr != sysList.end(); ++sysListItr ) {
    sysTreeMap[ *sysListItr ]->Write();
  }

  //::: Close output file
  outputFile->Close();

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();


  //::: Return gracefully:
  return 0;

}
