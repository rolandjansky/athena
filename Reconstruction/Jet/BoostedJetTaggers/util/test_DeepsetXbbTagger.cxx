/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TChain.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/tools/IOStats.h"

// Tool testing include(s):
#include "BoostedJetTaggers/DeepsetXbbTagger.h"

using namespace std;

int main( int argc, char* argv[] ) {

  // The application's name:
  char* APP_NAME = argv[ 0 ];

  // arguments
  // Current only tested with FTAG1
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
  std::string options;
  for( int i=0; i<argc; i++){
    options+=(argv[i]);
  }

  if(options.find("-f")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-f")==0){
        fileName = argv[ipos+1];
        Info( APP_NAME, "Argument (-f) : Running on file # %s", fileName.Data() );
        break;
      }
    }
  }

  if(options.find("-event")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-event")==0){
        ievent = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-event) : Running only on event # %i", ievent );
        break;
      }
    }
  }

  if(options.find("-n")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-n")==0){
        nevents = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-n) : Running on NEvents = %i", nevents );
        break;
      }
    }
  }

  if(options.find("-v")!=std::string::npos){
    verbose=true;
    Info( APP_NAME, "Argument (-v) : Setting verbose");
  }


  ////////////////////////////////////////////////////
  //:::  initialize the application and get the event
  ////////////////////////////////////////////////////
  xAOD::Init( APP_NAME );
  xAOD::TReturnCode::enableFailure();

  // Open the input file:
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
  float dexter_pbb, dexter_pb, dexter_pl, dexter_Dbb;
  TTree* Tree = new TTree( "tree", "test_tree" );
  Tree->Branch( "pass", &pass, "pass/I" );
  Tree->Branch( "dexter_pbb", &dexter_pbb, "dexter_pbb/F" );
  Tree->Branch( "dexter_pb", &dexter_pb, "dexter_pb/F" );
  Tree->Branch( "dexter_pl", &dexter_pl, "dexter_pl/F" );
  Tree->Branch( "dexter_Dbb", &dexter_Dbb, "dexter_Dbb/F" );
  // free parameters b-jet fraction 
  float f_b = 0.4; 

  ////////////////////////////////////////////
  /////////// START TOOL SPECIFIC ////////////
  ////////////////////////////////////////////

  ////////////////////////////////////////////////////
  //::: Tool setup
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  std::cout<<"Initializing DeepsetXbbTagger"<<std::endl;
  asg::AnaToolHandle<DeepsetXbbTagger> m_Tagger; //!
  m_Tagger.setTypeAndName("DeepsetXbbTagger","DeepsetXbbTagger");
  if(verbose) m_Tagger.setProperty("OutputLevel", MSG::VERBOSE);
  m_Tagger.setProperty( "KerasConfigFile","/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/BTagging/DeepsetXbbTagger/202011/nn-config.json");
  m_Tagger.setProperty( "ConfigFile" ,"BoostedJetTaggers/DeepsetXbbTagger/test_config.json");
  // m_Tagger.setProperty( "negativeTagMode", "SVMassNegTrksFlip");  // Set this property to run in flipped tag mode 
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
      const Root::TAccept& res = m_Tagger->tag( *jet ); 
      pass = res;
      if (verbose) std::cout << ( (res.getCutResult("ValidJet")) ? "Valid jet" : "Invalid jet" ) << std::endl;
      auto scores = m_Tagger->getScores(*jet);
      if (verbose) {
        std::cout << "BB Score: " << scores.at("dexter_pbb") << std::endl;
        std::cout << "B Score: " << scores.at("dexter_pb") << std::endl;
        std::cout << "Light Score: " << scores.at("dexter_pl") << std::endl;
        std::cout << "Dbb Score: " << log(scores.at("dexter_pbb") / (f_b * scores.at("dexter_pb") + (1- f_b) * scores.at("dexter_pl") ) ) << std::endl;
      }
      dexter_pbb = scores.at("dexter_pbb");
      dexter_pb = scores.at("dexter_pb");
      dexter_pl = scores.at("dexter_pl");
      dexter_Dbb = log(scores.at("dexter_pbb") / (f_b * scores.at("dexter_pb") + (1- f_b) * scores.at("dexter_pl") ) );
      Tree->Fill();
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
