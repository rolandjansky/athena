/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "AsgTools/Check.h"
#include "AsgTools/AnaToolHandle.h"
#include "PATCore/TAccept.h"

// Tool testing include(s):
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetSelector.h"
#include "BoostedJetTaggers/IJetSelectorLabelTool.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "JetUncertainties/JetUncertaintiesTool.h"

int main( int argc, char* argv[] ) {

  // The application's name:
  char* APP_NAME = argv[ 0 ];

  // arguments
  TString fileName = "/eos/atlas/atlascerngroupdisk/perf-jets/ReferenceFiles/mc16_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.deriv.DAOD_FTAG1.e3569_s3126_r9364_r9315_p3260/DAOD_FTAG1.12133096._000074.pool.root.1";
  int  ievent=-1;
  int  nevents=-1;
  bool m_IsMC=true;
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

  if(options.find("-m")!=std::string::npos){
    for( int ipos=0; ipos<argc ; ipos++ ) {
      if(std::string(argv[ipos]).compare("-m")==0){
        m_IsMC = atoi(argv[ipos+1]);
        Info( APP_NAME, "Argument (-m) : IsMC = %i", m_IsMC );
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
  std::unique_ptr<TFile> outputFile(TFile::Open("output_JSSWTopTaggerDNN.root", "recreate"));
  int pass,truthLabel,idx;
  float sf,pt,eta,m;
  TTree* Tree = new TTree( "tree", "test_tree" );
  Tree->Branch( "pass", &pass, "pass/I" );
  Tree->Branch( "sf", &sf, "sf/F" );
  Tree->Branch( "pt", &pt, "pt/F" );
  Tree->Branch( "m", &m, "m/F" );
  Tree->Branch( "eta", &eta, "eta/F" );
  Tree->Branch( "idx", &idx, "idx/I" );
  Tree->Branch( "truthLabel", &truthLabel, "truthLabel/I" );

  std::unique_ptr<JetUncertaintiesTool> m_jetUncToolSF(new JetUncertaintiesTool(("JetUncProvider_SF")));
  m_jetUncToolSF->setProperty("JetDefinition", "AntiKt10LCTopoTrimmedPtFrac5SmallR20");
  //m_jetUncToolSF->setProperty("ConfigFile", "rel21/Summer2019/R10_SF_LC_DNNContained80_TopTag.config");
  m_jetUncToolSF->setProperty("ConfigFile", "/afs/cern.ch/user/t/tnobe/workDir/makeConfig/makebjtconfigs/outputs/temp_R10_SF_DNNTaggerTopQuarkContained80_SF.config");
  m_jetUncToolSF->setProperty("MCType", "MC16");
  m_jetUncToolSF->initialize();

  std::vector<std::string> pulls = {"__1down", "__1up"};
  CP::SystematicSet jetUnc_sysSet = m_jetUncToolSF->recommendedSystematics();
  const std::set<std::string> sysNames = jetUnc_sysSet.getBaseNames();
  std::vector<CP::SystematicSet> m_jetUnc_sysSets;
  for (std::string sysName: sysNames) {
    for (std::string pull : pulls) {
      std::string sysPulled = sysName + pull;
      m_jetUnc_sysSets.push_back(CP::SystematicSet(sysPulled));
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
  std::cout<<"Initializing JSSWTopTaggerDNN Tagger"<<std::endl;
  asg::AnaToolHandle<IJetSelectorTool> m_Tagger; //!
  ASG_SET_ANA_TOOL_TYPE( m_Tagger, JSSWTopTaggerDNN);
  m_Tagger.setName("MyTagger");
  if(verbose) m_Tagger.setProperty("OutputLevel", MSG::DEBUG);
  m_Tagger.setProperty( "CalibArea",   "Local");
  m_Tagger.setProperty( "ConfigFile",   "JSSWTopTaggerDNN/temp_JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_80Eff.dat");
  m_Tagger.setProperty( "UseTRUTH3", false);
  //m_Tagger.setProperty( "CalibArea",   "JSSWTopTaggerDNN/Rel21");
  //m_Tagger.setProperty( "ConfigFile",   "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_20190405_80Eff.dat");
  m_Tagger.setProperty("IsMC", m_IsMC);
  m_Tagger.retrieve();


  std::cout << "Total Events in File : " << entries << std::endl;

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
    idx=0;
    for( xAOD::Jet* jetSC : *shallowJets ){
      
      if(verbose) std::cout<<"Testing top Tagger "<<std::endl;
      const Root::TAccept& res = m_Tagger->tag( *jetSC );
      if(verbose) std::cout<<"jet pt              = "<<jetSC->pt()<<std::endl;
      if(verbose) std::cout<<"RunningTag : "<<res<<std::endl;
      if(verbose) std::cout<<"Printing jet score : " << jetSC->auxdata<float>("DNNTaggerTopQuarkContained80_Score") << std::endl;
      if(verbose) std::cout<<"result masspasslow  = "<<res.getCutResult("PassMassLow")<<std::endl;
      if(verbose) std::cout<<"result masspasshigh = "<<res.getCutResult("PassMassHigh")<<std::endl;
      truthLabel = jetSC->auxdata<int>("R10TruthLabel_R21Consolidated");

      pass = res;
      sf = jetSC->auxdata<float>("DNNTaggerTopQuarkContained80_SF");
      pt = jetSC->pt();
      m  = jetSC->m();
      eta = jetSC->eta();

      Tree->Fill();
      idx++;
      if ( m_IsMC ){
	if ( jetSC->pt() > 350e3 && fabs(jetSC->eta()) < 2.0  ) {
	  bool validForUncTool = (pt >= 150e3 && pt < 2500e3);
	  validForUncTool &= (m/pt >= 0 && m/pt <= 1);
	  validForUncTool &= (fabs(eta) < 2);
	  std::cout << "Pass: " << pass << std::endl;
	  std::cout << "Nominal SF=" << sf << " truthLabel=" << truthLabel << " (1: t->qqb) " 
		    <<  jetSC->auxdata<float>("DNNTaggerTopQuarkContained80_effSF") 
		    << " "
		    <<  jetSC->auxdata<float>("DNNTaggerTopQuarkContained80_efficiency") 
		    << std::endl;
	  if( validForUncTool ){
	    for ( CP::SystematicSet sysSet : m_jetUnc_sysSets ){
	      m_Tagger->tag( *jetSC );
	      m_jetUncToolSF->applySystematicVariation(sysSet);
	      m_jetUncToolSF->applyCorrection(*jetSC);
	      std::cout << sysSet.name() << " " << jetSC->auxdata<float>("DNNTaggerTopQuarkContained80_SF") << std::endl;
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

