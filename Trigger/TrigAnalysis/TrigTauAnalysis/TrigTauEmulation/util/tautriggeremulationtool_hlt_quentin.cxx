// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
// System include(s):
#include <memory>
#include <set>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TChain.h>
#include <TError.h>
#include <TTree.h>
#include <TSystem.h>
#include <TH1F.h>

// Core EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTracking/TrackParticle.h"

// Local include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"
#endif

#include "AsgTools/ToolHandle.h"

#include "TrigTauEmulation/ToolsRegistry.h"
#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"
#include "TrigTauEmulation/HltChain.h"

// Trigger Decision Tool
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

using namespace TrigConf;
using namespace Trig;

std::vector<std::string> all_l1_chains() ;


// Error checking macro
#define CHECK( ARG )\
  do {                                                                  \
  const bool result = ARG;\
  if(!result) {\
  ::Error(APP_NAME, "Failed to execute: \"%s\"",\
	  #ARG );\
  return 1;\
  }\
  } while( false )

int main(int argc, char** argv) {


  TH1F h("h_ntracks", "h_ntracks", 10, 0, 10);
  // Get the name of the application:
  const char* APP_NAME = "tautriggeremulationtool_hlt_quentin";

  if (argc != 3) {
    ::Error(APP_NAME, XAOD_MESSAGE("Wrong number of arguments"));
    return 1;
  }
  std::string reference_chain = argv[1];
  std::string chain_to_test = argv[2];
  std::cout << "Evaluate " << chain_to_test
	    << " based on " << reference_chain 
	    << std::endl;

  // Initialise the environment:
  RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );


  // static const char* FNAME = "/afs/cern.ch/work/q/qbuat/public/mc15_13TeV/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630_tid05367718_00/AOD.05367718._006348.pool.root.1";
  //  std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );

  //  if( ! ifile.get() ) {
  //    ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
  // 	      FNAME );
  //    return 1;
  //  }
  //  // change

   // Create the TEvent object
   xAOD::TEvent event(xAOD::TEvent::kClassAccess);
   // CHECK( event.readFrom(ifile.get()) );

   xAOD::TStore store;


   ::TChain chain1("CollectionTree");
   // chain1.Add("/afs/cern.ch/work/q/qbuat/public/mc15_13TeV/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630/*root.1");
   // chain1.Add("/tmp/qbuat/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630_tid05367726_00/*root.1");
   chain1.Add("/tmp/qbuat/user.dzanzi.data15_13TeV.00267638.physics_EnhancedBias.merge.RAW_EXT0.31882626/user.dzanzi.5729275.EXT0._000004.AOD.root");
   // chain1.Add("/tmp/qbuat/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264_tid05382618_00/*root*");

   RETURN_CHECK(APP_NAME, event.readFrom(&chain1));
  //Set up TDT for testing
  //add config tool
  // xAODConfigTool configTool("TrigConf::xAODConfigTool");
  auto* trigConfigTool = new TrigConf::xAODConfigTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(trigConfigTool);
  configHandle->initialize();

  // The decision tool
  // The decision tool
  Trig::TrigDecisionTool *trigDecTool = new Trig::TrigDecisionTool("TrigDecTool");
  trigDecTool->setProperty("ConfigTool", configHandle);
  //  trigDecTool.setProperty("OutputLevel", MSG::VERBOSE);
  trigDecTool->setProperty("TrigDecisionKey", "xTrigDecision");
  trigDecTool->initialize();


  // chains to test
  std::vector<std::string> chains_to_test;
  chains_to_test.push_back(chain_to_test);
  // chains_to_test.push_back("HLT_tau25_loose1_ptonly");
  // chains_to_test.push_back("HLT_tau25_medium1_tracktwo");
  // chains_to_test.push_back("HLT_tau25_tight1_tracktwo");
  // chains_to_test.push_back("HLT_tau35_loose1_tracktwo");
  // chains_to_test.push_back("HLT_tau35_medium1_tracktwo");
  // chains_to_test.push_back("HLT_tau35_tight1_tracktwo");
  // chains_to_test.push_back("HLT_tau80_medium1_tracktwo");
  // chains_to_test.push_back("HLT_tau125_medium1_tracktwo");
  // chains_to_test.push_back("HLT_tau160_medium1_tracktwo");

  std::map<std::string, int> fire_tdt;
  std::map<std::string, int> fire_emul;
  for (auto c: chains_to_test) {
    fire_tdt[c] = 0;
    fire_emul[c] = 0;
  }

  ToolsRegistry registry("ToolsRegistry");
  CHECK(registry.initialize());

  // Initialize the tool
  auto l1_chains = all_l1_chains();
  // Initialize the tool
  TrigTauEmul::Level1EmulationTool l1_emulationTool("Level1TauTriggerEmulator");
  CHECK(l1_emulationTool.setProperty("l1_chains", l1_chains));
  CHECK(l1_emulationTool.setProperty("JetTools", registry.GetL1JetTools()));
  CHECK(l1_emulationTool.setProperty("EmTauTools", registry.GetL1TauTools()));
  CHECK(l1_emulationTool.setProperty("XeTools", registry.GetL1XeTools()));
  CHECK(l1_emulationTool.setProperty("MuonTools", registry.GetL1MuonTools()));
  ToolHandle<TrigTauEmul::ILevel1EmulationTool> handle(&l1_emulationTool);

  TrigTauEmul::HltEmulationTool emulationTool("TauTriggerEmulator");
  CHECK(emulationTool.setProperty("hlt_chains", chains_to_test));
  CHECK(emulationTool.setProperty("Level1EmulationTool", handle));
  CHECK(emulationTool.setProperty("HltTauTools", registry.GetHltTauTools()));
  CHECK(emulationTool.setProperty("TrigDecTool", "TrigDecTool"));
  CHECK(emulationTool.initialize());

  int differences_tdt_emul = 0;
   Long64_t entries = event.getEntries();
   for (Long64_t entry = 0; entry < entries; entry++) {

     if ((entry%200)==0)
       ::Info(APP_NAME, "Start processing event %d", (int)entry);

     event.getEntry(entry);

     // retrieve the EDM objects
     const xAOD::EventInfo * ei = 0;
     CHECK(event.retrieve(ei, "EventInfo"));

     const xAOD::EmTauRoIContainer *l1taus = 0;
     CHECK(event.retrieve(l1taus, "LVL1EmTauRoIs"));

     // for (auto it:  *l1taus) {
     //   std::cout << "pT: \t" << it->tauClus() << "\n" 
     // 		 << "eta: \t" << it->eta() << "\n"
     // 		 << "phi: \t " << it->phi() << std::endl;
     // } 

     const xAOD::JetRoIContainer *l1jets = 0;
     CHECK(event.retrieve(l1jets, "LVL1JetRoIs"));

     const xAOD::MuonRoIContainer* l1muons = 0;
     CHECK(event.retrieve(l1muons, "LVL1MuonRoIs"));

     const xAOD::EnergySumRoI* l1xe = 0;
     CHECK(event.retrieve(l1xe, "LVL1EnergySumRoI"));

    const DataVector<xAOD::TrackParticle>* fast_tracks;
    CHECK(event.retrieve(fast_tracks, "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF"));

     // const xAOD::TauJetContainer* hlt_taus;
     // CHECK(event.retrieve(hlt_taus, "HLT_xAOD__TauJetContainer_TrigTauRecMerged"));

     if (not trigDecTool->isPassed(reference_chain)) 
       continue;

     // std::cout << "features !" << std::endl;
     auto cg = trigDecTool->getChainGroup(reference_chain);
     auto features = cg->features();

     xAOD::TauJetContainer* presel_taus = new xAOD::TauJetContainer();
     xAOD::TauJetAuxContainer* presel_taus_aux = new xAOD::TauJetAuxContainer();
     presel_taus->setStore(presel_taus_aux);
     auto tauPreselFeatures = features.containerFeature<xAOD::TauJetContainer>("TrigTauRecPreselection");
     for (auto &tauContainer: tauPreselFeatures) {
       if (tauContainer.cptr()) {
	 for (auto tau: *tauContainer.cptr()) {
	   xAOD::TauJet * new_tau = new xAOD::TauJet();
	   new_tau->makePrivateStore(tau);
	   presel_taus->push_back(new_tau);
	 }
       }
     }

     xAOD::TauJetContainer* hlt_taus = new xAOD::TauJetContainer();
     xAOD::TauJetAuxContainer* hlt_taus_aux = new xAOD::TauJetAuxContainer();
     hlt_taus->setStore(hlt_taus_aux);
     // std::cout << "Create the HLT tau container" << std::endl;
     auto tauHltFeatures = features.containerFeature<xAOD::TauJetContainer>("TrigTauRecMerged");
     for (auto &tauContainer: tauHltFeatures) {
       if (tauContainer.cptr()) {
	 for (auto tau: *tauContainer.cptr()) {
	   xAOD::TauJet * new_tau = new xAOD::TauJet();
	   new_tau->makePrivateStore(tau);
	   hlt_taus->push_back(new_tau);
	   // std::cout << "tau " << tau->index() << ": Pt/Eta/Phi/Discri/IsLoose = " 
	   // 	     << tau->pt() << " / "
	   // 	     << tau->eta() << " / "
	   // 	     << tau->phi() << " / "
	   // 	     << tau->discriminant(xAOD::TauJetParameters::BDTJetScore) << " / "
	   // 	     << tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) 
	   // 	     << std::endl;
	 }
       }
     }


     // CHECK(emulationTool.execute(l1taus, l1jets, l1muons, l1xe, hlt_taus, fast_tracks));
     CHECK(emulationTool.execute(l1taus, l1jets, l1muons, l1xe, hlt_taus, presel_taus));


     // Print the decision for all the tested chains and the TDT decision
      for (auto it: chains_to_test) {
       bool emulation_decision = emulationTool.decision(it);
       // std::cout << it << " emulation : " << emulation_decision << std::endl;
       auto chain_group = trigDecTool->getChainGroup(it);
       bool cg_passes_event = chain_group->isPassed();  
       fire_emul[it] += (int)emulation_decision;
       fire_tdt[it] += (int)cg_passes_event;

       // if (emulation_decision)
       // 	 ::Info(APP_NAME, "PASS: event %d with number %d", (int)entry, (int)ei->eventNumber());
       // std::cout << it << " TDT : " <<  cg_passes_event << std::endl;
       if (emulation_decision != cg_passes_event){
	 differences_tdt_emul += 1;
	 ::Info(APP_NAME, "event %d processed -- event number %d", (int)entry, (int)ei->eventNumber());
	 std::cout << Form("event %d processed -- event number %d", (int)entry, (int)ei->eventNumber()) << std::endl;
	 std::cout << "\n" << std::endl;
	 ::Warning(APP_NAME, "TDT AND EMULATION DECISION DIFFERENT. TDT: %d -- EMULATION: %d", (int)cg_passes_event, (int)emulation_decision);
	 emulationTool.msg().setLevel(MSG::DEBUG);
	 emulationTool.decision(it);
	 emulationTool.msg().setLevel(MSG::INFO);
	 
	 std::cout << "\t\t Scanning construction of chain " << it << std::endl;
	 auto chain = HltParsing::chains()[it];
	 
	 std::cout << "\t\t L1 TAUS" << std::endl;
	 std::ostringstream header;
	 header << "\t\t Index |\t\t Pt |\t\t Eta |\t\t Phi |\t\t iso |\t\t type |";
	 for (auto &item: chain.items()) {
	   if (item.isTau())
	     header << "\t\t" << item.seed() << "|" ;
	 }
	 std::cout << header.str() << std::endl;
	 for(auto l1tau: *l1taus) {
	   std::ostringstream l1_line;
	   l1_line << "\t\t";
	   l1_line << (l1tau->index()) ;
	   l1_line << "|\t\t";
	   l1_line << (l1tau->tauClus()) ;
	   l1_line << "|\t\t";
	   l1_line << (l1tau->eta()); 
	   l1_line << "|\t\t";
	   l1_line << (l1tau->phi()); 
	   l1_line << "|\t\t";
	   l1_line << (l1tau->emIsol()); 
	   l1_line << "|\t\t";
	   l1_line << (l1tau->roiType()); 
	   l1_line << "|\t\t";
	   for (auto &item: chain.items()) {
	     if (item.isTau()) {
	       l1_line << (l1tau->auxdataConst<bool>(item.seed()));
	       l1_line << "|\t\t";
	     }
	   }
	   std::cout << l1_line.str() << std::endl;
	 }
	 std::cout << "\n" << std::endl;


	 std::cout << "\t\t PRESEL TAUS" << std::endl;
	 std::string presel_header = "\t\t Index |\t\t Pt |\t\t Eta |\t\t Phi |\t\t Ntracks |\t\t Nwidetracks |";
	 std::cout << presel_header << std::endl;
	 for(auto tau: *presel_taus) {
	   std::ostringstream presel_line;
	   presel_line << "\t\t";
	   presel_line << (tau->index()) ;
	   presel_line << "|\t\t";
	   presel_line << (tau->pt()) ;
	   presel_line << "|\t\t";
	   presel_line << (tau->eta()); 
	   presel_line << "|\t\t";
	   presel_line << (tau->phi()); 
	   presel_line << "|\t\t";
	   presel_line << (tau->nTracks()); 
	   presel_line << "|\t\t";
	   presel_line << (tau->nWideTracks()); 
	   presel_line << "|\t\t";
	   std::cout << presel_line.str() << std::endl;
	 }
	 std::cout << "\n" << std::endl;

	 std::cout << "\t\t HLT TAUS" << std::endl;
	 std::string hlt_header = "\t\t Index |\t\t Pt |\t\t Eta |\t\t Phi |\t\t Ntracks |\t\t Nwidetracks |\t\t l1_index |";
	 for (auto &item: chain.items()) {
	   if (item.isTau())
	     hlt_header += "\t\t" + item.name() + "|" ;
	 }
	 std::cout << hlt_header << std::endl;
	 for(auto tau: *hlt_taus) {
	   std::ostringstream hlt_line;
	   hlt_line << "\t\t";
	   hlt_line << (tau->index()) ;
	   hlt_line << "|\t\t";
	   hlt_line << (tau->pt()) ;
	   hlt_line << "|\t\t";
	   hlt_line << (tau->eta()); 
	   hlt_line << "|\t\t";
	   hlt_line << (tau->phi()); 
	   hlt_line << "|\t\t";
	   hlt_line << (tau->nTracks()); 
	   hlt_line << "|\t\t";
	   hlt_line << (tau->nWideTracks()); 
	   hlt_line << "|\t\t";
	   hlt_line << (tau->auxdataConst<int>("l1_index")); 
	   hlt_line << "|\t\t";
	   for (auto &item: chain.items()) {
	     if (item.isTau()) {
	       hlt_line << (tau->auxdataConst<bool>(item.name()));
	       hlt_line << "|\t\t";
	       if (tau->auxdataConst<bool>(item.name())) 
	 	 h.Fill(tau->nTracks());
	     }
	   }
	   std::cout << hlt_line.str() << std::endl;
	 }
	 std::cout << "\n" << std::endl;


       }
     }

      store.clear();

   }
   TFile fout("toto.root", "RECREATE");
   h.Write();
   fout.Close();

   // CHECK(emulationTool.finalize());
   std::cout << "------------------------------" << std::endl;
   for (auto ch: chains_to_test) 
     std::cout << "|" << ch << "| \t" << fire_tdt[ch] << "| \t" << fire_emul[ch] << "| \t" << fire_tdt[ch] - fire_emul[ch] << "\t |" << std::endl;
   std::cout << "Number of differences = " << differences_tdt_emul << std::endl;

   return 0;
}

std::vector<std::string> all_l1_chains() 

{
  std::vector<std::string> chains_to_test;

  chains_to_test.push_back("L1_TAU12");
  chains_to_test.push_back("L1_TAU12IL");
  chains_to_test.push_back("L1_TAU12IM");
  chains_to_test.push_back("L1_TAU12IT");
  chains_to_test.push_back("L1_TAU20");
  chains_to_test.push_back("L1_TAU20IL");
  chains_to_test.push_back("L1_TAU20IM");
  chains_to_test.push_back("L1_TAU20IT");
  chains_to_test.push_back("L1_TAU30");
  chains_to_test.push_back("L1_TAU40");
  chains_to_test.push_back("L1_TAU60");
  chains_to_test.push_back("L1_TAU8");
  chains_to_test.push_back("L1_J12");
  chains_to_test.push_back("L1_MU10");
  chains_to_test.push_back("L1_TAU20IM_2TAU12IM");
  chains_to_test.push_back("L1_TAU20_2TAU12");
  chains_to_test.push_back("L1_EM15HI_2TAU12");
  chains_to_test.push_back("L1_EM15HI_2TAU12IM");
  chains_to_test.push_back("L1_EM15HI_2TAU12IM_J25_3J12");
  chains_to_test.push_back("L1_EM15HI_2TAU12_J25_3J12");
  chains_to_test.push_back("L1_EM15HI_TAU40_2TAU15");
  chains_to_test.push_back("L1_J25_3J12_EM15-TAU12I");
  chains_to_test.push_back("L1_MU10_TAU12");
  chains_to_test.push_back("L1_MU10_TAU12IM");
  chains_to_test.push_back("L1_MU10_TAU12IM_J25_2J12");
  chains_to_test.push_back("L1_MU10_TAU12IL_J25_2J12");
  chains_to_test.push_back("L1_MU10_TAU12_J25_2J12");
  chains_to_test.push_back("L1_J25_2J12_DR-MU10TAU12I");
  chains_to_test.push_back("L1_MU10_TAU20");
  chains_to_test.push_back("L1_MU10_TAU20IM");
  chains_to_test.push_back("L1_TAU25IT_2TAU12IT_2J25_3J12");
  chains_to_test.push_back("L1_TAU20IL_2TAU12IL_J25_2J20_3J12");
  chains_to_test.push_back("L1_TAU20IM_2TAU12IM_J25_2J20_3J12");
  chains_to_test.push_back("L1_TAU20_2TAU12_J25_2J20_3J12");
  chains_to_test.push_back("L1_J25_2J20_3J12_BOX-TAU20ITAU12I");
  chains_to_test.push_back("L1_J25_2J20_3J12_DR-TAU20ITAU12I");
  chains_to_test.push_back("L1_DR-MU10TAU12I_TAU12I-J25");
  chains_to_test.push_back("L1_MU10_TAU12I-J25");
  chains_to_test.push_back("L1_TAU20IM_2J20_XE45");
  chains_to_test.push_back("L1_TAU20_2J20_XE45");
  chains_to_test.push_back("L1_TAU25_2J20_XE45");
  chains_to_test.push_back("L1_TAU20IM_2J20_XE50");
  chains_to_test.push_back("L1_XE45_TAU20-J20");
  chains_to_test.push_back("L1_EM15HI_2TAU12IM_XE35");
  chains_to_test.push_back("L1_EM15HI_2TAU12IL_XE35");
  chains_to_test.push_back("L1_EM15HI_2TAU12_XE35");
  chains_to_test.push_back("L1_XE35_EM15-TAU12I");
  chains_to_test.push_back("L1_XE40_EM15-TAU12I");
  chains_to_test.push_back("L1_MU10_TAU12_XE35");
  chains_to_test.push_back("L1_MU10_TAU12IM_XE35");
  chains_to_test.push_back("L1_MU10_TAU12IL_XE35");
  chains_to_test.push_back("L1_MU10_TAU12IT_XE35");
  chains_to_test.push_back("L1_MU10_TAU12IM_XE40");
  chains_to_test.push_back("L1_TAU20IM_2TAU12IM_XE35");
  chains_to_test.push_back("L1_TAU20IL_2TAU12IL_XE35");
  chains_to_test.push_back("L1_TAU20IT_2TAU12IT_XE35");
  chains_to_test.push_back("L1_TAU20_2TAU12_XE35");
  chains_to_test.push_back("L1_TAU20IM_2TAU12IM_XE40");
  chains_to_test.push_back("L1_DR-MU10TAU12I");
  chains_to_test.push_back("L1_TAU12I-J25");
  chains_to_test.push_back("L1_EM15-TAU40");
  chains_to_test.push_back("L1_TAU20-J20");
  chains_to_test.push_back("L1_EM15-TAU12I");
  chains_to_test.push_back("L1_EM15TAU12I-J25");
  chains_to_test.push_back("L1_DR-EM15TAU12I-J25");
  chains_to_test.push_back("L1_TAU20ITAU12I-J25");
  chains_to_test.push_back("L1_DR-TAU20ITAU12I");
  chains_to_test.push_back("L1_BOX-TAU20ITAU12I");
  chains_to_test.push_back("L1_DR-TAU20ITAU12I-J25");
  chains_to_test.push_back("L1_EM15");
  chains_to_test.push_back("L1_EM15HI");
  return chains_to_test;
}


