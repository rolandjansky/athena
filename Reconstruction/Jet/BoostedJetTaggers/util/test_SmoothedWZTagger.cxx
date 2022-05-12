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
#include "AsgTools/StandaloneToolHandle.h"

// EDM include(s):
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/tools/IOStats.h"

// Tool testing include(s):
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "JetUncertainties/JetUncertaintiesTool.h"

#include "AsgMessaging/MessageCheck.h"

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
  bool isMC=true;
  bool verbose=false;


  Info( APP_NAME, "==============================================" );
  Info( APP_NAME, "Usage: $> %s [xAOD file name]", APP_NAME );
  Info( APP_NAME, " $> %s       | Run on default file", APP_NAME );
  Info( APP_NAME, " $> %s -f X  | Run on xAOD file X", APP_NAME );
  Info( APP_NAME, " $> %s -n X  | X = number of events you want to run on", APP_NAME );
  Info( APP_NAME, " $> %s -e X  | X = specific number of the event to run on - for debugging", APP_NAME );
  Info( APP_NAME, " $> %s -d X  | X = dataset ID", APP_NAME );
  Info( APP_NAME, " $> %s -m X  | X = isMC", APP_NAME );
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
	if( ipos+1 == argc || std::string(argv[ipos+1])[0]=='-' ) {
	  Error( APP_NAME, "Please add the file name after -f argument" );
	  return 1;
	}
        fileName = argv[ipos+1];
        Info( APP_NAME, "Argument (-f) : Running on file # %s", fileName.Data() );
        break;
      }
    }
  }

  if(options.find("-event")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-event")==0){
	if( ipos+1 == argc || std::string(argv[ipos+1])[0]=='-' ) {
	  Error( APP_NAME, "Please add the event# after -event argument" );
	  return 1;
	}
        ievent = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-event) : Running only on event # %i", ievent );
        break;
      }
    }
  }

  if(options.find("-m")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-m")==0){
	if( ipos+1 == argc || std::string(argv[ipos+1])[0]=='-' ) {
	  Error( APP_NAME, "Please add 0 or 1 after -m (IsMC) argument" );
	  return 1;
	}
        isMC = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-m) : IsMC = %i", isMC );
        break;
      }
    }
  }

  if(options.find("-n")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-n")==0){
	if( ipos+1 == argc || std::string(argv[ipos+1])[0]=='-' ) {
	  Error( APP_NAME, "Please add NEvents after -n argument" );
	  return 1;
	}
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
  ANA_CHECK( xAOD::Init( APP_NAME ) );
  StatusCode::enableFailure();

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
  std::unique_ptr<TFile> outputFile(TFile::Open("output_SmoothedWZTagger.root", "recreate"));
  int pass,truthLabel,ntrk;
  float sf,pt,eta,m,eff,effSF,sigeffSF;
  TTree* Tree = new TTree( "tree", "test_tree" );
  Tree->Branch( "pass", &pass, "pass/I" );
  Tree->Branch( "sf", &sf, "sf/F" );
  Tree->Branch( "pt", &pt, "pt/F" );
  Tree->Branch( "m", &m, "m/F" );
  Tree->Branch( "eta", &eta, "eta/F" );
  Tree->Branch( "ntrk", &ntrk, "ntrk/I" );
  Tree->Branch( "eff", &eff, "eff/F" );
  Tree->Branch( "effSF", &effSF, "effSF/F" );  
  Tree->Branch( "sigeffSF", &sigeffSF, "sigeffSF/F" );  
  Tree->Branch( "truthLabel", &truthLabel, "truthLabel/I" );

  std::unique_ptr<JetUncertaintiesTool> jetUncToolSF(new JetUncertaintiesTool(("JetUncProvider_SF")));
  ANA_CHECK( jetUncToolSF->setProperty("JetDefinition", "AntiKt10LCTopoTrimmedPtFrac5SmallR20") );
  ANA_CHECK( jetUncToolSF->setProperty("ConfigFile", "rel21/Fall2020/R10_SF_LCTopo_WTag_SigEff50.config") );
  ANA_CHECK( jetUncToolSF->setProperty("MCType", "MC16") );
  ANA_CHECK( jetUncToolSF->initialize() );

  std::vector<std::string> pulls = {"__1down", "__1up"};
  CP::SystematicSet jetUnc_sysSet = jetUncToolSF->recommendedSystematics();
  const std::set<std::string> sysNames = jetUnc_sysSet.getBaseNames();
  std::vector<CP::SystematicSet> jetUnc_sysSets;
  for (std::string sysName: sysNames) {
    for (std::string pull : pulls) {
      std::string sysPulled = sysName + pull;
      jetUnc_sysSets.push_back(CP::SystematicSet(sysPulled));
    }
  }  
  
  ////////////////////////////////////////////
  /////////// START TOOL SPECIFIC ////////////
  ////////////////////////////////////////////

  ////////////////////////////////////////////////////
  //::: Tool setup
  // setup the tool handle as per the
  // recommendation by ASG - https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_use_AnaToolHandle
  ////////////////////////////////////////////////////
  std::cout<<"Initializing WZ Tagger"<<std::endl;
  asg::StandaloneToolHandle<SmoothedWZTagger> m_Tagger; //!
  m_Tagger.setTypeAndName("SmoothedWZTagger/MyTagger");
  if(verbose) ANA_CHECK( m_Tagger.setProperty("OutputLevel", MSG::DEBUG) );
  ANA_CHECK( m_Tagger.setProperty( "CalibArea", "SmoothedWZTaggers/Rel21/") );
  ANA_CHECK( m_Tagger.setProperty( "ConfigFile",   "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat") );
  ANA_CHECK( m_Tagger.setProperty( "IsMC", isMC ) );
  ANA_CHECK( m_Tagger.retrieve() );

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
      continue;
    }
    if(ievent!=-1 && static_cast <int> (evtInfo->eventNumber())!=ievent) {
      continue;
    }

    // Get the jets
    const xAOD::JetContainer* myJets = 0;
    if( event.retrieve( myJets, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" ) != StatusCode::SUCCESS)
      continue ;

    // Loop over jet container
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *myJets );
    std::unique_ptr<xAOD::JetContainer> shallowJets(jets_shallowCopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> shallowAux(jets_shallowCopy.second);
    for( xAOD::Jet* jetSC : *shallowJets ){

      ANA_CHECK( m_Tagger->tag( *jetSC ) );
      if(verbose) {
        std::cout << "Testing W Tagger " << std::endl;
        std::cout << "jet pt              = " << jetSC->pt() << std::endl;
        std::cout << "jet ntrk            = " << jetSC->auxdata<int>("ParentJetNTrkPt500") << std::endl;
        std::cout << "RunningTag : " << jetSC->auxdata<bool>("SmoothWContained50_Tagged") << std::endl;
        std::cout << "result d2pass       = " << jetSC->auxdata<bool>("SmoothWContained50_PassD2") << std::endl;
        std::cout << "result ntrkpass     = " << jetSC->auxdata<bool>("SmoothWContained50_PassNtrk") << std::endl;
        std::cout << "result masspass     = " << jetSC->auxdata<bool>("SmoothWContained50_PassMass") << std::endl;
      }
      truthLabel = jetSC->auxdata<int>("R10TruthLabel_R21Consolidated");

      pass = jetSC->auxdata<bool>("SmoothWContained50_Tagged");
      pt = jetSC->pt();
      m  = jetSC->m();
      eta = jetSC->eta();
      ntrk = jetSC->auxdata<int>("ParentJetNTrkPt500");
      sf = jetSC->auxdata<float>("SmoothWContained50_SF");
      eff = jetSC->auxdata<float>("SmoothWContained50_efficiency");
      effSF = jetSC->auxdata<float>("SmoothWContained50_effSF");
      sigeffSF = jetSC->auxdata<float>("SmoothWContained50_sigeffSF");
      std::cout << "pass " << pass
		<< " truthLabel " << truthLabel
		<< " sf " << sf
		<< " pt " << pt*0.001
		<< " eta " << eta
		<< " m " << m*0.001
		<< " eff " << eff
		<< " effSF " << effSF
		<< " sigeffSF " << sigeffSF
		<< std::endl;

      Tree->Fill();

      if ( isMC ){
	if ( pt/1.e3 > 150 && std::abs(jetSC->eta()) < 2.0 ) {
	  bool validForUncTool = ( pt/1.e3 >= 150 && pt/1.e3 < 2500 && m > 40000. && m < 700000.);
	  validForUncTool &= ( m/pt >= 0 && m/pt <= 1 );
	  validForUncTool &= ( std::abs(eta) < 2 );
	  std::cout << "Pass: " << pass << std::endl;
	  std::cout << "Nominal SF=" << sf << " truthLabel=" << truthLabel << " (1: t->qqb) " 
		    << effSF << " " << eff << " " << pass << std::endl;     
	  if( validForUncTool ){
	    for ( CP::SystematicSet sysSet : jetUnc_sysSets ){
	      ANA_CHECK( m_Tagger->tag( *jetSC ) );
	      ANA_CHECK( jetUncToolSF->applySystematicVariation(sysSet) );
	      ANA_CHECK( jetUncToolSF->applyCorrection(*jetSC) );
	      std::cout << sysSet.name() << " " << jetSC->auxdata<float>("SmoothWContained50_SF") << std::endl;
	    }
	  }
	}
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

