/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Simple class for working with truth DAODs */
/* Goal is to validate truth DAOD contents:
    MET_Truth
    Truth particles with aux: TruthElectrons, TruthMuons, TruthPhotons, TruthTaus, TruthNeutrinos, TruthBSM, TruthBottom, TruthTop, TruthBoson
    Full collections with aux: TruthWbosonWithDecayParticles, TruthWbosonWithDecayVertices
    AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TrueFlavor // with Aux
    AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2 // with Aux
    TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.crossSection
    xAOD::TruthMetaDataContainer#TruthMetaData // with Aux
   Plus extra features
*/

// Setup for reading ATLAS data
#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#include "GaudiKernel/SmartIF.h"
#endif

// Boost for argument parsing
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Truth weight tool
#include "PATInterfaces/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"
#include "PMGAnalysisInterfaces/IPMGTruthWeightTool.h"
//#include "PMGTools/PMGTruthWeightTool.h"

// ATLAS data dependencies
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthMetaDataContainer.h"

// ROOT dependencies
#include <TFile.h>
#include <TChain.h>
#include <TH1D.h>

// STL includes
#include <iostream>
#include <cmath>

// Helper macro for retrieving containers from the event
#define CHECK_RETRIEVE( container , name ) { \
    if (!event.retrieve( container , name ).isSuccess()){ \
      Error( APP_NAME , "Could not load event %s from the file!" , name ); \
      throw std::runtime_error("Container retrieval failed"); \
    } \
  }

// Helper for counting children
int countChildren( const xAOD::TruthParticle * p ){
  if (!p) return 0;
  int children = 0;
  for (size_t n=0;n<p->nChildren();++n){
    children += countChildren( p->child(n) );
  }
  return children;
}

// Helper for counting parents
int countParents( const xAOD::TruthParticle * p ){
  if (!p) return 0;
  int parents = 0;
  for (size_t n=0;n<p->nParents();++n){
    parents += countParents( p->parent(n) );
  }
  return parents;
}

// Main routine... here we go!
int main(int argc, char **argv) {

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Make sure things know we are not in StatusCode land
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  // Setup for reading -- if this fails, we have major problems
#ifdef XAOD_STANDALONE
  if ( ! xAOD::Init().isSuccess() ) {
    throw std::runtime_error("Cannot initialise xAOD access !");
  }
  ANA_MSG_INFO("Using xAOD access");
#else
  SmartIF<IAppMgrUI> app = POOL::Init();
  ANA_MSG_INFO("Using POOL access");
#endif

  // Get and parse the command-line arguments
  po::options_description desc("Running a simple truth analysis");
  std::string outputName,inputName;
  desc.add_options()
    ("help,h", "print usage and exit")
    ("output,o",    po::value<std::string>(&outputName), "Output file name")
    ("input,i",     po::value<std::string>(&inputName), "Input file name")
    ("nevents", po::value<int>()->default_value(-1), "number of events to run on (set to -1 to ignore it")
    ;

  po::variables_map vm;
  po::store( po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  long long nevents = -1;
  if (vm.count("nevents")) {
      nevents = vm["nevents"].as<int>();
  }

  Info( "Reading from %s and writing to %s" , inputName.c_str() , outputName.c_str() );

  // Input chain
  std::unique_ptr< TFile > ifile( TFile::Open( inputName.c_str() , "READ" ) );
  ANA_CHECK( ifile.get() );
#ifdef XAOD_STANDALONE
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
#else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
#endif
  ANA_CHECK( event.readFrom( ifile.get() ) );

  // Load metadata
  event.getEntries();

  // Make some temporary variables that we'll get out during the event loop
  const size_t nParticleContainers = 9;
  std::string particleKeyList[nParticleContainers] = { "TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson" };
  const xAOD::TruthEventContainer* xTruthEventContainer(nullptr);
  const xAOD::EventInfo* xEventInfo(nullptr);
  const xAOD::JetContainer* smallRJets(nullptr);
  const xAOD::JetContainer* largeRJets(nullptr);
  const xAOD::TruthMetaDataContainer* truthMeta(nullptr);
  const xAOD::MissingETContainer* truthMET(nullptr);
  const xAOD::TruthParticleContainer * truthParticles[nParticleContainers];
  for (size_t n=0;n<nParticleContainers;++n) truthParticles[n] = nullptr;

/*
    Missing full collections with aux: TruthBosonsWithDecayParticles, TruthBosonsWithDecayVertices
*/
  // Make histograms
  // MET histograms
  TH1D* h_metNonInt = new TH1D("MET_NonInt","",50,0,500.);
  TH1D* h_metInt = new TH1D("MET_Int","",50,0,200.);
  TH1D* h_metIntOut = new TH1D("MET_IntOut","",50,0,100.);
  TH1D* h_metIntMuons = new TH1D("MET_IntMuons","",50,0,300.);
  // Particle collections: pT and connections for all
  TH1D* h_partPt[nParticleContainers];
  for (size_t n=0;n<nParticleContainers;++n) h_partPt[n] = new TH1D((particleKeyList[n]+"_pT").c_str(),"",50,0.,500.);
  TH1D* h_partConn[nParticleContainers];
  for (size_t n=0;n<nParticleContainers;++n) h_partConn[n] = new TH1D((particleKeyList[n]+"_Connections").c_str(),"",35,-10,25);
  // Isolation for e/y/u/tau
  TH1D* h_elPtCone = new TH1D("ElPtCone","",50,0.,20.);
  TH1D* h_elEtCone = new TH1D("ElEtCone","",50,0.,20.);
  TH1D* h_muPtCone = new TH1D("MuPtCone","",50,0.,20.);
  TH1D* h_muEtCone = new TH1D("MuEtCone","",50,0.,20.);
  TH1D* h_phPtCone = new TH1D("PhPtCone","",50,0.,20.);
  TH1D* h_phEtCone = new TH1D("PhEtCone","",50,0.,20.);
  // MCTC decorations for e/y/u/tau/nu
  TH1D* h_elType = new TH1D("ElType","",50,0,50);
  TH1D* h_elOrig = new TH1D("ElOrigin","",50,0,50);
  TH1D* h_muType = new TH1D("MuType","",50,0,50);
  TH1D* h_muOrig = new TH1D("MuOrigin","",50,0,50);
  TH1D* h_phType = new TH1D("PhType","",50,0,50);
  TH1D* h_phOrig = new TH1D("PhOrigin","",50,0,50);
  TH1D* h_taType = new TH1D("TaType","",50,0,50);
  TH1D* h_taOrig = new TH1D("TaOrigin","",50,0,50);
  TH1D* h_nuType = new TH1D("NuType","",50,0,50);
  TH1D* h_nuOrig = new TH1D("NuOrigin","",50,0,50);
  // Dressed momenta for e/mu/tau
  TH1D* h_elDressedPt = new TH1D("ElDressedPt","",50,0.,200.);
  TH1D* h_muDressedPt = new TH1D("MuDressedPt","",50,0.,200.);
  TH1D* h_taDressedPt = new TH1D("TaDressedPt","",50,0.,200.);
  // Truth jets: pT, three flavor classifiers
  TH1D* h_jetPt = new TH1D("JetPt","",50,0,500.);
  TH1D* h_jetFTAG = new TH1D("JetFTAG","",25,0,25);
  TH1D* h_jetJeMe = new TH1D("JetJeMe","",25,0,25);
  TH1D* h_jetFull = new TH1D("JetFull","",50,0,50);
  // Large R : pT, tau2, D2
  TH1D* h_jetLRPt = new TH1D("JetLRPt","",50,0,500.);
  TH1D* h_jetLRT2 = new TH1D("JetLRT2","",50,0,500.);
  TH1D* h_jetLRD2 = new TH1D("JetLRD2","",50,0,500.);
  // Truth events: PDF info , cross section , filter outcome
  TH1D* h_x1 = new TH1D("X1","",50,0,1.);
  TH1D* h_x2 = new TH1D("X2","",50,0,1.);
  TH1D* h_xsec = new TH1D("XSec","",50,0,1.);
  TH1D* h_HTFilt = new TH1D("HTFilt","",50,0,1000.);
  // Lazy for truth weights
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > weightTool("PMGTools::PMGTruthWeightTool/PMGTruthWeightTool");
  ANA_CHECK(weightTool.retrieve());

  // For the weights in the file
  std::vector<TH1D*> h_weights;
  // For testing the truth metadata
  uint32_t channelNumber = 0;
  std::vector<std::string> weightNames;

  // Into the event loop, with an optional cap
  long long nEvents = nevents > 0 ? std::min(static_cast<long long>(event.getEntries()), nevents) : event.getEntries();
  Info( APP_NAME , "Beginning event loop over %llu events." , nEvents );
  for (long evt=0;evt<nEvents;++evt){
    // Grab the data
    event.getEntry(evt);
    if (evt%1000==0) Info( APP_NAME , "Working on entry %lu" , evt );

    // Get the containers out
    CHECK_RETRIEVE( xTruthEventContainer , "TruthEvents" )
    CHECK_RETRIEVE( xEventInfo , "EventInfo" )
    CHECK_RETRIEVE( smallRJets , "AntiKt4TruthDressedWZJets" )
    CHECK_RETRIEVE( largeRJets , "AntiKt10TruthTrimmedPtFrac5SmallR20Jets" )
    CHECK_RETRIEVE( truthMET , "MET_Truth" )
    for (size_t n=0;n<nParticleContainers;++n){
      CHECK_RETRIEVE( truthParticles[n] , particleKeyList[n].c_str() )
    }

    if (!event.retrieveMetaInput( truthMeta , "TruthMetaData" ).isSuccess()){
      Error( APP_NAME , "Could not load event TruthMetaData from the file!" );
      throw std::runtime_error("Could not load event TruthMetaData from the file!");
    }

    // Metadata check
    if (truthMeta->size()>1){
      Error( APP_NAME , "Truth metadata size: %lu . No one will look past item 0!" , truthMeta->size() );
      throw std::runtime_error("Truth metadata size >1");
    }
    if (channelNumber==0){
      channelNumber = (*truthMeta)[0]->mcChannelNumber();
      weightNames = std::vector<std::string>((*truthMeta)[0]->weightNames());
      Info( APP_NAME , "Got channel number %u and %lu weight names" , channelNumber , weightNames.size() );
    }
    if (channelNumber != (*truthMeta)[0]->mcChannelNumber()){
      Error( APP_NAME , "Channel number changed mid-file! Was: %u now: %u " , channelNumber , (*truthMeta)[0]->mcChannelNumber() );
      throw std::runtime_error("Channel number changed mid-file!");
    }
    if (weightNames != (*truthMeta)[0]->weightNames() ){
      Error( APP_NAME , "Weights have changed!" );
      for (size_t n=0;n<std::max( weightNames.size() , (*truthMeta)[0]->weightNames().size() );++n){
        std::cerr << "   " << n << " ";
        if (n<weightNames.size()) std::cerr << weightNames[n] << " ";
        else                      std::cerr << "- ";
        if (n<(*truthMeta)[0]->weightNames().size()) std::cerr << (*truthMeta)[0]->weightNames()[n] << " ";
        else                                         std::cerr << "- ";
        std::cerr << std::endl;
      }
      throw std::runtime_error("Weights have changed!");
    }
    // Event weight handling
    if (h_weights.size()==0){
      // First event, init!
      for (auto & name : weightNames ){
        h_weights.push_back( new TH1D(("h_W_"+name).c_str(),"",100,-10.,10.) );
      }
      h_weights.push_back( new TH1D("h_W_nominalTest","",100,-10.,10.) );
    }
    for (size_t n=0;n<weightNames.size();++n) h_weights[n]->Fill( weightTool->getWeight(weightNames[n]) );
    // Eventually this should be the nominal weight without needing to give an explicit name
    h_weights[weightNames.size()]->Fill( weightTool->getWeight(" nominal ") );
    // Event info
    float x1=0.,x2=0.;
    (*xTruthEventContainer)[0]->pdfInfoParameter( x1 , xAOD::TruthEvent::X1 );
    (*xTruthEventContainer)[0]->pdfInfoParameter( x2 , xAOD::TruthEvent::X2 );
    h_x1->Fill( x1 );
    h_x2->Fill( x2 );
    h_xsec->Fill( (*xTruthEventContainer)[0]->crossSection() );
    h_HTFilt->Fill( xEventInfo->auxdata<float>("GenFiltHT") );
    // For MET: NonInt, Int, IntOut, IntMuons
    h_metNonInt->Fill( (*truthMET)["NonInt"]->met()*0.001 );
    h_metNonInt->Fill( (*truthMET)["Int"]->met()*0.001 );
    h_metNonInt->Fill( (*truthMET)["IntOut"]->met()*0.001 );
    h_metNonInt->Fill( (*truthMET)["IntMuons"]->met()*0.001 );
    // Truth particles
    for (size_t n=0;n<nParticleContainers;++n){ // PT and connections for all
      for (const auto * p : *truthParticles[n]){
        h_partPt[n]->Fill( p->pt() );
        h_partConn[n]->Fill( countChildren( p ) );
        h_partConn[n]->Fill( -1-countParents( p ) );
      }
    }
    for (const auto * p : *truthParticles[0]){ // Electrons
      h_elPtCone->Fill( p->auxdata<float>("ptcone30")*0.001 );
      h_elEtCone->Fill( p->auxdata<float>("etcone20")*0.001 );
      h_elDressedPt->Fill( p->auxdata<float>("pt_dressed")*0.001 );
      h_elType->Fill( p->auxdata<unsigned int>("classifierParticleType") );
      h_elOrig->Fill( p->auxdata<unsigned int>("classifierParticleOrigin") );
    }
    for (const auto * p : *truthParticles[1]){ // Muons
      h_muPtCone->Fill( p->auxdata<float>("ptcone30")*0.001 );
      h_muEtCone->Fill( p->auxdata<float>("etcone20")*0.001 );
      h_muDressedPt->Fill( p->auxdata<float>("pt_dressed")*0.001 );
      h_muType->Fill( p->auxdata<unsigned int>("classifierParticleType") );
      h_muOrig->Fill( p->auxdata<unsigned int>("classifierParticleOrigin") );
    }
    for (const auto * p : *truthParticles[2]){ // Photons
      h_phPtCone->Fill( p->auxdata<float>("ptcone20")*0.001 );
      h_phEtCone->Fill( p->auxdata<float>("etcone20")*0.001 );
      h_phType->Fill( p->auxdata<unsigned int>("classifierParticleType") );
      h_phOrig->Fill( p->auxdata<unsigned int>("classifierParticleOrigin") );
    }
    for (const auto * p : *truthParticles[3]){ // Taus
      h_taDressedPt->Fill( p->auxdata<float>("pt_vis_dressed")*0.001 );
      h_taType->Fill( p->auxdata<unsigned int>("classifierParticleType") );
      h_taOrig->Fill( p->auxdata<unsigned int>("classifierParticleOrigin") );
    }
    for (const auto * p : *truthParticles[4]){ // Neutrinos
      h_nuType->Fill( p->auxdata<unsigned int>("classifierParticleType") );
      h_nuOrig->Fill( p->auxdata<unsigned int>("classifierParticleOrigin") );
    }
    for (const auto * j : *smallRJets){ // Small-R jets
      h_jetPt->Fill( j->pt()*0.001 );
      h_jetFTAG->Fill( j->auxdata<int>("HadronConeExclTruthLabelID") );
      h_jetJeMe->Fill( j->auxdata<int>("PartonTruthLabelID") );
      h_jetFull->Fill( j->auxdata<int>("TrueFlavor") );
    }
    for (const auto * j : *largeRJets){ // Large-R jets
      h_jetLRPt->Fill( j->pt()*0.001 );
      h_jetLRT2->Fill( j->auxdata<float>("Tau2_wta") );
      h_jetLRD2->Fill( j->auxdata<float>("D2") );
    }

  } // End of event loop
  Info( APP_NAME , "Done with event loop" );

  // Output file  
  TFile * oRoot = new TFile(outputName.c_str(),"RECREATE");
  oRoot->cd();

  // Write histograms
  // MET histograms
  h_metNonInt->Write();
  h_metInt->Write();
  h_metIntOut->Write();
  h_metIntMuons->Write();
  // Truth particle histograms
  for (size_t n=0;n<nParticleContainers;++n) h_partPt[n]->Write();
  for (size_t n=0;n<nParticleContainers;++n) h_partConn[n]->Write();
  h_elPtCone->Write();
  h_elEtCone->Write();
  h_muPtCone->Write();
  h_muEtCone->Write();
  h_phPtCone->Write();
  h_phEtCone->Write();
  h_elDressedPt->Write();
  h_muDressedPt->Write();
  h_taDressedPt->Write();
  h_elType->Write();
  h_elOrig->Write();
  h_muType->Write();
  h_muOrig->Write();
  h_phType->Write();
  h_phOrig->Write();
  h_taType->Write();
  h_taOrig->Write();
  h_nuType->Write();
  h_nuOrig->Write();
  // Truth jet histograms
  h_jetPt->Write();
  h_jetFTAG->Write();
  h_jetJeMe->Write();
  h_jetFull->Write();
  h_jetLRPt->Write();
  h_jetLRT2->Write();
  h_jetLRD2->Write();
  // Event histograms
  h_x1->Write();
  h_x2->Write();
  h_xsec->Write();
  h_HTFilt->Write();
  for (auto * h : h_weights) h->Write();

  // Close up -- all done!
  oRoot->Close();

  // trigger finalization of all services and tools created by the Gaudi Application
#ifndef XAOD_STANDALONE
  if (app->finalize().isFailure()){
    Warning( APP_NAME , "Finalization failed?" );
  }
#endif

  Info( APP_NAME , "All done -- goodbye." );

  return 0;
}
