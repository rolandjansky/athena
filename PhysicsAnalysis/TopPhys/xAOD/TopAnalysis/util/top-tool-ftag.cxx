/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TSystem.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"

#include "TopConfiguration/AodMetaDataAccess.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/SelectionConfigurationData.h"

#include "TopAnalysis/Tools.h"
#include "TopCPTools/TopToolStore.h"

#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/SystematicEvent.h"
#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/EventCleaningSelection.h"

#include "TopCorrections/BTagScaleFactorCalculator.h"

// to disable the sending of file access statistics
#include "xAODRootAccess/tools/TFileAccessTracer.h"

/**
 * @file The main executable.
 *
 * @brief Entry point code to perform object and event selection and write-out a
 * few histograms and / or a tree in xAOD format.
 *
 * @return 0 If everything is okay.
 */

// Prototype functions
std::shared_ptr<top::TopConfig> InitialiseTopSoftware(int, char**);
int TestFTag(std::shared_ptr<top::TopConfig>);

std::shared_ptr<top::TopConfig> InitialiseTopSoftware(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Code to read an input file and return information on the";
    std::cout << "anticipated systematics list in the b-tagging calibration";
    std::cout << "Use like:\n";
    std::cout << "    " << argv[0] << " cuts.txt input.txt\n";
    std::cout << "    cuts.txt   - file containing TopConfig\n";
    std::cout << "    input.txt  - file containing list of input files\n";
    std::cout << "\n";
    std::cout << "For example\n";
    std::cout << "    " << argv[0] <<
    " $ROOTCOREBIN/data/TopAnalysis/nocuts.txt $ROOTCOREBIN/data/TopAnalysis/input-13TeV-fondueworld.txt\n";
    return 0;
  }

  // to disable the sending of file access statistics
  xAOD::TFileAccessTracer::enableDataSubmission(false);

  //magic xAOD stuff
  //true = fail at even the slightest xaod issue
  top::xAODInit(true);
  StatusCode::enableFailure();

  xAOD::TStore store;

  std::cout << "Configuration Files:\n";
  std::string settingsFilename = std::string(argv[1]);
  std::cout << "    " << settingsFilename << "\n";
  std::cout << "    " << std::string(argv[2]) << "\n\n";

  //load the settings from the input file
  auto* const settings = top::ConfigurationSettings::get();
  settings->loadFromFile(settingsFilename);
  std::cout << "Configuration:\n";
  std::cout << *settings << "\n";

  const std::string libraryNames = settings->value("LibraryNames");
  top::loadLibraries(libraryNames);

  //load the event selection from the input file
  std::string cutsFilename = std::string(settingsFilename);

  //open the files (to check they exist) and count how many events we have
  std::vector<std::string> filenames = top::fileList(std::string(argv[2]));

  // A configuration that John can easily understand
  // This is not the same as a good configuration
  std::shared_ptr<top::TopConfig> topConfig(new top::TopConfig());

  topConfig->setMapIndex(410501);
  topConfig->setConfigSettings(settings);
  xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
  // Setup all asg::AsgTools
  top::TopToolStore topTools("top::TopToolStore");
  top::check(topTools.setProperty("config", topConfig),
             "Failed to setProperty of topTools");
  top::check(topTools.initialize(), "Failed to initialize topTools");
  topConfig->fixConfiguration();
  std::cout << *topConfig << "\n";

  return topConfig;
}

int TestFTag(std::shared_ptr<top::TopConfig> topConfig) {
  // Here we need to access the flavour tagging CP tool
  // and get it to tell us what systematics are available
  // given the configuration we setup up

  // Get TopCPTool handle
  top::BTagScaleFactorCalculator* m_btagSF = new top::BTagScaleFactorCalculator("top::BTagScaleFactorCalculator");
  top::check(m_btagSF->setProperty("config", topConfig), "Failed to pass in the config");
  top::check(m_btagSF->initialize(), "Failed to init");
  top::check(m_btagSF->debug(), "Failed to debug");
  return 0;
}

int main(int argc, char** argv) {
  std::shared_ptr<top::TopConfig> topConfig = InitialiseTopSoftware(argc, argv);
  TestFTag(topConfig);
}
