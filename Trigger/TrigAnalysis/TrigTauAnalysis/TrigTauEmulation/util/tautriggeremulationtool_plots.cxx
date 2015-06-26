// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// $Id$
#ifdef ROOTCORE
// Simple plotting script to test code
// based on ut_tautriggeremulationtool.cxx
// To be updated

// System include(s):
#include <memory>
#include <set>
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TFile.h>
#include "TSystem.h"
#include "TPad.h"
#include "TLatex.h"
#include "TError.h"
#include "TColor.h"
#include "TImage.h"
#include "THStack.h"
#include "TLegend.h"
#include <stdexcept>
#include <map>
#include "math.h"
#include "TPaveStats.h"

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

// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"

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

int main() {

  // Get the name of the application:
  const char* APP_NAME = "ut_tautriggeremulationtool_plots_test";

  // Initialise the environment:
  RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

  ToolsRegistry registry("ToolsRegistry");
  CHECK(registry.initialize());

  // Initialize the tool
  TrigTauEmul::Level1EmulationTool emulationTool("TauTriggerEmulator");
  std::vector<std::string> chains_to_test;

  chains_to_test.push_back("L1_TAU12");
  //chains_to_test.push_back("L1_TAU12IL");
  //chains_to_test.push_back("L1_TAU12IM");
  //chains_to_test.push_back("L1_TAU12IT");
  //chains_to_test.push_back("L1_TAU20");
  //chains_to_test.push_back("L1_TAU20IL");
  //chains_to_test.push_back("L1_TAU20IM");
  //chains_to_test.push_back("L1_TAU20IT");
  //chains_to_test.push_back("L1_TAU30");
  //chains_to_test.push_back("L1_TAU40");
  //chains_to_test.push_back("L1_TAU60");
  //chains_to_test.push_back("L1_TAU8");
  //chains_to_test.push_back("L1_TAU20IM_2TAU12IM");
  //chains_to_test.push_back("L1_TAU20_2TAU12");
  //chains_to_test.push_back("L1_EM15HI_2TAU12");
  //chains_to_test.push_back("L1_EM15HI_2TAU12IM");
  //chains_to_test.push_back("L1_EM15HI_2TAU12IM_J25_3J12");
  //chains_to_test.push_back("L1_EM15HI_2TAU12_J25_3J12");
  //chains_to_test.push_back("L1_EM15HI_TAU40_2TAU15");
  //chains_to_test.push_back("L1_J25_3J12_EM15-TAU12I");
  //chains_to_test.push_back("L1_MU10_TAU12");
  //chains_to_test.push_back("L1_MU10_TAU12IM");
  //chains_to_test.push_back("L1_MU10_TAU12IM_J25_2J12");
  //chains_to_test.push_back("L1_MU10_TAU12IL_J25_2J12");
  //chains_to_test.push_back("L1_MU10_TAU12_J25_2J12");
  //chains_to_test.push_back("L1_J25_2J12_DR-MU10TAU12I");
  //chains_to_test.push_back("L1_MU10_TAU20");
  //chains_to_test.push_back("L1_MU10_TAU20IM");
  //chains_to_test.push_back("L1_TAU25IT_2TAU12IT_2J25_3J12");
  //chains_to_test.push_back("L1_TAU20IL_2TAU12IL_J25_2J20_3J12");
  //chains_to_test.push_back("L1_TAU20IM_2TAU12IM_J25_2J20_3J12");
  //chains_to_test.push_back("L1_TAU20_2TAU12_J25_2J20_3J12");
  ////  chains_to_test.push_back("L1_J25_2J20_3J12_BOX-TAU20ITAU12I");
  //chains_to_test.push_back("L1_J25_2J20_3J12_DR-TAU20ITAU12I");
  //chains_to_test.push_back("L1_DR-MU10TAU12I_TAU12I-J25");
  //chains_to_test.push_back("L1_MU10_TAU12I-J25");
  //chains_to_test.push_back("L1_TAU20IM_2J20_XE45");
  //chains_to_test.push_back("L1_TAU20_2J20_XE45");
  //chains_to_test.push_back("L1_TAU25_2J20_XE45");
  //chains_to_test.push_back("L1_TAU20IM_2J20_XE50");
  //chains_to_test.push_back("L1_XE45_TAU20-J20");
  //chains_to_test.push_back("L1_EM15HI_2TAU12IM_XE35");
  //chains_to_test.push_back("L1_EM15HI_2TAU12IL_XE35");
  //chains_to_test.push_back("L1_EM15HI_2TAU12_XE35");
  //chains_to_test.push_back("L1_XE35_EM15-TAU12I");
  //chains_to_test.push_back("L1_XE40_EM15-TAU12I");
  //chains_to_test.push_back("L1_MU10_TAU12_XE35");
  //chains_to_test.push_back("L1_MU10_TAU12IM_XE35");
  //chains_to_test.push_back("L1_MU10_TAU12IL_XE35");
  //chains_to_test.push_back("L1_MU10_TAU12IT_XE35");
  //chains_to_test.push_back("L1_MU10_TAU12IM_XE40");
  //chains_to_test.push_back("L1_TAU20IM_2TAU12IM_XE35");
  //chains_to_test.push_back("L1_TAU20IL_2TAU12IL_XE35");
  //chains_to_test.push_back("L1_TAU20IT_2TAU12IT_XE35");
  //chains_to_test.push_back("L1_TAU20_2TAU12_XE35");
  //chains_to_test.push_back("L1_TAU20IM_2TAU12IM_XE40");
  //chains_to_test.push_back("L1_DR-MU10TAU12I");
  //chains_to_test.push_back("L1_TAU12I-J25");
  //chains_to_test.push_back("L1_EM15-TAU40");
  //chains_to_test.push_back("L1_TAU20-J20");
  //chains_to_test.push_back("L1_EM15-TAU12I");
  //chains_to_test.push_back("L1_EM15TAU12I-J25");
  //chains_to_test.push_back("L1_DR-EM15TAU12I-J25");
  //chains_to_test.push_back("L1_TAU20ITAU12I-J25");
  //chains_to_test.push_back("L1_DR-TAU20ITAU12I");
  ////  chains_to_test.push_back("L1_BOX-TAU20ITAU12I");
  //chains_to_test.push_back("L1_DR-TAU20ITAU12I-J25");
  //chains_to_test.push_back("L1_EM15");
  //chains_to_test.push_back("L1_EM15HI");
  //chains_to_test.push_back("L1_J12");
  //chains_to_test.push_back("L1_J20");
  //chains_to_test.push_back("L1_J25");
  //chains_to_test.push_back("L1_XE35");
  //chains_to_test.push_back("L1_XE40");
  //chains_to_test.push_back("L1_XE50");
  //chains_to_test.push_back("L1_MU10");

  CHECK(emulationTool.setProperty("l1_chains", chains_to_test));
  //emulationTool.msg().setLevel(MSG::DEBUG);
  CHECK(emulationTool.initialize());

  //static const char* FNAME = "/afs/cern.ch/work/q/qbuat/public/mc15_13TeV/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.AOD.e3601_s2576_s2132_r6630/AOD.05367718._006348.pool.root.1";
  //static const char* FNAME = "/afs/cern.ch/work/g/gbesjes/public/data15_13TeV.00267639.express_express.recon.AOD.r6815_tid05695816_00/AOD.05695816._000165.pool.root.1";
  static const char *FNAME = "/afs/cern.ch/user/g/gbesjes/work/public/data15_13TeV.00267639.physics_Main.recon.AOD.r6818_tid05695957_00/AOD.05695957._002298.pool.root.1";

  std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );

  if( ! ifile.get() ) {
    ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
        FNAME );
    return 1;
  }
  // change

  // Create the TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  CHECK( event.readFrom(ifile.get()) );


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

  //HISTOGRAMS
  std::string canname = "TDT_Emulation_differences";
  TCanvas* can=new TCanvas(canname.c_str(),canname.c_str(),800,600);
  TH1D* _h_TDT_Emulation_differences     = new TH1D("_h_TDT_Emulation_differences", "TDT_Emulation_differences",chains_to_test.size(),0,chains_to_test.size());
  TH1D* _h_TDT_fires     = new TH1D("_h_TDT_fires", "TDT_fires_total_number",chains_to_test.size(),0,chains_to_test.size());
  int chain_number = 1;
  for (auto it: chains_to_test) {
    _h_TDT_Emulation_differences->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    _h_TDT_fires->GetXaxis()->SetBinLabel(chain_number, it.c_str());
    chain_number++;
  }

  int nevents_with_two_taus = 0;
  Long64_t entries = event.getEntries();
  for (Long64_t entry = 0; entry < entries; entry++) {
    if (nevents_with_two_taus > 10)
      break;
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


    CHECK(emulationTool.calculate(l1taus, l1jets, l1muons, l1xe));


    // Print the decision for all the tested chains and the TDT decision
    int chain_number = 0;
    bool TDT_emulation_decisions_unmatched = false;
    for (auto it: chains_to_test) {
      //       const std::string chain = it.c_str();
      bool emulation_decision = emulationTool.decision(it);
      std::cout << it << " emulation : " << emulation_decision << std::endl;
      auto chain_group = trigDecTool.getChainGroup(it);
      bool cg_passes_event = chain_group->isPassed();  
      std::cout << it << " TDT : " <<  cg_passes_event << std::endl;
      if(cg_passes_event) _h_TDT_fires->Fill(it.c_str(), 1);
      if (emulation_decision != cg_passes_event){
        std::cout << "TDT and emulation decision different for " << it << ", TDT gives : " << cg_passes_event << " emulation gives : " << emulation_decision << std::endl;
        _h_TDT_Emulation_differences->Fill(it.c_str(), 1);
        TDT_emulation_decisions_unmatched = true;
      }
      chain_number++;
    }
    // Individual counts printout
    emulationTool.PrintCounters();

    // Print out event information for debugging
    if(TDT_emulation_decisions_unmatched){
      // here print out the L1emtau, jet, muon, XE information for the event to debug differences

      std::cout << " " << std::endl;
      std::cout << "print L1 emtau info for event " << std::endl;
      std::cout << "L1 tau information : " << std::endl;
      std::cout << " " << std::endl;
      for (unsigned int i_l1taus=0;i_l1taus<l1taus->size();i_l1taus++) {
        const xAOD::EmTauRoI *L1Tau = l1taus->at(i_l1taus);
        double tauClus = L1Tau->tauClus();
        double EMIsol = L1Tau->emIsol();
        double L1_tau_eta = L1Tau->eta();
        double L1_tau_phi = L1Tau->phi();
        auto L1_type = L1Tau->roiType(); // L1_type == 2 for TAU items
        std::cout << "tauindex : " << i_l1taus << std::endl;
        std::cout << "L1_type : " << L1_type << std::endl;
        std::cout << "tauClus : " << tauClus << std::endl;
        std::cout << "EMIsol : " << EMIsol << std::endl;
        std::cout << "L1_tau_eta : " << L1_tau_eta << std::endl;
        std::cout << "L1_tau_phi : " << L1_tau_phi << std::endl;
        std::cout << " " << std::endl;
      }

      std::cout << " " << std::endl;
      std::cout << "L1 jet information " << std::endl;
      std::cout << " " << std::endl;
      for (unsigned int i_l1jets=0; i_l1jets<l1jets->size(); i_l1jets++){
        const xAOD::JetRoI *L1Jet = l1jets->at(i_l1jets);

        double L1_Jet_et8x8 = L1Jet->et8x8();
        double L1_Jet_eta = L1Jet->eta();
        double L1_Jet_phi = L1Jet->phi();
        std::cout << "L1_Jet_et8x8 : " << L1_Jet_et8x8 << std::endl;
        std::cout << "L1_Jet_eta : " << L1_Jet_eta << std::endl;
        std::cout << "L1_Jet_phi : " << L1_Jet_phi<< std::endl;
        std::cout << " " << std::endl;
      }

      std::cout << " " << std::endl;
      std::cout << "L1 muon information " << std::endl;
      std::cout << " " << std::endl;
      for (unsigned int i_l1muons=0;i_l1muons<l1muons->size();i_l1muons++) {
        const xAOD::MuonRoI *L1MU = l1muons->at(i_l1muons);
        double mu_eta = L1MU->eta();
        double mu_phi = L1MU->phi();
        double mu_pt = L1MU->thrValue();
        std::cout << "muonindex : " << i_l1muons << std::endl;
        std::cout << "mu_pt : " << mu_pt << std::endl;
        std::cout << "mu_eta : " << mu_eta << std::endl;
        std::cout << "mu_phi : " << mu_phi << std::endl;
        std::cout << " " << std::endl;
      }

      std::cout << " " << std::endl;
      std::cout << "L1 XE information " << std::endl;
      std::cout << " " << std::endl;
      double ENERGYSUM_et = (l1xe->energyT());
      double ENERGYSUM_ex = (l1xe->energyX());
      double ENERGYSUM_ey = (l1xe->energyY());
      std::cout << "EnergySum Total " << ENERGYSUM_et << std::endl;
      double L1_XE_met = TMath::Sqrt(TMath::Power(ENERGYSUM_ex,2)+TMath::Power(ENERGYSUM_ey,2));
      std::cout << "EnergySumT calc " << L1_XE_met << std::endl;
    }


    ::Info(APP_NAME, "event %d processed", (int)entry);

  }
  CHECK(emulationTool.finalize());

  _h_TDT_fires->Draw();
  _h_TDT_fires->SetLineColor(kRed);
  _h_TDT_Emulation_differences->Draw("SAMES");
  can->Update();
  TPaveStats *stats2 = (TPaveStats*)can->GetPrimitive("stats");
  stats2->SetName("h1stats2");
  stats2->SetX1NDC(.1);
  stats2->SetX2NDC(.3);
  can->SetBottomMargin(0.4);
  can->Update();
  can->SaveAs("TDT_totalFires_and_differences_to_Emulation.C");


  return 0;
}
#endif
