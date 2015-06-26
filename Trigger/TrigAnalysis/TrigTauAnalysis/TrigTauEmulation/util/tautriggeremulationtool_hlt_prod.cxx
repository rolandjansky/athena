// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
// vim: ts=2 sw=2
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

std::vector<std::string> splitNames(const std::string& files, std::string sep = ",") {
  std::vector<std::string> fileList;
  for (size_t i=0,n; i <= files.length(); i=n+1) {
    n = files.find_first_of(sep,i);
    if (n == std::string::npos) {
      n = files.length();
    }
    std::string tmp = files.substr(i,n-i);
    std::string ttmp;
    for(unsigned int j=0; j<tmp.size(); j++) {
      if(tmp[j]==' ' || tmp[j]=='\n') { continue; }
      ttmp += tmp[j];
    }
    fileList.push_back(ttmp);
  }
  return fileList;

}


int main(int argc, char** argv) {


  // Get the name of the application:
  const char* APP_NAME = "tautriggeremulationtool_hlt_prod";

  static const char* FNAME = 
    "/afs/cern.ch/user/q/qbuat/work/public/mc15_13TeV/"
    "mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD."
    "e3601_s2576_s2132_r6630/AOD.05358802._002522.pool.root.1";

  std::vector<std::string> filenames;
  if(argc < 2){
    filenames.push_back(std::string(FNAME));
  } else {
    filenames = splitNames(argv[1]);
  }

  std::string reference_chain = "HLT_tau25_idperf_tracktwo";

  std::vector<std::string> chains_to_test;
  chains_to_test.push_back("HLT_tau25_idperf_tracktwo");
  chains_to_test.push_back("HLT_tau25_perf_tracktwo");
  chains_to_test.push_back("HLT_tau25_loose1_tracktwo");
  chains_to_test.push_back("HLT_tau25_medium1_tracktwo");
  chains_to_test.push_back("HLT_tau35_medium1_tracktwo_xe70_L1XE45");
  chains_to_test.push_back("HLT_e17_lhmedium_tau25_medium1_tracktwo");
  chains_to_test.push_back("HLT_mu14_tau25_medium1_tracktwo");
  chains_to_test.push_back("HLT_mu14_tau35_medium1_tracktwo");
  chains_to_test.push_back("HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo");

  if(argc >= 4){
    reference_chain = argv[2];
    chains_to_test = splitNames(argv[3]);
  }
  
  std::cout << "Will evaluate the following based on " << reference_chain << ":" << std::endl;
  for(auto s: chains_to_test){
    std::cout << "\t" << s << std::endl;
  }

  // Initialise the environment:
  RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

  // Create the TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  // CHECK( event.readFrom(ifile.get()) );

  xAOD::TStore store;

  ::TChain chain1("CollectionTree");
  chain1.Add("/tmp/qbuat/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630_tid05367726_00/*root.1");
  //chain1.Add("/afs/cern.ch/work/g/gbesjes/public/data15_13TeV.00266904.physics_EnhancedBias.recon.AOD.r6791/*root.1");
  //chain1.Add("/afs/cern.ch/work/g/gbesjes/public/data15_13TeV.00266904.physics_EnhancedBias.recon.AOD.r6791/AOD.05612847._000084.pool.root.1");
  // chain1.Add("/tmp/qbuat/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264_tid05382618_00/*root*");

  RETURN_CHECK(APP_NAME, event.readFrom(&chain1));

  // Trigger configuration tool
  TrigConf::xAODConfigTool trigConfigTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&trigConfigTool);
  configHandle->initialize();

  // Trigger decision tool
  Trig::TrigDecisionTool *trigDecTool = new Trig::TrigDecisionTool("TrigDecTool");
  trigDecTool->setProperty("ConfigTool", configHandle);
  trigDecTool->setProperty("TrigDecisionKey", "xTrigDecision");
  trigDecTool->initialize();

  // ToolsRegistry: registry of the selection tools to 
  // perform the emulation
  ToolsRegistry registry("ToolsRegistry");
  CHECK(registry.initialize());

  // Level1 chains
  std::vector<std::string> l1_chains = all_l1_chains();

  // Instantiate and Initialize the Level1 emulation tool
  TrigTauEmul::Level1EmulationTool l1_emulationTool("Level1TauTriggerEmulator");
  CHECK(l1_emulationTool.setProperty("l1_chains", l1_chains));
  CHECK(l1_emulationTool.setProperty("JetTools", registry.GetL1JetTools()));
  CHECK(l1_emulationTool.setProperty("EmTauTools", registry.GetL1TauTools()));
  CHECK(l1_emulationTool.setProperty("XeTools", registry.GetL1XeTools()));
  CHECK(l1_emulationTool.setProperty("MuonTools", registry.GetL1MuonTools()));
  // Create a handle on the Level1 tool
  ToolHandle<TrigTauEmul::ILevel1EmulationTool> handle(&l1_emulationTool);

  // Instantiate and Initialize the hlt emulator
  TrigTauEmul::HltEmulationTool emulationTool("TauTriggerEmulator");
  CHECK(emulationTool.setProperty("hlt_chains", chains_to_test));
  CHECK(emulationTool.setProperty("Level1EmulationTool", handle));
  CHECK(emulationTool.setProperty("HltTauTools", registry.GetHltTauTools()));
  CHECK(emulationTool.setProperty("TrigDecTool", "TrigDecTool"));
  CHECK(emulationTool.initialize());

  TH1D* _h_TDT_Emulation_differences     = new TH1D("_h_TDT_Emulation_differences", "TDT_Emulation_differences",chains_to_test.size(),0,chains_to_test.size());
  TH1D* _h_TDT_fires     = new TH1D("_h_TDT_fires", "TDT_fires_total_number",chains_to_test.size(),0,chains_to_test.size());
  int chain_number = 1;
  for (auto it: chains_to_test) {
    _h_TDT_Emulation_differences->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    _h_TDT_fires->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    chain_number++;
  }


  std::map<std::string, int> fire_tdt;
  std::map<std::string, int> fire_emul;
  for (auto c: chains_to_test) {
    fire_tdt[c] = 0;
    fire_emul[c] = 0;
  }


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

    const xAOD::JetRoIContainer *l1jets = 0;
    CHECK(event.retrieve(l1jets, "LVL1JetRoIs"));

    const xAOD::MuonRoIContainer* l1muons = 0;
    CHECK(event.retrieve(l1muons, "LVL1MuonRoIs"));

    const xAOD::EnergySumRoI* l1xe = 0;
    CHECK(event.retrieve(l1xe, "LVL1EnergySumRoI"));

    // const DataVector<xAOD::TrackParticle>* fast_tracks;
    // CHECK(event.retrieve(fast_tracks, "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF"));

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
    auto tauHltFeatures = features.containerFeature<xAOD::TauJetContainer>("TrigTauRecMerged");
    for (auto &tauContainer: tauHltFeatures) {
      if (tauContainer.cptr()) {
        for (auto tau: *tauContainer.cptr()) {
          xAOD::TauJet * new_tau = new xAOD::TauJet();
          new_tau->makePrivateStore(tau);
          hlt_taus->push_back(new_tau);
        }
      }
    }


    // CHECK(emulationTool.execute(l1taus, l1jets, l1muons, l1xe, hlt_taus, fast_tracks));
    CHECK(emulationTool.execute(l1taus, l1jets, l1muons, l1xe, hlt_taus, presel_taus));


    // Print the decision for all the tested chains and the TDT decision
    int chain_number = 0;
    bool TDT_emulation_decisions_unmatched = false;
    for (auto it: chains_to_test) {
      bool emulation_decision = emulationTool.decision(it);
      auto chain_group = trigDecTool->getChainGroup(it);
      bool cg_passes_event = chain_group->isPassed();  
      fire_emul[it] += (int)emulation_decision;
      fire_tdt[it] += (int)cg_passes_event;
      if(cg_passes_event) 
        _h_TDT_fires->Fill(it.c_str(), 1);

      if (emulation_decision != cg_passes_event){
        differences_tdt_emul += 1;
        _h_TDT_Emulation_differences->Fill(it.c_str(), 1);
        TDT_emulation_decisions_unmatched = true;
      }
      chain_number++;
    }
    store.clear();
  }
  TFile *fOut = new TFile("output.root", "RECREATE");
  fOut->cd();
  _h_TDT_fires->Write();
  _h_TDT_Emulation_differences->Write();
  fOut->Close();

  unsigned int max_len = 0;
  for (auto ch: chains_to_test) {
    if (ch.length() > max_len){
      max_len = ch.length();
    }
  }
  
  //std::cout << "------------------------------" << std::endl;
  std::cout << std::string( (max_len+1) + 3*12 + 13, '-') << std::endl;
  for (auto ch: chains_to_test) { 
    std::cout << "| " << std::setw(max_len+1)  << std::left << ch << " | " 
      << std::setw(12) << std::right << fire_tdt[ch] << " | " 
      << std::setw(12) << fire_emul[ch] << " | " 
      << std::setw(12) << fire_tdt[ch] - fire_emul[ch] << " |" << std::endl;
  }
  std::cout << std::string( (max_len+1) + 3*12 + 13, '-') << std::endl;
  std::cout << "Number of differences = " << differences_tdt_emul << std::endl;
  std::cout << std::string( (max_len+1) + 3*12 + 13, '-') << std::endl;

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
  chains_to_test.push_back("L1_XE45");
  chains_to_test.push_back("L1_MU10_TAU20IM_J25_2J12");
  return chains_to_test;
}


