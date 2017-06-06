/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"

// bits for I/O performance monitoring
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/IOStats.h"

#include "TopAnalysis/Tools.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/ConfigurationSettings.h"

#include "TopExamples/MakeTopConfig.h"
#include "TopExamples/MiniToFlatAnalysis.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Please provide a file containing input files!!!" << std::endl;
  }

  // magic xAOD stuff
  // true = fail at even the slightest xaod issue
  top::xAODInit(true);
  StatusCode::enableFailure();

  std::cout << "Configuration Files:\n";
  std::string settingsFilename = std::string(argv[1]);
  std::cout << "    " << settingsFilename << "\n";
  std::cout << "    " << std::string(argv[2]) << "\n\n";

  // load the settings from the input file
  auto* const settings = top::ConfigurationSettings::get();
  settings->loadFromFile(settingsFilename);
  // std::cout << "Configuration:\n";
  // std::cout << *settings << "\n";

  // open the files (to check they exist) and count how many events we have
  std::vector<std::string> filenames = top::fileList(std::string(argv[2]));
  if (filenames.size() == 0) {
    std::cout << "Please supply an input file in the filelist" << std::endl;
    exit(1);
  }

  // Create a TEvent object:
  xAOD::TEvent xaodevent(xAOD::TEvent::kBranchAccess);
  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // Make a Top::TopConfig from the input file metadata
  top::MakeTopConfig mkConfig("top::MakeTopConfig");
  std::unique_ptr<TFile> firstInputFile(TFile::Open(filenames[0].c_str()));
  top::check(xaodevent.readFrom(firstInputFile.get()),
             "xAOD::TEvent readFrom failed");
  std::shared_ptr<top::TopConfig> topConfig = mkConfig.makeTopConfig();
  topConfig->setConfigSettings(settings);
  // Analysis
  top::MiniToFlatAnalysis analysis("top::MiniToFlatAnalysis");
  top::check(analysis.setProperty("config", topConfig),
             "Failed to set property");
  top::check(analysis.initialize() , "Failed to initialize");

  xAOD::PerfStats::instance().start();

  // Loop over files, and events in files
  for (const auto& filename : filenames) {
    std::cout << "Opening " << filename << std::endl;
    std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));
    top::check(xaodevent.readFrom(inputFile.get()),
               "xAOD::TEvent readFrom failed");
    Long64_t entries = xaodevent.getEntries();
    for (Long64_t entry=0; entry < entries; ++entry) {
      // clear the TStore - get rid of the last events objects
      xAOD::TActiveStore::store()->clear();
      // increment the event
      xaodevent.getEntry(entry);
      top::check(analysis.execute() , "Failed to execute event");
    }
  }

  top::check(analysis.finalize() , "Failed to finalize");

  xAOD::PerfStats::instance().stop();
  // xAOD::IOStats::instance().stats().Print("Summary");
  // xAOD::IOStats::instance().stats().Print();

  return 0;
}
