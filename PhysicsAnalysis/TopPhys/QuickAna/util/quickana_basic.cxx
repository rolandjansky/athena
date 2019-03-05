/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/CorrectionCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <TFile.h>
#include <TH1.h>
#include <chrono>
#include <xAODRootAccess/Init.h>

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
    ATH_MSG_ERROR ("usage: " << argv[0]
		   << " <xAOD.root> <output.root> [<schedulerDef>]");
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
  // Shows how to set the message level of quickana's tools
  quickAna->msgLevel = MSG::INFO;

  // We just need some files for testing. Do not use these in your analysis
  quickAna->muMcFiles = {
    //"dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    //"dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
    "dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // use the optimized tool scheduler
  if (argc>3) quickAna->schedulerDef = argv[3]; // "optimized";

  // initialize QuickAna tool
  ANA_CHECK (quickAna->initialize());

  // make an output histogram file and histograms
  TFile outputFile (output_file.c_str(), "RECREATE");
  TH1 *muon_n = new TH1F ("muon_n", "number of muons", 10, -0.5, 9.5);
  TH1 *muon_pt = new TH1F ("muon_pt", "muon pt", 100, 0, 500e3);
  TH1 *met_pt = new TH1F ("met_pt", "MET pt", 100, 0, 500e3);
  TH1 *jet_n = new TH1F ("jet_n", "number of jets", 20, -0.5, 19.5);
  TH1 *jet_pt = new TH1F ("jet_pt", "jet pt", 100, 0, 500e3);

  Long64_t entries = event.getEntries();
  if (entries > 100)
    entries = 100;
  ATH_MSG_INFO ("Processing " << entries << " entries");

  const auto startTime = std::chrono::steady_clock::now();
  auto lastPrintoutTime = startTime;
  for (Long64_t entry = 0; entry < entries; ++ entry)
  {
    const auto currentStartTime = std::chrono::steady_clock::now();
    if (currentStartTime - lastPrintoutTime > std::chrono::seconds (5))
    {
      lastPrintoutTime = currentStartTime;
      float rate = entry;
      rate /= (lastPrintoutTime - startTime) / std::chrono::seconds(1);
      ATH_MSG_INFO ("Processing entry " << entry << " " << rate << "evt/s");
    }

    store.clear ();
    event.getEntry (entry);

    // run QuickAna on this event
    ANA_CHECK (quickAna->process (event));

    // loop over muons and fill pt
    unsigned muon_num = 0;
    for (auto muon : *quickAna->muons())
    {
      if (muon->auxdata<ana::SelectType> ("ana_select"))
      {
        ++ muon_num;
        muon_pt->Fill (muon->pt(), quickAna->weight());
      }
    }
    muon_n->Fill (muon_num, quickAna->weight());

    // loop over jets and fill pt
    unsigned jet_num = 0;
    for (auto jet : *quickAna->jets())
    {
      if (jet->auxdata<ana::SelectType> ("ana_select"))
      {
        ++ jet_num;
        jet_pt->Fill (jet->pt(), quickAna->weight());
      }
    }
    jet_n->Fill (jet_num, quickAna->weight());

    // fill MET
    met_pt->Fill (quickAna->met()->met(), quickAna->weight());

  }

  outputFile.Write ();
  ATH_MSG_INFO ("finished processing successfully");

  return EXIT_SUCCESS;
}
