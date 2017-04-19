/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <PATInterfaces/CorrectionCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <TFile.h>
#include <TH1.h>
#include <chrono>
#include <xAODRootAccess/Init.h>

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/multi_index/random_access_index.hpp>
#pragma GCC diagnostic pop

#include <AsgTools/AnaToolHandle.h>
#include <TrigConfxAOD/xAODConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>

//
// main program
//

int main (int argc, char **argv)
{
  // declare the return type of this function for ANA_CHECK
  ANA_CHECK_SET_TYPE (int);

  // needed for messaging macros
  using namespace ana::msgUserCode;

  // make unchecked status codes abort the job
  ana::enableStatusCodeFailure ();

  // initialize xAOD classes
  ANA_CHECK (xAOD::Init ());


  // command line processing
  if (argc < 3 || argc > 4)
  {
    ATH_MSG_ERROR ("usage: " << argv[0] << " <xAOD.root> <output.root> [<schedulerDef>]");
    return EXIT_FAILURE;
  }
  const std::string input_file = argv[1];
  const std::string output_file = argv[2];

  // open and attach input xAOD file
  std::unique_ptr<TFile> file (TFile::Open (input_file.c_str()));
  xAOD::TEvent event (xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK (event.readFrom (file.get()));

  // create a new QuickAna tool
  ana::QuickAna *quickAna = new ana::QuickAna ("quickana");

  // request the actual object definitions to use
  quickAna->eventinfoDef = "default";
  quickAna->muonDef = "default";
  quickAna->electronDef = "default";
  quickAna->photonDef = "none";
  quickAna->tauDef = "default";
  quickAna->jetDef = "default";
  // quickAna->jetDef = "AntiKt4EMTopo";
  quickAna->jetKine = "pt > 50e3";
  quickAna->metDef = "default";
  quickAna->orDef = "default";
  quickAna->triggerDef = "SingleE SingleMu"; //Defined in ROOT/TriggerTool.cxx

  // We just need some files for testing. Do not use these in your analysis
  quickAna->muMcFiles = {
    "dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // use the optimized tool scheduler
  if (argc>3) quickAna->schedulerDef = argv[3]; // "optimized";

  // initialize QuickAna tool
  ANA_CHECK (quickAna->initialize());

  // The configuration tool.
  asg::AnaToolHandle<TrigConf::xAODConfigTool> configTool
    ("xAODConfigTool", nullptr);
  ATH_CHECK (ASG_MAKE_ANA_TOOL (configTool, TrigConf::xAODConfigTool));
  ANA_CHECK (configTool.initialize());
  // The decision tool
  asg::AnaToolHandle<Trig::TrigDecisionTool> trigDecTool
    ("TrigDecisionTool", nullptr);
  ATH_CHECK (ASG_MAKE_ANA_TOOL (trigDecTool, Trig::TrigDecisionTool));
  ANA_CHECK (trigDecTool.setProperty("ConfigTool",configTool));
  //trigDecTool.setProperty("OutputLevel", MSG::VERBOSE);
  ANA_CHECK (trigDecTool.setProperty("TrigDecisionKey","xTrigDecision"));
  ANA_CHECK (trigDecTool.initialize());



  // make an output histogram file and histograms
  TFile outputFile (output_file.c_str(), "RECREATE");
  TH1 *muon_loose_sf = new TH1F ("muon_trig_sf_loose", "Loose Muon Trigger SF", 20, 0 ,2);
  TH1 *muon_medium_sf = new TH1F ("muon_trig_sf_meiudm", "Medium Muon Trigger SF", 20, 0 ,2);
  TH1 *muon_tight_sf = new TH1F ("muon_trig_sf_tight", "Tight Muon Trigger SF", 20, 0 ,2);

  TH1 *muon_tr_match = new TH1F ("muon_trig_match", "Trigger Matched Muons", 4, 0, 2);
  TH1 *el_tr_match = new TH1F ("el_trig_match", "Trigger Matched Electrons", 4, 0, 2);
  TH1 *tr_pass = new TH1F ("trig_pass", "events passing trigger", 4, 0, 2);
  TH1 *tr_e_pass = new TH1F ("trig_e_pass", "events passing electron triggers", 4, 0, 2);
  TH1 *tr_m_pass = new TH1F ("trig_mu_pass", "events passing muon triggers", 4, 0, 2);

  Long64_t entries = event.getEntries();
  if (entries > 100)
    entries = 100;
  ATH_MSG_INFO ("Processing " << entries << " entries");
  bool start=true;

  for (Long64_t entry = 0; entry < entries; ++ entry)
  {
    store.clear ();
    event.getEntry (entry);

    if(start){
      auto chainGroup=trigDecTool->getChainGroup(".*");
      ATH_MSG_INFO ("----------Available Triggers ---------");
      for(auto &trig : chainGroup->getListOfTriggers())
      {
	ATH_MSG_INFO (trig);
      }
      start=false;
    }

    // run QuickAna on this event
    ANA_CHECK (quickAna->process (event));

    //Check if event passes any of the triggers
    auto evtInfo = quickAna->eventinfo();

    tr_pass->Fill(evtInfo->auxdata<bool>("passAllTrig"));
    //Check if event passes electron group

    tr_e_pass->Fill(evtInfo->auxdata<bool>("SingleE_passTrig"));

    //Check if event passes muon group
    tr_m_pass->Fill(evtInfo->auxdata<bool>("SingleMu_passTrig"));

    //Get the Muon Trigger Scale factors for different working points
    muon_loose_sf->Fill(evtInfo->auxdata<double>("SingleMu_Mu_LooseTrigSF"));
    muon_medium_sf->Fill(evtInfo->auxdata<double>("SingleMu_Mu_MediumTrigSF"));
    muon_tight_sf->Fill(evtInfo->auxdata<double>("SingleMu_Mu_TightTrigSF"));


    // loop over muons and fill pt
    for (auto muon : *quickAna->muons())
    {
      //Check if Muon matching to a trigger in the SingleMu group
      if (muon->auxdata<ana::SelectType> ("ana_select"))
        muon_tr_match->Fill(muon->auxdata<bool>("SingleMu_trigMatch"));
    }

    for (auto el : *quickAna->electrons())
    {
      //Check if electron matchis to a trigger in the SingleE group
      if (el->auxdata<ana::SelectType> ("ana_select"))
        el_tr_match->Fill(el->auxdata<bool>("SingleE_trigMatch"));
    }
  }

  outputFile.Write ();
  ATH_MSG_INFO ("finished processing successfully");

  return EXIT_SUCCESS;
}
