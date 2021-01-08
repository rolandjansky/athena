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
#include "BoostedJetTaggers/JetQGTaggerBDT.h"

// messaging
ANA_MSG_HEADER(Test)
ANA_MSG_SOURCE(Test, "BoostedJetTaggers")
using namespace Test;

int main( int argc, char* argv[] ) {

  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  // The application's name:
  char* APP_NAME = argv[ 0 ];

  // arguments
  TString fileName = "/eos/atlas/atlascerngroupdisk/perf-jets/ReferenceFiles/mc16_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.deriv.DAOD_FTAG1.e3569_s3126_r9364_r9315_p3260/DAOD_FTAG1.12133096._000074.pool.root.1";
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
  if(! xAOD::Init( APP_NAME ) )
    return 1;
  StatusCode::enableFailure();

  // Open the input file:
  std::unique_ptr<TFile> ifile( TFile::Open( fileName, "READ" ) );
  if( !ifile ){
    Error( APP_NAME, "Cannot find file %s",fileName.Data() );
    return 1;
  }
  ifile->Close();
  ifile.reset( TFile::Open( fileName, "READ" ) );

  // Create a TEvent object:
  xAOD::TEvent event( ifile.get(), xAOD::TEvent::kAthenaAccess );
  Info( APP_NAME, "Number of events in the file: %i", static_cast< int >( event.getEntries() ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // Decide how many events to run over:
  Long64_t entries = event.getEntries();

  // Fill a validation true with the tag return value
  std::unique_ptr<TFile> outputFile(TFile::Open( "output_JetQGTaggerBDT.root", "recreate" ));
  int pass;
  TTree* Tree = new TTree( "tree", "test_tree" );
  Tree->SetDirectory( outputFile.get() );
  Tree->Branch( "pass", &pass, "pass/I" );

  ////////////////////////////////////////////
  /////////// START TOOL SPECIFIC ////////////
  ////////////////////////////////////////////

  ////////////////////////////////////////////////////
  //::: Tool setup
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  std::cout<<"Initializing QG BDT Tagger"<<std::endl;
  asg::AnaToolHandle<CP::JetQGTaggerBDT> m_Tagger; //!
  m_Tagger.setName("MyTagger");
  if(verbose) ANA_CHECK( m_Tagger.setProperty("OutputLevel", MSG::DEBUG) );
  if(! m_Tagger.setProperty( "ConfigFile", "JetQGTaggerBDT/JetQGTaggerBDT50Gluon.dat") )  return 1;
  if(! m_Tagger.setProperty( "UseJetVars", 0) )  return 1;
  if(! m_Tagger.retrieve() )  return 1;

  ////////////////////////////////////////////////////
  // Loop over the events
  ////////////////////////////////////////////////////

  //weighted number of quark, gluon jets and tags
  float w_nGluon = 0.;
  float w_nQuark = 0.;
  float w_nGluonTaggedANDisGluon = 0.;
  float w_nQuarkTaggedANDisQuark = 0.;
  float w_nGluonTaggedANDisQuark = 0.;
  float w_nQuarkTaggedANDisGluon = 0.;
  float eventweight = 0.;

  for( Long64_t entry = 0; entry < entries; ++entry ) {

    if( nevents!=-1 && entry > nevents ) break;
    // Tell the object which entry to look at:
    event.getEntry( entry );

    // Print some event information
    const xAOD::EventInfo* evtInfo = 0;
    if( ! event.retrieve( evtInfo, "EventInfo" ).isSuccess() ) {
      continue;
    }
    if(ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=ievent) {
      continue;
    }
    eventweight = evtInfo->mcEventWeight();

    // Get the jets
    const xAOD::JetContainer* myJets = 0;
    if( ! event.retrieve( myJets, "AntiKt4EMTopoJets" ).isSuccess() ) {
      continue ;
    }

    // Loop over jet container
    for(const xAOD::Jet* jet : * myJets ){
      int truthlabel = jet->getAttribute<int>("PartonTruthLabelID");
      if(jet->pt()<20000 || TMath::Abs(jet->eta())>2.5 || truthlabel==-1 || truthlabel==5)
    	continue;
      if(verbose) std::cout<<std::endl;

      ANA_CHECK( m_Tagger->tag( *jet ) );
      if(verbose) {
        std::cout << "Testing QG BDT Tagger " << std::endl;
        std::cout << "RunningTag : " << jet->auxdata<bool>("Tagged") << " jet truth label: " << truthlabel << std::endl;
      }
      
      //--------------------------------------------------------------------------------
      switch (truthlabel) {
      case -1:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
        w_nQuark += eventweight;
        if(jet->auxdata<bool>("Tagged")==1){
          w_nQuarkTaggedANDisQuark += eventweight;
        }
        else{
          w_nGluonTaggedANDisQuark += eventweight;
        }
        break;
      case 5:
        break;
      case 21:
        w_nGluon += eventweight;
        if(jet->auxdata<bool>("Tagged")==1){
          w_nQuarkTaggedANDisGluon += eventweight;
        }
        else{
          w_nGluonTaggedANDisGluon += eventweight;
        }
        break;
      default:
        break;
      }
      //--------------------------------------------------------------------------------

      pass = jet->auxdata<bool>("Tagged");

      Tree->Fill();
    }

    if( nevents!=-1 && entry%100 == 0) Info( APP_NAME, "===>>>  done processing event #%i, run #%i %i events processed so far  <<<===", static_cast< int >( evtInfo->eventNumber() ), static_cast< int >( evtInfo->runNumber() ), static_cast< int >( entry + 1 ) );
  }
  ////////////////////////////////////////////
  /////////// END TOOL SPECIFIC //////////////
  ////////////////////////////////////////////

  if(verbose){
    std::cout<<"gluons: "<<w_nGluon<<"\n";
    std::cout<<"lights: "<<w_nQuark<<"\n";
    std::cout<<"efficiencies for gluon WP:"<<"\n";
    std::cout<<"gluon efficiency: "<<w_nGluonTaggedANDisGluon/w_nGluon<<"\n";
    std::cout<<"quark efficiency: "<<w_nGluonTaggedANDisQuark/w_nQuark<<"\n";
    std::cout<<"efficiencies for quark WP:"<<"\n";
    std::cout<<"quark efficiency: "<<w_nQuarkTaggedANDisQuark/w_nQuark<<"\n";
    std::cout<<"gluon efficiency: "<<w_nQuarkTaggedANDisGluon/w_nGluon<<"\n";
  }

  // write the tree to the output file
  outputFile->cd();
  Tree->Write();
  outputFile->Close();

  // print the branches that were used for help with smart slimming
  std::cout<<std::endl<<std::endl;
  std::cout<<"Smart Slimming Checker :"<<std::endl;
  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();
  std::cout<<std::endl<<std::endl;

  return 0;

}

