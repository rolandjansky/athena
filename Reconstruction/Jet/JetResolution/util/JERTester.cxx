/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s)
#include <memory>
#include <cstdlib>

// ROOT include(s)
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// Infrastructure include(s)
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s)
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

// Local include(s)
#include "JetResolution/JERTool.h"
#include "JetResolution/JERSmearingTool.h"

// Other includes
#include "PATInterfaces/SystematicsUtil.h"
//#include "PATInterfaces/CorrectionCode.h"

// Error checking macro
#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      ::Error(APP_NAME, "Failed to execute: \"%s\"", \
              #ARG );                                \
      return 1;                                      \
    }                                                \
  } while( false )


int main( int argc, char* argv[] ) {

  // The application's name
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name
  if(argc < 2) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
    return 1;
  }

  // Initialise the application
  CHECK( xAOD::Init(APP_NAME) );
  StatusCode::enableFailure();
  CP::CorrectionCode::enableFailure();
  CP::SystematicCode::enableFailure();

  // Open the input file
  const TString fileName = argv[1];
  Info(APP_NAME, "Opening file: %s", fileName.Data());
  std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
  CHECK( ifile.get() );

  // Create a TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  CHECK( event.readFrom(ifile.get()) );
  Info(APP_NAME, "Number of events in the file: %i",
       static_cast<int>(event.getEntries()));

  // Decide how many events to run over
  Long64_t entries = event.getEntries();
  if(argc > 2) {
    const Long64_t e = atoll(argv[2]);
    if(e < entries) {
      entries = e;
    }
  }

  // Create the tools with default config to test
  JERTool jerTool("JERTool");
  JERSmearingTool smearTool("JERSmearingTool");

  // Configure the JERTool.
  // I am demonstrating all the defaults here, so behavior
  // is the same as not specifying any configuration
  //jerTool.msg().setLevel(MSG::DEBUG);
  CHECK( jerTool.setProperty("PlotFileName",
			     "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root") );
  //CHECK( jerTool.setProperty("JetAlg", JER::AKt4EM) );
  CHECK( jerTool.setProperty("CollectionName", "AntiKt4EMTopoJets") );


  // Configure the JERSmearingTool
  smearTool.msg().setLevel(MSG::DEBUG);
  ToolHandle<IJERTool> jerHandle(jerTool.name());
  CHECK( smearTool.setProperty("JERTool", jerHandle) );
  CHECK( smearTool.setProperty("ApplyNominalSmearing", false) );
  CHECK( smearTool.setProperty("isMC", true) );
  CHECK( smearTool.setProperty("SystematicMode", "Full") );

  // Initialize the tools
  CHECK( jerTool.initialize() );
  CHECK( smearTool.initialize() );

  // Build a simple list of systematics
  const std::vector<CP::SystematicSet> sysList =
    CP::make_systematics_vector( smearTool.recommendedSystematics() );

  // Loop over the events
  for(Long64_t entry = 0; entry < entries; ++entry) {

    // Tell the object which entry to look at
    event.getEntry(entry);

    // Print some event information for fun
    const xAOD::EventInfo* ei = 0;
    CHECK( event.retrieve(ei, "EventInfo") );
    Info(APP_NAME,
         "===>>>  start processing event #%i, "
         "run #%i %i events processed so far  <<<===",
         static_cast<int>(ei->eventNumber()),
         static_cast<int>(ei->runNumber()),
         static_cast<int>(entry));

    // Get the jets from the event
    const xAOD::JetContainer* jets = 0;
    CHECK( event.retrieve(jets, "AntiKt4LCTopoJets") );
    Info(APP_NAME, "Number of jets: %i",
         static_cast<int>(jets->size()));

    xAOD::JetContainer::const_iterator jetItr;
    for(jetItr = jets->begin(); jetItr != jets->end(); ++jetItr){

      const xAOD::Jet* jet = *jetItr;

      // Print basic info about this jet
      Info(APP_NAME, "Jet: pt = %g, eta = %g", jet->pt()/1000., jet->eta());

      // Get the resolution in MC and data
      double resMC = jerTool.getRelResolutionMC(jet);
      double resData = jerTool.getRelResolutionData(jet);
      // Get the resolution uncertainty
      double uncert = jerTool.getUncertainty(jet, JER::JER_NP_ALL);

      //DV

      // Print the resolution information
      Info(APP_NAME, "  Resolution MC   = %g", resMC);
      Info(APP_NAME, "  Resolution Data = %g", resData);
      Info(APP_NAME, "  Resolution Err  = %g", uncert);

      // Loop over systematic sets
      for(const auto& sysSet : sysList){
        std::string sysName = sysSet.empty()? "nominal" : sysSet.name();
        Info(APP_NAME, "  Correcting jets with sysSet: %s",
             sysName.c_str());

        // Apply systematic configuration to the smearing tool
        CHECK( smearTool.applySystematicVariation(sysSet) );

        // Now smear the jet
        xAOD::Jet* newJet = 0;
        CP::CorrectionCode c = smearTool.correctedCopy(*jet, newJet);
        if(c != CP::CorrectionCode::Ok)
          Error(APP_NAME, "problem in correctedCopy");
        //const xAOD::FourMom_t& lv = newJet->p4();
        Info(APP_NAME, "    New Jet: pt = %g, eta = %g",
             newJet->pt()/1000., newJet->eta());
        delete newJet;
      }

    }

    // Close with a message
    Info(APP_NAME,
         "===>>>  done processing event #%i, "
         "run #%i %i events processed so far  <<<===",
         static_cast<int>(ei->eventNumber()),
         static_cast<int>(ei->runNumber()),
         static_cast<int>(entry + 1));
  }

  Info(APP_NAME, "Application finished");

  // Return gracefully
  return 0;
}
