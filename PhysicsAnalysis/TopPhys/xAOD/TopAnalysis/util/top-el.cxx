/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
#include "TFile.h"
#include "TTree.h"

#include "AnaAlgorithm/AnaAlgorithmConfig.h"
#include "AsgMessaging/MessageCheck.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/Job.h"
#include "EventLoop/OutputStream.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/SampleLocal.h"

#include "TopAnalysis/Tools.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// to disable the sending of file access statistics
#include "xAODEventInfo/EventInfo.h"
#include "xAODRootAccess/tools/TFileAccessTracer.h"

// Path resolver
#include "PathResolver/PathResolver.h"

#include "TopAnalysis/MsgCategory.h"
// use ATH_MSG macros defined in the namespace TopAnalysis
using namespace TopAnalysis;

int main(int argc, char **argv) {

  // set appropriate msg level
  setMsgLevel(MSG::Level::INFO);

  if (argc != 4) {
    ATH_MSG_INFO("Code to perform object and event selection and write-out\n"
                 << "a few histograms and / or a tree in xAOD format.\n"
                 << "Use like:\n"
                 << "    " << argv[0] << " cuts.txt input.txt\n"
                 << "    cuts.txt   - file containing cuts\n"
                 << "    input.txt  - file containing list of input files\n"
                 << "    submitdir  - path for EventLoop submit directory\n"
                 << "\n"
                 << "For example\n"
                 << "    " << argv[0]
                 << " $ROOTCOREBIN/data/TopAnalysis/nocuts.txt "
                    "$ROOTCOREBIN/data/TopAnalysis/input-13TeV-fondueworld.txt output\n");
    return 1;
  }

  // to disable the sending of file access statistics
  xAOD::TFileAccessTracer::enableDataSubmission(false);

  // magic xAOD stuff
  // true = fail at even the slightest xaod issue
  top::xAODInit(true);
  StatusCode::enableFailure();
  ANA_CHECK_SET_TYPE(int);

  EL::AnaAlgorithmConfig algConfig("top::TopxAODAlg/TopxAODAlg");

  ATH_MSG_INFO("INPUT: Configuration file (argv[1]) = " << argv[1] << "\n");
  std::string settingsFilename =
      PathResolver::find_file(argv[1], "DATAPATH", PathResolver::LocalSearch);

  ATH_MSG_INFO("LOCATED (using PathResolverFindFile ): Configuration file = " << settingsFilename
                                                                              << "\n");
  ANA_CHECK(algConfig.setProperty("ConfigPath", settingsFilename));

  ATH_MSG_INFO("Configuration Files:\n" << settingsFilename << "\n" << argv[2] << "\n");

  // load the settings from the input file
  top::ConfigurationSettings *settings = top::ConfigurationSettings::get();
  settings->loadFromFile(settingsFilename);
  ATH_MSG_INFO("Configuration:\n" << *settings << "\n");

  // only after printing full configuration check if there are issues and inform
  // user and crash if necessary
  settings->checkSettings();

  const std::string libraryNames = settings->value("LibraryNames");
  top::loadLibraries(libraryNames);
  ANA_CHECK(algConfig.setProperty("LibraryNames", libraryNames));

  // A configuration that John can easily understand
  // This is not the same as a good configuration
  std::shared_ptr<top::TopConfig> topConfig(new top::TopConfig());

  // open the files (to check they exist) and count how many events we have
  std::vector<std::string> filenames = top::fileList(std::string(argv[2]));

  ////////////////////////////////////////////////////////////
  // METADATA READING (SOMETHING THAT SHOULD NOT BE DONE HERE)
  ////////////////////////////////////////////////////////////

  // picking the first file was a bad idea because in the derivations it often
  // has no events (no CollectionTree).  Be sure to pick a file with events in
  // it -- this is needed to determine the year to setup triggers
  // FIXME -- can we have this in MetaData without having to open CollectionTree
  // ?
  {
    std::string usethisfile = filenames[0];
    for (const auto &filename : filenames) {
      std::unique_ptr<TFile> checkingYieldFile(TFile::Open(filename.c_str()));

      // collection tree means > 0 events
      const TTree *const collectionTree =
          dynamic_cast<TTree *>(checkingYieldFile->Get("CollectionTree"));
      if (collectionTree) {
        usethisfile = filename;
        break;
      }
    }

    // read FileMetaData using AsgMetadataTool
    std::unique_ptr<TFile> testFile(TFile::Open(usethisfile.c_str()));
    if (!top::readMetaData(testFile.get(), topConfig)) {
      ATH_MSG_ERROR("Unable to access FileMetaData in this file : "
                    << usethisfile << "\nPlease report this message.");
      return 1;
    }

    // Pass the settings file to the TopConfig
    topConfig->setConfigSettings(settings);

    if (topConfig->isMC() && !topConfig->isTruthDxAOD()) {
      // now need to get and set the parton shower generator from TopDataPrep
      SampleXsection tdp;
      // Package/filename - XS file we want to use (can now be configured via
      // cutfile)
      const std::string tdp_filename = settings->value("TDPPath");
      // Use the path resolver to find the first file in the list of possible
      // paths ($CALIBPATH)
      const std::string fullpath = PathResolverFindCalibFile(tdp_filename);
      if (!tdp.readFromFile(fullpath.c_str())) {
        ATH_MSG_ERROR("TopDataPreparation - could not read file\n" << tdp_filename);
        return 1;
      }
      ATH_MSG_INFO("SampleXsection::Found " << fullpath);

      tdp.setTranslator(topConfig->GetMCMCTranslator());

      int ShowerIndex = tdp.getShoweringIndex(topConfig->getDSID());
      ATH_MSG_INFO("DSID: " << topConfig->getDSID() << "\t"
                            << "ShowerIndex: " << ShowerIndex
                            << " PS generator: " << tdp.getShoweringString(topConfig->getDSID()));
      topConfig->setMapIndex(ShowerIndex);
      topConfig->setShoweringAlgorithm(tdp.getShowering(topConfig->getDSID()));
      ANA_CHECK(algConfig.setProperty("FTAGMapIndex", ShowerIndex));
      ANA_CHECK(algConfig.setProperty("ShoweringAlgorithm", topConfig->getShoweringAlgorithm()));
    }
    // check year
    // TODO Replace this with different functionality, that does not need opening a file
    {
      xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
      top::check(xaodEvent.readFrom(testFile.get()), "Failed to read file in");
      const unsigned int entries = xaodEvent.getEntries();
      if (entries > 0) {
        xaodEvent.getEntry(0);
        const xAOD::EventInfo *eventInfo(nullptr);
        top::check(xaodEvent.retrieve(eventInfo, topConfig->sgKeyEventInfo()),
                   "Failed to retrieve EventInfo");
        const unsigned int runnumber = eventInfo->runNumber();
        const std::string thisYear = topConfig->getYear(runnumber, topConfig->isMC());
        topConfig->SetYear(thisYear);
      } else {
        topConfig->SetYear("UNKNOWN");
      }
      topConfig->SetTriggersToYear(topConfig->isMC());

      ANA_CHECK(algConfig.setProperty("DataTakingYear", topConfig->getYear()));
    }

  } // close and delete the ptr to testFile

  EL::Job job;

  SH::SampleHandler sh;
  auto sample = std::make_unique<SH::SampleLocal>("sample");
  for (const auto &file : filenames)
    sample->add(file);
  sh.add(sample.release());
  job.sampleHandler(sh);
  job.options()->setDouble(EL::Job::optMaxEvents, topConfig->numberOfEventsToRun());
  job.options()->setDouble(EL::Job::optSkipEvents, topConfig->numberOfEventsToSkip());
  job.options()->setString(EL::Job::optSubmitDirMode, "unique-link");

  EL::OutputStream output("output");
  job.outputAdd(output);
  ANA_CHECK(algConfig.setProperty("StreamName", "output"));

  job.algsAdd(algConfig);

  EL::DirectDriver driver;
  driver.submit(job, argv[3]);

  return 0;
}
