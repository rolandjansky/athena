/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/CorrectionCode.h>
#include <PATInterfaces/SystematicsUtil.h>
#include <QuickAna/Global.h>
#include <QuickAna/MasterOutputToolXAOD.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <QuickAna/EventData.h>
#include <TFile.h>
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


  // Command line processing
  if (argc != 3)
  {
    ATH_MSG_FATAL ("usage: " << argv[0] << " <xAOD.root> <output.root>");
    return EXIT_FAILURE;
  }
  const std::string inputFileName = argv[1];
  const std::string outputFileName = argv[2];

  // Open and attach input xAOD file
  std::unique_ptr<TFile> inputFile (TFile::Open (inputFileName.c_str()));
  xAOD::TEvent event (xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK (event.readFrom (inputFile.get()));

  // Open and attach output file
  std::unique_ptr<TFile> outputFile;
  if (outputFileName != "no_output")
  {
    outputFile = std::make_unique<TFile> (outputFileName.c_str(), "RECREATE");
    ANA_CHECK (event.writeTo(outputFile.get()));
  }

  // Create a new QuickAna tool
  ana::QuickAna *quickAna = new ana::QuickAna ("quickana");

  // Request the actual object definitions to use
  quickAna->eventinfoDef = "default";
  quickAna->muonDef = "default";
  quickAna->electronDef = "default";
  // quickAna->photonDef = "default";
  quickAna->tauDef = "default";
  quickAna->jetDef = "default";
  quickAna->metDef = "default";
  quickAna->orDef = "default";
  quickAna->schedulerDef = "optimized";

  // We just need some files for testing. Do not use these in your analysis
  quickAna->muMcFiles = {
    //"dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    //"dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
    "dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // Initialize QuickAna tool
  ANA_CHECK (quickAna->initialize());

  // Configure systematics
  auto sysList = CP::make_systematics_vector (quickAna->recommendedSystematics());
  ANA_CHECK (quickAna->setSystematics(sysList));

  // The master output tool for analysis xAODs
  std::unique_ptr<ana::MasterOutputToolXAOD> outputTool;
  if (outputFile)
  {
    outputTool = std::make_unique<ana::MasterOutputToolXAOD> ("OutputTool");

    // Assign a subset of object types to write out
    std::vector<ana::ObjectType> outputTypes = {
      ana::OBJECT_EVENTINFO, ana::OBJECT_MUON,
      ana::OBJECT_ELECTRON, ana::OBJECT_JET, ana::OBJECT_MET
    };

    ANA_CHECK (outputTool->setProperty("OutputTypes", outputTypes));

    // Configure output tool with the EventData
    ANA_CHECK (outputTool->setProperty("EventData", quickAna->getEventData()));

    // Initialize the output tool
    ANA_CHECK (outputTool->initialize());

    // Specify the aux-item list to write out
    event.setAuxItemList("EventInfoAux.","mcChannelNumber.mcEventNumber.mcEventWeights");
    event.setAuxItemList("ElectronsAux.", "pt.eta.phi.m");
    event.setAuxItemList("MuonsAux.", "pt.eta.phi.m");
    event.setAuxItemList("AntiKt4LCTopoJetsAux.", "pt.eta.phi.JVF");
  }

  Long64_t entries = event.getEntries();
  // Small number of entries for testing
  if (entries > 100)
    entries = 100;
  ANA_MSG_INFO ("Processing " << entries << " entries");

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
      ANA_MSG_INFO ("Processing entry " << entry << " " << rate << "evt/s");
    }

    event.getEntry (entry);

    ANA_MSG_INFO ("pre event processing");

    for (auto sys : quickAna->systematics())
    {
      // Apply systematic variation
      ANA_CHECK (quickAna->applySystematicVariation (sys));

      // Run QuickAna on this event
      ANA_CHECK (quickAna->process(event));
    }

    ANA_MSG_INFO ("post event processing");

    if (outputTool)
    {
      ANA_MSG_INFO ("pre output tool write");

      // Write analysis objects to output store
      ANA_CHECK (outputTool->write());

      // Save this event
      ANA_CHECK (event.fill() > 0);

      ANA_MSG_INFO ("post output tool write");
    }

    store.clear ();
  }
  ANA_MSG_INFO ("finished event loop");

  if (outputFile)
  {
    // Finalize output
    ANA_CHECK (event.finishWritingTo(outputFile.get()));
    ANA_MSG_INFO ("processed file: " << inputFileName);
  }

  return EXIT_SUCCESS;
}
