/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <PATInterfaces/CorrectionCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <RootCoreUtils/Assert.h>
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
  if (argc != 3)
  {
    ATH_MSG_ERROR ("usage: " << argv[0] << " <xAOD.root> <output.root>");
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
  quickAna->muonDef = "medium loose";

  // We just need some files for testing. Do not use these in your analysis
  quickAna->muMcFiles = {
    "dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // use the optimized tool scheduler
  // quickAna->schedulerDef = "optimized";

  // initialize QuickAna tool
  ANA_CHECK (quickAna->initialize());

  // make an output histogram file and histograms
  TFile outputFile (output_file.c_str(), "RECREATE");
  TH1 *muon_n_any = new TH1F ("muon_n_any", "number of any muons", 10, -0.5, 9.5);
  TH1 *muon_n_medium = new TH1F ("muon_n_medium", "number of medium muons", 10, -0.5, 9.5);
  TH1 *muon_n_loose = new TH1F ("muon_n_loose", "number of loose muons", 10, -0.5, 9.5);
  TH1 *muon_pt_any = new TH1F ("muon_pt_any", "any muon pt", 100, 0, 500e3);
  TH1 *muon_pt_medium = new TH1F ("muon_pt_medium", "medium muon pt", 100, 0, 500e3);
  TH1 *muon_pt_loose = new TH1F ("muon_pt_loose", "loose muon pt", 100, 0, 500e3);

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
    unsigned muon_num_any = 0;
    unsigned muon_num_medium = 0;
    unsigned muon_num_loose = 0;
    double muon_weight_any = 1;
    double muon_weight_medium = 1;
    double muon_weight_loose = 1;
    for (auto muon : *quickAna->muons())
    {
      // first check the muons that are passing the medium working
      // point.  since we are only looking at one working point here,
      // we take both the selection and the weight from the medium
      // working point, so this is pretty much the same code as in the
      // basic example, but the name of the working point is
      // in-grained into the decoration.
      if (muon->auxdata<ana::SelectType> ("ana_select_medium"))
      {
	RCU_ASSERT (muon->isAvailable<float> ("ana_weight_medium"));
	float myweight = muon->auxdata<float> ("ana_weight_medium");
	RCU_ASSERT (myweight > 0);
	muon_pt_medium->Fill (muon->pt(), quickAna->weight() * myweight);
	++ muon_num_medium;

	// if you use multiple working points, the object weight is
	// not included in the event weight, instead you need to track
	// it separately, as QuickAna has no way of knowing which
	// working point you choose for which object.
	muon_weight_medium *= myweight;
      }

      // same code for loose working points as for medium
      if (muon->auxdata<ana::SelectType> ("ana_select_loose"))
      {
	float myweight = muon->auxdata<float> ("ana_weight_loose");
	muon_pt_loose->Fill (muon->pt(), quickAna->weight() * myweight);
	++ muon_num_loose;
	muon_weight_loose *= myweight;
      }

      // now this code works with muons that pass any working point.
      // those muons are used with MET calculation and overlap
      // removal.  for actual analysis usage this is a bit tricky
      // though, since you have to figure out which weight to use: is
      // it loost weight for all? is it loose weight for loose muons
      // and medium for medium?  is the loose weight actually
      // appropriate for loose-but-not-medium muons?  these are
      // questions that don't have easy answers, so for this example
      // we fudge and just set the weight to 1, which is so clearly
      // wrong that hopefully nobody is going to copy that for their
      // analysis.
      if (muon->auxdata<ana::SelectType> ("ana_select"))
      {
	float myweight = 1; // <- DON'T DO THIS!!!
	muon_pt_any->Fill (muon->pt(), quickAna->weight() * myweight);
	++ muon_num_any;
	muon_weight_any *= myweight;
      }
    }
    muon_n_any->Fill (muon_num_any, quickAna->weight() * muon_weight_any);
    muon_n_medium->Fill (muon_num_medium, quickAna->weight() * muon_weight_medium);
    muon_n_loose->Fill (muon_num_loose, quickAna->weight() * muon_weight_loose);
  }

  outputFile.Write ();
  ATH_MSG_INFO ("finished processing successfully");

  return EXIT_SUCCESS;
}
