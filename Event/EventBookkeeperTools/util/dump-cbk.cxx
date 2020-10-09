/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Setup for reading ATLAS data
#ifdef XAOD_STANDALONE
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#else
#include <POOLRootAccess/TEvent.h>
#endif
#include <xAODRootAccess/tools/TFileAccessTracer.h>

// ASG
#include <AsgMessaging/MessageCheck.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgMetadataTool.h>
#include <AthAnalysisBaseComps/AthAnalysisHelper.h>

ANA_MSG_HEADER(CutFlow)
ANA_MSG_SOURCE(CutFlow, "Dump")

// ROOT dependencies
#include <TFile.h>

// Main routine... here we go!
int main(int argc, char **argv)
{
  // Make sure things know we are not in StatusCode land
  using namespace CutFlow;
  ANA_CHECK_SET_TYPE (int);

  if (argc < 2) {
    ANA_MSG_ERROR("Input file missing.");
    return 1;
  }
  const char *inputFile = argv[1];

  // Setup for reading -- if this fails, we have major problems
#ifdef XAOD_STANDALONE
  if ( ! xAOD::Init().isSuccess() ) {
    throw std::runtime_error("Cannot initialise xAOD access !");
  }
  ANA_MSG_INFO("Using xAOD access");
#else
  IAppMgrUI *app = POOL::Init();
  ANA_MSG_INFO("Using POOL access");
#endif

  xAOD::TFileAccessTracer::enableDataSubmission(false);

  ANA_MSG_INFO("Reading file:" << inputFile);

  // Input chain
  std::unique_ptr<TFile> file(TFile::Open(inputFile, "READ"));
  ANA_CHECK(file.get());
#ifdef XAOD_STANDALONE
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
#else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
#endif
  ANA_CHECK(event.readFrom(file.get()));

  // Load metadata
  event.getEntries();

  // Retrieve the tool
  ToolHandle<asg::AsgMetadataTool> tool("BookkeeperDumperTool/BookkeeperDumperTool");
  ANA_CHECK(AAH::setProperty(tool, "StandaloneMode", true));
  ANA_CHECK(tool.retrieve());

  // Trigger finalization of all services and tools created by the Gaudi Application
#ifndef XAOD_STANDALONE
  app->finalize().ignore();
#endif

  return 0;
}
