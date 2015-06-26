// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// $Id$

// System include(s):
#include <memory>
#include <set>
#include <string>

// ROOT include(s):
#include <TChain.h>
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2D.h>

// Core EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTracking/TrackParticle.h"

#ifdef ROOTCORE
// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"
#endif

#include "TrigTauEmulation/ToolsRegistry.h"
#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"

// Trigger Decision Tool
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

using namespace TrigConf;
using namespace Trig;


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

int main(int argc, char **argv) {


  // Get the name of the application:
  const char* APP_NAME = "ut_tautriggeremulationtool_test";

  // Initialise the environment:
  RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

  //
  ToolsRegistry registry("ToolsRegistry"); 
  CHECK(registry.initialize());

  // Initialize the tool
  TrigTauEmul::Level1EmulationTool emulationTool("TauTriggerEmulator");
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




  CHECK(emulationTool.setProperty("l1_chains", chains_to_test));
  CHECK(emulationTool.setProperty("JetTools", registry.GetL1JetTools()));
  CHECK(emulationTool.setProperty("EmTauTools", registry.GetL1TauTools()));
  CHECK(emulationTool.setProperty("XeTools", registry.GetL1XeTools()));
  CHECK(emulationTool.setProperty("MuonTools", registry.GetL1MuonTools()));

  //emulationTool.msg().setLevel(MSG::DEBUG);
  CHECK(emulationTool.initialize());

  static const char* FNAME = "/afs/cern.ch/user/q/qbuat/work/public/mc15_13TeV/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630/AOD.05358802._002522.pool.root.1";
 
  std::vector<std::string> filenames;
  if(argc < 2){
    filenames.push_back(std::string(FNAME));
  } else {
    filenames = splitNames(argv[1]);
  }

  // Create the TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  
  ::TChain chain1("CollectionTree");
  for(auto fname : filenames){
    chain1.Add(fname.c_str());
  }

  RETURN_CHECK(APP_NAME, event.readFrom(&chain1));
  //CHECK( event.readFrom(ifile.get()) );


  //Set up TDT for testing
  //add config tool
  xAODConfigTool configTool("TrigConf::xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  configHandle->initialize();

  // The decision tool
  TrigDecisionTool trigDecTool("TrigDecTool");
  trigDecTool.setProperty("ConfigTool",configHandle);
  //  trigDecTool.setProperty("OutputLevel", MSG::VERBOSE);
  trigDecTool.setProperty("TrigDecisionKey","xTrigDecision");
  trigDecTool.initialize();


  TH1D* _h_TDT_Emulation_differences     = new TH1D("_h_TDT_Emulation_differences", "TDT_Emulation_differences",chains_to_test.size(),0,chains_to_test.size());
  TH1D* _h_TDT_fires     = new TH1D("_h_TDT_fires", "TDT_fires_total_number",chains_to_test.size(),0,chains_to_test.size());
  TH1D* _h_et = new TH1D("_h_et", "", 200, 0, 200000);
  TH1D* _h_eta = new TH1D("_h_eta", "", 50, -5, 5);
  TH2D* _h_et_eta = new TH2D("_h_et_eta", "_h_et_eta", 300, 0, 300, 100, -5, 5);

  int chain_number = 1;
  for (auto it: chains_to_test) {
    _h_TDT_Emulation_differences->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    _h_TDT_fires->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    chain_number++;
  }

  Long64_t entries = event.getEntries();
  for (Long64_t entry = 0; entry < entries; entry++) {
     if ((entry%200)==0)
       ::Info(APP_NAME, "Start processing event %d", (int)entry);
    // ::Info(APP_NAME, "Start processing event %d", (int)entry);

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


    CHECK(emulationTool.calculate(l1taus, l1jets, l1muons, l1xe));


    // Print the decision for all the tested chains and the TDT decision
    int chain_number = 0;
    bool TDT_emulation_decisions_unmatched = false;
    for (auto it: chains_to_test) {
      bool emulation_decision = emulationTool.decision(it);
      // std::cout << it << " emulation : " << emulation_decision << std::endl;
      auto chain_group = trigDecTool.getChainGroup(it);
      // bool cg_passes_event = chain_group->isPassedBits();  
      bool cg_passes_event = chain_group->isPassed();  
      // std::cout << it << " TDT : " <<  cg_passes_event << std::endl;
      if(cg_passes_event) _h_TDT_fires->Fill(it.c_str(), 1);
      if (emulation_decision != cg_passes_event){
	::Info(APP_NAME, "In event %d ", (int)entry);
	std::cout << "RunNumber: " << ei->runNumber() << ", EventNumber = " << ei->eventNumber() << std::endl;
        std::cout << "\t TDT and emulation decision different for " << it << ", TDT gives : " << cg_passes_event << " emulation gives : " << emulation_decision << std::endl;
	// std::cout << "Index | et8x8() | eta() | phi()" << std::endl;
	// for(const auto* jet: *l1jets) {
	//   std::cout << jet->index() << "|\t"
	// 	    << jet->et8x8()  << "|\t"
	// 	    << jet->eta() << "|\t"
	// 	    << jet->phi() << std::endl;
	//   if (jet->auxdataConst<bool>("J12")) {
	//     _h_et_eta->Fill(jet->et8x8() / 1000., jet->eta());
	//     _h_et->Fill(jet->et8x8());
	//     _h_eta->Fill(jet->eta());
	//   }
	// }
        _h_TDT_Emulation_differences->Fill(it.c_str(), 1);
        TDT_emulation_decisions_unmatched = true;
      }
      chain_number++;
    }

    // // Individual counts printout
    // emulationTool.PrintCounters();


    // ::Info(APP_NAME, "event %d processed", (int)entry);

  }
  CHECK(emulationTool.finalize());


  for (int ibin=0; ibin < _h_TDT_fires->GetNbinsX(); ibin++)
    std::cout <<  _h_TDT_fires->GetXaxis()->GetBinLabel(ibin + 1) << ": "<< _h_TDT_fires->GetBinContent(ibin+1) << std::endl;

  //write _h_TDT_fires and _h_TDT_Emulation_differences to root file here
  TFile *fOut = new TFile("output.root", "RECREATE");
  fOut->cd();
  _h_TDT_fires->Write();
  _h_TDT_Emulation_differences->Write();
  _h_et_eta->Write();
  _h_et->Write();
  _h_eta->Write();
  fOut->Close();

  return 0;
}

