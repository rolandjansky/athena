/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>

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
#include <xAODJet/JetContainer.h>
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODTracking/VertexContainer.h"
#include "AsgTools/Check.h"
#include "AsgTools/AnaToolHandle.h"
#include "PATCore/TAccept.h"

// Tool testing include(s):
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetSelector.h"
#include "BoostedJetTaggers/DeepsetXbbTagger.h"

using namespace std;

int main( int argc, char* argv[] ) {

  // The application's name:
  char* APP_NAME = argv[ 0 ];

  // arguments
  TString fileName = "/eos/user/y/yuchou/DxAOD/mc16_13TeV.346115.PowhegPy8EG_ggZH_H125_a20a20_4b.deriv.DAOD_FTAG1.e7370_a875_r9364_p4239/DAOD_FTAG1.22347060._000007.pool.root.1"; 
  int  ievent=-1;
  int  nevents=-1;
  bool verbose=false;


  Info( APP_NAME, "==============================================" );
  Info( APP_NAME, "Usage: $> %s [xAOD file name]", APP_NAME );
  Info( APP_NAME, " $> %s       | Run on default file", APP_NAME );
  Info( APP_NAME, " $> %s -f X  | Run on xAOD file X", APP_NAME );
  Info( APP_NAME, " $> %s -n X  | X = number of events you want to run on", APP_NAME );
  Info( APP_NAME, " $> %s -e X  | X = specific number of the event to run on - for debugging", APP_NAME );
  Info( APP_NAME, " $> %s -v    | run in verbose mode   ", APP_NAME );
  Info( APP_NAME, "==============================================" );

  // Check if we received a file name:
  if( argc < 2 ) {
    Info( APP_NAME, "No arguments - using default file" );
    Info( APP_NAME, "Executing on : %s", fileName.Data() );
  }

  ////////////////////////////////////////////////////
  //:::  parse the options
  ////////////////////////////////////////////////////
  string options;
  for( int i=0; i<argc; i++){
    options+=(argv[i]);
  }

  if(options.find("-f")!=string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(string(argv[ipos]).compare("-f")==0){
        fileName = argv[ipos+1];
        Info( APP_NAME, "Argument (-f) : Running on file # %s", fileName.Data() );
        break;
      }
    }
  }

  if(options.find("-event")!=string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(string(argv[ipos]).compare("-event")==0){
        ievent = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-event) : Running only on event # %i", ievent );
        break;
      }
    }
  }

  if(options.find("-n")!=string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(string(argv[ipos]).compare("-n")==0){
        nevents = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-n) : Running on NEvents = %i", nevents );
        break;
      }
    }
  }

  if(options.find("-v")!=string::npos){
    verbose=true;
    Info( APP_NAME, "Argument (-v) : Setting verbose");
  }


  ////////////////////////////////////////////////////
  //:::  initialize the application and get the event
  ////////////////////////////////////////////////////
  xAOD::Init( APP_NAME );
  xAOD::TReturnCode::enableFailure();

  // Open the input file:
  TFile* ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile ) Error( APP_NAME, "Cannot find file %s",fileName.Data() );

  TChain *chain = new TChain ("CollectionTree","CollectionTree");
  chain->Add(fileName);

  // Create a TEvent object:
  xAOD::TEvent event( (TTree*)chain, xAOD::TEvent::kAthenaAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // Decide how many events to run over:
  Long64_t entries = event.getEntries();

  // Fill a validation true with the tag return value
  std::unique_ptr<TFile> outputFile(TFile::Open( "output_Dexter.root", "recreate" ));
  int pass;
  float dexter_pbb, dexter_pb, dexter_pl;
  TTree* Tree = new TTree( "tree", "test_tree" );
  Tree->Branch( "pass", &pass, "pass/I" );
  Tree->Branch( "dexter_pbb", &dexter_pbb, "dexter_pb/F" );
  Tree->Branch( "dexter_pb", &dexter_pb, "dexter_pb/F" );
  Tree->Branch( "dexter_pl", &dexter_pl, "dexter_pb/F" );

  ////////////////////////////////////////////
  /////////// START TOOL SPECIFIC ////////////
  ////////////////////////////////////////////

  ////////////////////////////////////////////////////
  //::: Tool setup
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  std::cout<<"Initializing Dexter"<<std::endl;
  asg::AnaToolHandle<Dexter> m_Tagger; //!
  m_Tagger.setTypeAndName("Dexter","Dexter");

  if(verbose) m_Tagger.setProperty("OutputLevel", MSG::VERBOSE);
  m_Tagger.setProperty( "neuralNetworkFile","/eos/user/y/yuchou/Dexter/Models/nn-config.json");
  m_Tagger.setProperty( "tagThreshold", 0.5);
  auto status_code = m_Tagger.retrieve();
  if (status_code.isFailure()) {
    return 1;
  }

  ////////////////////////////////////////////////////
  // Loop over the events
  ////////////////////////////////////////////////////
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    if( nevents!=-1 && entry > nevents ) break;
    // Tell the object which entry to look at:
    event.getEntry( entry );

    // Print some event information
    const xAOD::EventInfo* evtInfo = 0;
    if(event.retrieve( evtInfo, "EventInfo" ) != StatusCode::SUCCESS){
      return 1;
    }
    if(ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=ievent) {
      continue;
    }

    // Get the jets & msv collections
    const xAOD::JetContainer *myJets = nullptr;
    std::string jc_name = "AntiKt8EMPFlowJets";
    auto sc_jet = event.retrieve( myJets, jc_name );
    
    if (sc_jet != StatusCode::SUCCESS) return 1;

    // Loop over jet container
    for(const xAOD::Jet* jet : *myJets ){

      if (m_Tagger->n_subjets(*jet) == 2 && jet->pt() > 20e3) {
        auto scores = m_Tagger->getScores(*jet);
        if (verbose) {
          std::cout << "BB Score: " << scores.at("dexter_pbb") << std::endl;
          std::cout << "B Score: " << scores.at("dexter_pb") << std::endl;
          std::cout << "Light Score: " << scores.at("dexter_pl") << std::endl;
        }
        bool res = m_Tagger->keep( *jet );
        pass = res;
        dexter_pbb = scores.at("dexter_pbb");
        dexter_pb = scores.at("dexter_pb");
        dexter_pl = scores.at("dexter_pl");
        Tree->Fill();
      }
    }

    Info( APP_NAME, "===>>>  done processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry + 1 ) );
  }

  ////////////////////////////////////////////
  /////////// END TOOL SPECIFIC //////////////
  ////////////////////////////////////////////

  // write the tree to the output file
  outputFile->cd();
  Tree->Write();
  outputFile->Close();

  // cleanup
  delete chain;

  // print the branches that were used for help with smart slimming
  std::cout<<std::endl<<std::endl;
  std::cout<<"Smart Slimming Checker :"<<std::endl;
  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();
  std::cout<<std::endl<<std::endl;

  return 0;

}

