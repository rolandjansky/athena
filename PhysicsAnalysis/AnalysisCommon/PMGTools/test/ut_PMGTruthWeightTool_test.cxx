/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Example standalone executable using TEvent (from POOL or xAODRootAccess) to read an xAOD
/// Tests the PMGTruthWeightTool, showing example usage

/// @author: James Robinson

// EDM include(s):
#include "AsgTools/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

// Project dependent include(s)
#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#endif

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"
#include "PMGTools/PMGSherpaVjetsSysTool.h"

// ROOT include(s):
#include "TFile.h"
#include <chrono>

using namespace asg::msgUserCode;  // messaging


// Check for exceptions matching known text
#define EXPECT_THROW(x,text) \
{ std::string internalTestMessage; \
  try { \
    x; internalTestMessage = std::string ("expected statement ") + #x " to throw, but it didn't"; \
  } catch (std::exception& e) { \
    if (std::string(e.what()).find(std::string(text)) == std::string::npos) { \
      internalTestMessage = std::string ("expected statement ") + #x " to throw message matching " + (text) + ", but actual message didn't match: " + e.what(); \
    } \
  } catch (...) { \
    internalTestMessage = std::string ("statement ") + #x " threw an exception that didn't derive from std::exception"; \
  } \
  if (internalTestMessage.empty()) { ANA_MSG_INFO("Success!"); \
  } else { ANA_MSG_FATAL(internalTestMessage); return -1; } \
}

// Check for functions returning false
#define EXPECT_FALSE(x) \
{ if (x == false) { ANA_MSG_INFO("Success!"); \
  } else { ANA_MSG_FATAL("Expected " << #x << " to return false"); return -1; } \
}


int main(int argc, char *argv[])
{
  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  // Initialise the application:
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  ANA_CHECK(xAOD::Init());
#else
  IAppMgrUI *app = POOL::Init();
#endif

  // Use a default MC file for testing if none is provided
  TString fileName = "$ASG_TEST_FILE_MC";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using ASG_TEST_FILE_MC");
  } else {
    fileName = argv[1]; // use the user provided file
  }

  // Initialise TEvent reading
#ifdef XAOD_STANDALONE
  std::unique_ptr<TFile> ptrFile(TFile::Open(fileName, "READ"));
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  ANA_CHECK(event.readFrom(ptrFile.get()));
#else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
  event.readFrom(fileName);
#endif
  ANA_MSG_INFO("Opened file: " << fileName);

  // Create the truth weight tool:
  ANA_MSG_INFO("Creating PMGTruthWeightTool...");
#ifdef XAOD_STANDALONE
  ANA_MSG_INFO("Creating PMGTruthWeightTool...");
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > weightTool;
  ASG_SET_ANA_TOOL_TYPE(weightTool, PMGTools::PMGTruthWeightTool);
  weightTool.setName("PMGTruthWeightTool");
  ANA_CHECK(weightTool.initialize());
#else
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > weightTool("PMGTools::PMGTruthWeightTool/PMGTruthWeightTool");
  ANA_CHECK(weightTool.retrieve());
#endif

  // Loop over a few events:
  ANA_MSG_INFO("Preparing to loop over events...");
  const Long64_t nEntries = 5;
  // double retrievalTimeNanoSeconds = 0;
  for(Long64_t entry = 0; entry < nEntries; ++entry) {
    if (event.getEntry(entry) < 0) { ANA_MSG_ERROR("Failed to read event " << entry); continue; }

    auto weightNames = weightTool->getWeightNames();
    ANA_MSG_INFO("Event #" << entry << ": found " << weightNames.size() << " weights for this event");

    // Print out the first weight for every event
    if (weightNames.size() > 1) {
      static std::string weight_name_to_test = weightNames.at(1);
      ANA_MSG_INFO("Event #" << entry << ": weight called '" << weight_name_to_test << "' = " << weightTool->getWeight(weight_name_to_test));
    }

    // Full print out for some of the events
    if ((entry + 1) % 5 == 0) {
      // Print out all weights and names
      ANA_MSG_INFO("Printing all " << weightNames.size() << " weights for this event...");
      unsigned int idx(0);
      for (auto weight : weightNames) {
        ANA_MSG_INFO("... weight " << idx++ << " has name \"" << weight << "\" and value " << weightTool->getWeight(weight));
      }

      // Give feedback about where we are
      ANA_MSG_INFO("Processed " << entry + 1 << " / " << nEntries << " events");
    }

    // Check retrieval speed
    ANA_MSG_INFO("Check average time taken to retrieve a weight");
    if (entry == nEntries - 1) {
      int nWeightCalls = 1000000;
      static std::string weight_name_to_test = weightNames.back();
      auto start = std::chrono::steady_clock::now();
      for (int i = 0; i < nWeightCalls; ++i) {
        weightTool->getWeight(weight_name_to_test);
      }
      auto elapsed = std::chrono::steady_clock::now() - start;
      double retrievalTimeNanoSeconds = std::chrono::duration <double, std::nano> (elapsed).count() / nWeightCalls;
      ANA_MSG_INFO("Retrieving " << nWeightCalls << " weights took " << retrievalTimeNanoSeconds << " ns per call");
    }

    // Check that retrieving a non-existent weight throws an exception
    ANA_MSG_INFO("Check that retrieving a non-existent weight throws an exception");
    EXPECT_THROW(weightTool->getWeight("non-existent-name"), "Weight \"non-existent-name\" could not be found");

    // Check that asking for a non-existent weight returns false
    ANA_MSG_INFO("Check that asking for a non-existent weight returns false");
    EXPECT_FALSE(weightTool->hasWeight("non-existent-name"));
  }

  // trigger finalization of all services and tools created by the Gaudi Application
#ifndef XAOD_STANDALONE
  app->finalize();
#endif

  return 0;
}
