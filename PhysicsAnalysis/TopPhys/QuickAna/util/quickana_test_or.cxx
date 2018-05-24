/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// @brief This file contains testing code for the overlap removal in QuickAna.
///
/// It is currently being used to test the migration to the new tools design
/// in AssociationUtils.
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///

// System includes
#include <chrono>

// ROOT includes
#include "TFile.h"

// ASG toolkit includes
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "PATInterfaces/CorrectionCode.h"

// Local includes
#include "QuickAna/QuickAna.h"
#include "QuickAna/MessageCheck.h"


//-----------------------------------------------------------------------------
// Main program
//-----------------------------------------------------------------------------
int main (int argc, char **argv)
{
  // declare the return type of this function for ANA_CHECK
  ANA_CHECK_SET_TYPE (int);

  // needed for messaging macros
  using namespace ana::msgUserCode;

  // make unchecked status codes abort the job
  ana::enableStatusCodeFailure ();

  // initialize xAOD classes
  ANA_CHECK (xAOD::Init (argv[0]));

  // The application's name
  const char* APP_NAME = argv[0];

  // Command line processing
  if(argc < 2) {
    Error(APP_NAME, "No file name received!" );
    Error(APP_NAME, "Usage: %s [xAOD.root]", APP_NAME);
    return EXIT_FAILURE;
  }
  const std::string inputFileName = argv[1];

  // Open and attach input xAOD file
  std::unique_ptr<TFile> file( TFile::Open(inputFileName.c_str()) );
  xAOD::TEvent event (xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK( event.readFrom(file.get()) );

  // Create QuickAna tool
  ana::QuickAna quickAna("quickana");

  // Configure object definitions
  quickAna.eventinfoDef = "default";
  quickAna.muonDef = "default";
  quickAna.electronDef = "default";
  quickAna.photonDef = "default";
  quickAna.tauDef = "default";
  quickAna.jetDef = "default";
  quickAna.jetKine = "pt > 20e3";
  quickAna.metDef = "default";
  quickAna.orDef = "experimental";
  quickAna.schedulerDef = "basic"; // "optimized";

  // We just need some files for testing. Do not use these in your analysis
  quickAna.muMcFiles = {
    //"dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    //"dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
    "dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna.muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // Initialize QuickAna tool
  ANA_CHECK( quickAna.initialize() );

  // Number of events to process
  Long64_t entries = event.getEntries();
  if (entries > 100)
    entries = 100;
  ANA_MSG_INFO ("Processing " << entries << " entries");

  // Event loop
  const auto startTime = std::chrono::steady_clock::now();
  auto lastPrintoutTime = startTime;
  for (Long64_t entry = 0; entry < entries; ++entry) {
    const auto currentStartTime = std::chrono::steady_clock::now();
    if (currentStartTime - lastPrintoutTime > std::chrono::seconds (5)) {
      lastPrintoutTime = currentStartTime;
      float rate = entry;
      rate /= (lastPrintoutTime - startTime) / std::chrono::seconds(1);
      Info(APP_NAME, "Processing entry %lli, rate %fevts/s", entry, rate);
    }

    store.clear();
    event.getEntry(entry);

    // Run QuickAna on this event
    ANA_CHECK( quickAna.process(event) );

  }

  Info(APP_NAME, "finished processing successfully");
  return EXIT_SUCCESS;
}
