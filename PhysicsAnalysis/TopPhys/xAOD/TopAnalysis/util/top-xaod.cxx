/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include <iostream>
#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TSystem.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"

// bits for I/O performance monitoring
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/IOStats.h"

// CutBookKeeping
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "TopCorrections/ScaleFactorCalculator.h"
#include "TopCorrections/PDFScaleFactorCalculator.h"

#include "FakeBkgTools/AsymptMatrixTool.h"

#include "TopSystematicObjectMaker/ObjectCollectionMaker.h"

#include "TopConfiguration/AodMetaDataAccess.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/SelectionConfigurationData.h"
#include "TopConfiguration/Tokenize.h"

#include "TopAnalysis/AnalysisTrackingHelper.h"
#include "TopAnalysis/EventSelectionManager.h"
#include "TopAnalysis/Tools.h"
#include "TopCPTools/TopToolStore.h"
#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/SystematicEvent.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/EventCleaningSelection.h"

#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopPartons/CalcTtbarLightPartonHistory.h"
#include "TopPartons/CalcTbbarPartonHistory.h"
#include "TopPartons/CalcWtbPartonHistory.h"
#include "TopPartons/CalcTTZPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/CalcTtbarGammaPartonHistory.h"
#include "TopPartons/CalcThqPartonHistory.h"
#include "TopPartons/CalcTzqPartonHistory.h"

#include "TopParticleLevel/ParticleLevelLoader.h"

#include "TopDataPreparation/SampleXsection.h"

#include "TopHLUpgrade/UpgradeObjectLoader.h"

// to disable the sending of file access statistics
#include "xAODRootAccess/tools/TFileAccessTracer.h"

// Path resolver
#include "PathResolver/PathResolver.h"

#include "TopAnalysis/MsgCategory.h"
// use ATH_MSG macros defined in the namespace TopAnalysis
using namespace TopAnalysis;

/**
 * @file The main executable.
 *
 * @brief Entry point code to perform object and event selection and write-out a
 * few histograms and / or a tree in xAOD format.
 *
 * @return 0 If everything is okay.
 */
int main(int argc, char** argv) {
  if (argc != 3) {
    ATH_MSG_INFO("Code to perform object and event selection and write-out\n"
        << "a few histograms and / or a tree in xAOD format.\n"
        << "Use like:\n"
        << "    " << argv[0] << " cuts.txt input.txt\n"
        << "    cuts.txt   - file containing cuts\n"
        << "    input.txt  - file containing list of input files\n"
        << "\n"
        << "For example\n"
        << "    " << argv[0]
        << " $ROOTCOREBIN/data/TopAnalysis/nocuts.txt $ROOTCOREBIN/data/TopAnalysis/input-13TeV-fondueworld.txt\n");
    return 1;
  }

  // to disable the sending of file access statistics
  xAOD::TFileAccessTracer::enableDataSubmission(false);

  //magic xAOD stuff
  //true = fail at even the slightest xaod issue
  top::xAODInit(true);
  StatusCode::enableFailure();

  xAOD::TStore store;

  std::string settingsFilename = std::string(argv[1]);
  ATH_MSG_INFO("Configuration Files:\n"
      << settingsFilename << "\n"
      << std::string(argv[2]) << "\n");

  //load the settings from the input file
  auto* const settings = top::ConfigurationSettings::get();
  settings->loadFromFile(settingsFilename);
  ATH_MSG_INFO("Configuration:\n" << *settings << "\n");

  const std::string libraryNames = settings->value("LibraryNames");
  top::loadLibraries(libraryNames);

  // Analysis tracking
  std::unique_ptr<top::AnalysisTrackingHelper> tracker;
  {
    bool useTracking = true;
    settings->retrieve("WriteTrackingData", useTracking);
    if (useTracking) {
      tracker.reset(new top::AnalysisTrackingHelper());
    }
  }

  // I/O Performance stats?
  // Summary gives a summary
  // Full gives detailed info on each collection in the file
  unsigned int doPerfStats(0);
  if (settings->value("PerfStats") == "Summary") doPerfStats = 1;

  if (settings->value("PerfStats") == "Full") doPerfStats = 2;

  //load the event selection from the input file
  std::string cutsFilename = std::string(settingsFilename);

  //open the files (to check they exist) and count how many events we have
  std::vector<std::string> filenames = top::fileList(std::string(argv[2]));
  size_t totalYield = top::checkFiles(filenames);

  //open output file
  std::unique_ptr<TFile> outputFile(TFile::Open((settings->value("OutputFilename") + ".tmp").c_str(), "RECREATE"));

  // A configuration that John can easily understand
  // This is not the same as a good configuration
  std::shared_ptr<top::TopConfig> topConfig(new top::TopConfig());

  // load AOD meta-data from all files
  bool useAodMetaData = false;
  settings->retrieve("UseAodMetaData", useAodMetaData);
  if (useAodMetaData) {
    ATH_MSG_INFO("Loading meta-data from input files ... ");
    topConfig->aodMetaData().loadWithFilesFrom(argv[2]);
    ATH_MSG_INFO("OK.");
  }

  
  //picking the first file was a bad idea because in the derivations it often
  //has no events (no CollectionTree).  Be sure to pick a file with events in
  //it...
  // The isMC flag needs to be taken from the xAOD metadata
  // We also need something for AFII
  {
    std::string usethisfile = filenames[0];
    bool atLeastOneFileIsValid(false);
    for (const auto& filename : filenames) {
      std::unique_ptr<TFile> checkingYieldFile(TFile::Open(filename.c_str()));

      //collection tree means > 0 events
      const TTree* const collectionTree = dynamic_cast<TTree* > (checkingYieldFile->Get("CollectionTree"));
      if (collectionTree) {
        usethisfile = filename;
        atLeastOneFileIsValid = true;
        break;
      }
    }

    // If there are no valid files, let's exit cleanly
    if (!(atLeastOneFileIsValid || useAodMetaData)) {
      ATH_MSG_ERROR("No input file contains a CollectionTree and no metadata used. Cannot Continue.");
      ATH_MSG_ERROR("If you had turned on 'UseAodMetaData' in your configuration, "
          "we could overcome this limitation.");
      return 1;
    }

    std::unique_ptr<TFile> testFile(TFile::Open(usethisfile.c_str()));
    if (!top::readMetaData(testFile.get(), topConfig)) {
      ATH_MSG_ERROR("Unable to access FileMetaData and/or TruthMetaData in this file : " << usethisfile
          << "\nPlease report this message.");
    }


    const bool isOverlay = useAodMetaData ? topConfig->aodMetaData().IsEventOverlayInputSim() : false;
    bool isMC(true);
    if (!isOverlay) {
      isMC = (useAodMetaData ?
              topConfig->aodMetaData().isSimulation() :
              top::isFileSimulation(testFile.get(), topConfig->sgKeyEventInfo())
              );
    }

    topConfig->setIsMC(isMC);

    const bool isPrimaryxAOD = top::isFilePrimaryxAOD(testFile.get());
    topConfig->setIsPrimaryxAOD(isPrimaryxAOD);

    const std::string derivationStream = top::getDerivationStream(testFile.get());
    ATH_MSG_INFO("Derivation stream is -> " << derivationStream);
    topConfig->setDerivationStream(derivationStream);

    // first we need to read some metadata before we read the config
    if (isMC) {
      // check number of MC generator weights -- we need this before start initializing PMGTruthWeightTool later
      if (atLeastOneFileIsValid) topConfig->setMCweightsVectorSize(top::MCweightsSize(testFile.get(), topConfig->sgKeyEventInfo()));
      ///-- Are we using a truth derivation (no reco information)? --///
      ///-- Let's find out in the first event, this could be done better --///
      bool isTruthDxAOD = top::isTruthDxAOD(testFile.get());
      topConfig->setIsTruthDxAOD(isTruthDxAOD);

      unsigned int DSID {0};
      if (!useAodMetaData) {
        if (atLeastOneFileIsValid) {
          DSID = top::getDSID(testFile.get(), topConfig->sgKeyEventInfo());
          topConfig->setDSID(DSID);
        } else {
          ATH_MSG_ERROR("We could not determine DSID for this sample from either CollectionTree, or FileMetaData, or TruthMetaData. There is something seriously wrong with this sample.");
          return 1;
        }
      } else {
        DSID = topConfig->getDSID();
      }
    }



    // Pass the settings file to the TopConfig
    topConfig->setConfigSettings(settings);

    if (isMC && !topConfig->isTruthDxAOD()) {
      // now need to get and set the parton shower generator from TopDataPrep
      SampleXsection tdp;
      // Package/filename - XS file we want to use (can now be configured via cutfile)
      const std::string tdp_filename = settings->value("TDPPath");
      // Use the path resolver to find the first file in the list of possible paths ($CALIBPATH)
      const std::string fullpath = PathResolverFindCalibFile(tdp_filename);
      if (!tdp.readFromFile(fullpath.c_str())) {
        ATH_MSG_ERROR("TopDataPreparation - could not read file\n" << tdp_filename);
        return 1;
      }
      ATH_MSG_INFO("SampleXsection::Found " << fullpath);

      tdp.setTranslator(topConfig->GetMCMCTranslator());

      int ShowerIndex = tdp.getShoweringIndex(topConfig->getDSID());
      ATH_MSG_INFO("DSID: " << topConfig->getDSID() << "\t" << "ShowerIndex: " << ShowerIndex << " PS generator: "<< tdp.getShoweringString(topConfig->getDSID()));
      topConfig->setMapIndex(ShowerIndex);
      topConfig->setShoweringAlgorithm(tdp.getShowering(topConfig->getDSID()));
    }
    // check year
    {
      xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);
      top::check(xaodEvent.readFrom(testFile.get()), "Failed to read file in");
      const unsigned int entries = xaodEvent.getEntries();
      if (entries > 0) {
        xaodEvent.getEntry(0);
        const xAOD::EventInfo* eventInfo(nullptr);
        top::check(xaodEvent.retrieve(eventInfo, topConfig->sgKeyEventInfo()), "Failed to retrieve EventInfo");
        const unsigned int runnumber = eventInfo->runNumber();
        const std::string thisYear = topConfig->getYear(runnumber, isMC);
        topConfig->SetYear(thisYear);
      } else {
        topConfig->SetYear("UNKNOWN");
      }
      topConfig->SetTriggersToYear(isMC);
    }

  } //close and delete the ptr to testFile


  //In rel 19 we had a function to guess Class or Branch Access.
  //In rel20 just use branch (quicker)
  // In rel 20.7, need to go back to class access
  xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);

  // Read metadata
  std::unique_ptr<TFile> metadataInitFile(TFile::Open(filenames[0].c_str()));
  top::check(xaodEvent.readFrom(metadataInitFile.get()), "xAOD::TEvent readFrom failed");


  // Setup all asg::AsgTools
  top::TopToolStore topTools("top::TopToolStore");
  top::check(topTools.setProperty("config", topConfig),
             "Failed to setProperty of topTools");
  top::check(topTools.initialize(), "Failed to initialize topTools");
  // EventCleaningSelection
  // Decorates EventInfo with GRL decision (always true for MC)
  // Evaluates all triggers requested from all selections
  // Trigger matching
  // Check for good Tile and LAr calorimeters
  // Let's check this passes before running expensive calibrations
  // If ANY selection does not request a trigger, we won't veto any events

  std::unique_ptr<top::EventCleaningSelection> eventCleaning(new top::EventCleaningSelection(
                                                               "top::EventCleaningSelection"));
  top::check(eventCleaning->setProperty("config", topConfig),
             "Failed to setProperty of triggerGRLSelection");
  top::check(eventCleaning->initialize(),
             "Failed to initialize triggerGRLSelection");
  eventCleaning->setEventSelections(settings->selections());


  // Systematic object collection making
  std::unique_ptr<top::ObjectCollectionMaker> systObjMaker(new top::ObjectCollectionMaker("top::ObjectCollectionMaker"));
  top::check(systObjMaker->setProperty("config", topConfig), "Failed to setProperty of systObjMaker");
  if (!topConfig->isTruthDxAOD()) top::check(systObjMaker->initialize(), "Failed to initialize systObjMaker");

  //setup object definitions - not used in HLUpgrade tools
  std::unique_ptr<top::TopObjectSelection> objectSelection;
  if (!topConfig->HLLHC()) {
    objectSelection.reset(top::loadObjectSelection(topConfig));
    objectSelection->print(msg(MSG::Level::INFO)); // forward to msg stream using INFO level
  }

  //setup event-level cuts
  top::EventSelectionManager eventSelectionManager(settings->selections(), outputFile.get(), libraryNames, topConfig);

  //The loader tool for top::ParticleLevelEvent objects
  top::ParticleLevelLoader particleLevelLoader(topConfig);

  // The loader tool for Upgrade objects
  top::UpgradeObjectLoader upgradeLoader(topConfig);

  // Fix the configuration - it now knows about:
  //     * all objects collections to work with
  //     * all systematic variations
  topConfig->fixConfiguration();


  // OK let's printout the TopConfig
  ATH_MSG_INFO(*topConfig << "\n");
  if (tracker) tracker->setTopConfig(topConfig);

  //Top parton history for MC events
  // This is quite ugly and simple, it will be harmonized with in the future
  // along with all other factory methods (it's not a factory method right now)
  std::unique_ptr<top::CalcTopPartonHistory> topPartonHistory(nullptr);
  if (settings->value("TopPartonHistory") == "ttbar") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcTtbarPartonHistory("top::CalcTtbarPartonHistory"));
    top::check(topPartonHistory->setProperty("config",
                                             topConfig), "Failed to setProperty of top::CalcTtbarPartonHistory");
  } else if (settings->value("TopPartonHistory") == "ttbarlight") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcTtbarLightPartonHistory(
                                                    "top::CalcTtbarLightPartonHistory"));
    top::check(topPartonHistory->setProperty("config",
                                             topConfig), "Failed to setProperty of top::CalcTtbarLightPartonHistory");
  } else if (settings->value("TopPartonHistory") == "tb") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcTbbarPartonHistory("top::CalcTbbarPartonHistory"));
    top::check(topPartonHistory->setProperty("config",
                                             topConfig), "Failed to setProperty of top::CalcTbbarPartonHistory");
  } else if (settings->value("TopPartonHistory") == "Wtb") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcWtbPartonHistory("top::CalcWtbPartonHistory"));
    top::check(topPartonHistory->setProperty("config", topConfig),
               "Failed to setProperty of top::CalcWtbPartonHistory");
  } else if (settings->value("TopPartonHistory") == "ttz") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcTTZPartonHistory("top::CalcTTZPartonHistory"));
    top::check(topPartonHistory->setProperty("config", topConfig),
               "Failed to setProperty of top::CalcTTZPartonHistory");
  } else if (settings->value("TopPartonHistory") == "ttgamma") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory> (new top::CalcTtbarGammaPartonHistory(
                                                    "top::CalcTtbarGammaPartonHistory"));
    top::check(topPartonHistory->setProperty("config",
                                             topConfig), "Failed to setProperty of top::CalcTtbarGammaPartonHistory");
  } else if (settings->value("TopPartonHistory") == "tHq") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory>(new top::CalcThqPartonHistory("top::CalcThqPartonHistory"));
    top::check(topPartonHistory->setProperty("config", topConfig),
               "Failed to setProperty of top::CalcThqPartonHistory");
  } else if (settings->value("TopPartonHistory") == "tZq") {
    topPartonHistory =
      std::unique_ptr<top::CalcTopPartonHistory>(new top::CalcTzqPartonHistory("top::CalcTzqPartonHistory"));
    top::check(topPartonHistory->setProperty("config", topConfig),
               "Failed to setProperty of top::CalcTzqPartonHistory");
  }


  //LHAPDF SF calculation
  std::unique_ptr<top::PDFScaleFactorCalculator> PDF_SF(nullptr);
  PDF_SF =
    std::unique_ptr<top::PDFScaleFactorCalculator> (new top::PDFScaleFactorCalculator("top::PDFScaleFactorCalculator"));
  if (topConfig->doLHAPDF()) {
    top::check(PDF_SF->setProperty("config", topConfig), "Failed to set config for PDF SF Calculator");
    top::check(PDF_SF->initialize(), "Failed to initialize PDF SF calculator");
  }


  // make top::Event objects
  std::unique_ptr<top::TopEventMaker> topEventMaker(new top::TopEventMaker("top::TopEventMaker"));
  top::check(topEventMaker->setProperty("config", topConfig), "Failed to setProperty of top::TopEventMaker");
  top::check(topEventMaker->initialize(), "Failed to initialize top::TopEventMaker");
  // Debug messages?
  // topEventMaker.msg().setLevel(MSG::DEBUG);

  std::unique_ptr<top::ScaleFactorCalculator> topScaleFactors(new top::ScaleFactorCalculator(
                                                                "top::ScaleFactorCalculator"));
  top::check(topScaleFactors->setProperty("config", topConfig), "Failed to setProperty of top::ScaleFactorCalculator");
  top::check(topScaleFactors->initialize(), "Failed to initialize  top::ScaleFactorCalculator");

  std::vector<std::unique_ptr<CP::AsymptMatrixTool> > topfakesMMWeightsIFF;
  std::vector<std::vector<std::string> > FakesMMConfigIFF;
  if (!topConfig->isMC() && topConfig->doLooseEvents() && topConfig->doFakesMMWeightsIFF()) {
    if (topConfig->FakesMMConfigIFF() != "") {
      std::vector<std::string> tokens;
      top::tokenize(topConfig->FakesMMConfigIFF(), tokens, ";");
      std::reverse(tokens.begin(), tokens.end());
      while (tokens.size()) {
        const auto& token = tokens.back();
        std::vector<std::string> tokens2;
        top::tokenize(token, tokens2, ":");
        top::check(tokens2.size() == 3,
                   "Failed to read FakesMMConfigIFF: " + topConfig->FakesMMConfigIFF() + " has size " +
                   std::to_string(tokens2.size()) + " (should be 3)");
        FakesMMConfigIFF.push_back(tokens2);
        FakesMMConfigIFF.back()[0] = PathResolverFindCalibFile(tokens2[0]);
        tokens.pop_back();
      }
    }
    for (unsigned int mmi = 0; mmi < FakesMMConfigIFF.size(); ++mmi) {
      topfakesMMWeightsIFF.emplace_back(std::make_unique<CP::AsymptMatrixTool>("AsymptMatrixTool_" + std::to_string (mmi)));
      top::check(topfakesMMWeightsIFF.back()->setProperty("InputFiles",
                                                          std::vector<std::string>{FakesMMConfigIFF[mmi][0]}),
                 "Failed To setProperty InputFiles of AsymptMatrixTool");
      top::check(topfakesMMWeightsIFF.back()->setProperty("Selection",
                                                          FakesMMConfigIFF[mmi][1]),
                 "Failed to set the selection FakesMMIFFConfigs for selection " + FakesMMConfigIFF[mmi][1]);
      top::check(topfakesMMWeightsIFF.back()->setProperty("Process",
                                                          FakesMMConfigIFF[mmi][2]),
                 "Failed to set the selection FakesMMIFFConfigs for process " + FakesMMConfigIFF[mmi][2]);
      top::check(topfakesMMWeightsIFF.back()->setProperty("EnergyUnit",
                                                          "GeV"),
                 "Failed to setProperty EnergyUnit of AsymptMatrixTool");
      top::check(topfakesMMWeightsIFF.back()->setProperty("ConvertWhenMissing",
                                                          true),
                 "Failed to setProperty ConvertWhenMissing of AsymptMatrixTool");
      top::check(topfakesMMWeightsIFF.back()->setProperty("TightDecoration",
                                                          "passPreORSelection,as_char"),
                 "Failed to setProperty TightDecoration of AsymptMatrixTool");
      if (topConfig->FakesMMIFFDebug()) top::check(topfakesMMWeightsIFF.back()->setProperty("OutputLevel",
                                                                                            MSG::INFO),
                                                   "Failed to setProperty of AsymptMatrixTool");
      top::check(topfakesMMWeightsIFF.back()->initialize(), "Failed to initialize  AsymptMatrixTool");
    }
  }

  //A list of extra branches that we create in this code and want to save
  std::vector<std::string> extraBranches;
  eventSelectionManager.addExtraBranches(extraBranches);

  //output file format (and any user-modified code)
  std::unique_ptr<top::EventSaverBase> eventSaver(top::loadEventSaver(topConfig));
  eventSaver->initialize(topConfig, outputFile.get(), extraBranches);

  // save sum of weights before derivations
  outputFile->cd();
  TTree* sumWeights = new TTree("sumWeights", "");
  float totalEventsWeighted = 0;
  double totalEventsWeighted_temp = 0; 
  std::vector<float> totalEventsWeighted_LHE3;
  std::vector<double> totalEventsWeighted_LHE3_temp;// having doubles is necessary in case of re-calculation of the sum
                                                    // of weights on the fly
  std::vector<std::string> names_LHE3;
  bool recalc_LHE3 = false;
  bool recalculateNominalWeightSum = false;
  int dsid = topConfig->getDSID();
  int isAFII = topConfig->isAFII();
  std::string generators = topConfig->getGenerators();
  std::string AMITag = topConfig->getAMITag();
  ULong64_t totalEvents = 0;
  ULong64_t totalEventsInFiles = 0;
  sumWeights->Branch("dsid", &dsid);
  sumWeights->Branch("isAFII", &isAFII);
  sumWeights->Branch("generators", &generators);
  sumWeights->Branch("AMITag", &AMITag);
  sumWeights->Branch("totalEventsWeighted", &totalEventsWeighted);
  if (topConfig->doMCGeneratorWeights()) {// the main problem is that we don't have the list of names a priori
    sumWeights->Branch("totalEventsWeighted_mc_generator_weights", &totalEventsWeighted_LHE3);
    sumWeights->Branch("names_mc_generator_weights", &names_LHE3);
  }
  sumWeights->Branch("totalEvents", &totalEvents, "totalEvents/l");

  TTree* sumPdfWeights = 0;
  std::unordered_map<std::string, std::vector<float>*> totalEventsPdfWeighted;
  int dsidPdf = topConfig->getDSID();
  bool pdfMetadataExists = false;
  if (topConfig->doLHAPDF()) {
    sumPdfWeights = new TTree("PDFsumWeights", "");
    sumPdfWeights->Branch("dsid", &dsidPdf);
    for (const auto& pdf_set : topConfig->LHAPDFSets()) {
      totalEventsPdfWeighted[pdf_set] = new std::vector<float>();
      sumPdfWeights->Branch(pdf_set.c_str(), totalEventsPdfWeighted[pdf_set]);
    }
  }

  //the analysis loop
  ATH_MSG_INFO("Starting event loop\n");

  // Performance stats
  if (doPerfStats > 0) xAOD::PerfStats::instance().start(); // start Perfstats timer

  unsigned int totalYieldSoFar = 0;
  unsigned int skippedEventsSoFar = 0;
  unsigned int eventSavedReco(0), eventSavedRecoLoose(0), eventSavedTruth(0), eventSavedParticle(0),
  eventSavedUpgrade(0);

  // Close the file that we opened only for metadata
  metadataInitFile->Close();

  for (const auto& filename : filenames) {
    if (topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun()) break;
    ATH_MSG_INFO("Opening " << filename);
    std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));

    // the derivation framework is making files with an empty collection tree
    // which TEvent can now handle
    // However, at least for data, this is causing problems with the trigger menu
    // skip the file, after the meta data access above
    const TTree* const collectionTree = dynamic_cast<TTree* > (inputFile->Get("CollectionTree"));
    if (!collectionTree && !topConfig->isMC()) {
      if (top::ConfigurationSettings::get()->feature("SkipInputFilesWithoutCollectionTree")) {
        ATH_MSG_INFO("No CollectionTree found, skipping file");
        continue;
      }
    }

    top::check(xaodEvent.readFrom(inputFile.get()), "xAOD::TEvent readFrom failed");

    // Sum of weights and raw number of entries before skimming in current file
    double sumW_file = 0;
    ULong64_t initialEvents = 0;

    // vector of MC generator weights and their names in current file
    std::vector<float> LHE3_sumW_file;
    std::vector<std::string> LHE3_names_file;

    // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisMetadata#Event_Bookkeepers
    const xAOD::CutBookkeeperContainer* cutBookKeepers = 0;
    // Try to get the cut bookkeepers.
    // <tom.neep@cern.ch> (4/4/16): Not there in DAOD_TRUTH1?
    // If we can't get them and we are running on TRUTH then carry on,
    // but anything else is bad!
    if(topConfig->isTruthDxAOD())
    {
      ATH_MSG_INFO("Bookkeepers are not read for TRUTH derivations");   
    }
    else if (!xaodEvent.retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {
      ATH_MSG_ERROR("Failed to retrieve cut book keepers");
      return 1;
    } else {
      if (topConfig->isMC()) {
        // try to retrieve CutBookKeepers for LHE3Weights first
        top::parseCutBookkeepers(cutBookKeepers, LHE3_names_file, LHE3_sumW_file, topConfig->HLLHC());

        // here we attempt to name the CutBookkeepers based on the MC weight names
        // but we might end up in a situation where we don't have PMGTruthWeightTool
        // e.g. if TruthMetaData container is broken in derivation
        // we continue without names of the MC weights, only indices will be available
        ToolHandle<PMGTools::IPMGTruthWeightTool> m_pmg_weightTool("PMGTruthWeightTool");
        if (m_pmg_weightTool.retrieve()) {
          const std::vector<std::string> &weight_names = m_pmg_weightTool->getWeightNames();
          // if we have MC generator weights, we rename the bookkeepers in sumWeights TTree to match the weight names from MetaData
          top::renameCutBookkeepers(LHE3_names_file, weight_names);
        } else {
          for (std::string &name : LHE3_names_file) {
            name = "?";
          }
        }

        // raw number of events taken from "AllExecutedEvents" bookkeeper, which corresponds to 0th MC weight
        // but these are raw entries, so doesn't matter if 0th MC weight is nominal or not
        initialEvents = top::getRawEventsBookkeeper(cutBookKeepers, topConfig->HLLHC());

        // determine the nominal sum of weight -- we already found the nominal weight in ScaleFactorCalculator
        const size_t nominalWeightIndex = topConfig->nominalWeightIndex();
        sumW_file = LHE3_sumW_file.at(nominalWeightIndex);
      } else {
        initialEvents = top::getRawEventsBookkeeper(cutBookKeepers, topConfig->HLLHC());
        sumW_file = initialEvents; // this is data, it's the same number...
      }
    }

    totalEventsWeighted += sumW_file;
    totalEvents += initialEvents;

    // now we must fill two vectors in sync for MCGeneratorWeights sum of weights
    if (topConfig->doMCGeneratorWeights() && topConfig->isMC()) {
      if (totalEventsWeighted_LHE3.size() != 0) {
        if (totalEventsWeighted_LHE3.size() != LHE3_sumW_file.size()
            || names_LHE3.size() != LHE3_names_file.size()
            || names_LHE3.size() != totalEventsWeighted_LHE3.size()) {
          ATH_MSG_ERROR("Strange inconsistency of vector sizes in sum of LHE3 weights calculation.");
          return 1;
        }
        for (unsigned int i_genweights = 0; i_genweights < LHE3_names_file.size();
             i_genweights++) {
          if (names_LHE3.at(i_genweights) != LHE3_names_file.at(i_genweights)) {
            ATH_MSG_ERROR("Strange inconsistency in the vector of weight names in sum of LHE3 weights calculation.");
            return 1;
          } else {
            totalEventsWeighted_LHE3.at(i_genweights)
              = totalEventsWeighted_LHE3.at(i_genweights)
                + LHE3_sumW_file.at(i_genweights);
          }
        }
      } else {
        for (unsigned int i_genweights = 0; i_genweights < LHE3_names_file.size();
             i_genweights++) {
          names_LHE3.push_back(LHE3_names_file.at(i_genweights));
          totalEventsWeighted_LHE3.push_back(LHE3_sumW_file.at(i_genweights));
        }
      }
      if (!names_LHE3.empty()) {
        ATH_MSG_INFO("The sum of weights for the following LHE3 weights were retrieved from the input file:");
        for (std::string s : names_LHE3)
          msg(MSG::Level::INFO) << s << " ";
        msg(MSG::Level::INFO) << std::endl;
      } else {
        ATH_MSG_INFO("No sum of LHE3 weights could be found in meta-data. Will try to recompute these sums.\n"
            "This only works on un-skimmed derivations, and the names of these weights may be unknown (but we'll try to read them from the PMG tool");
        recalc_LHE3 = true;
      }
    }
    
    if (topConfig->isTruthDxAOD()) recalculateNominalWeightSum=true;

    if (topConfig->printCDIpathWarning()) {
      ATH_MSG_WARNING(
          "\n*************************************************************************\n"
          << "YOU ARE USING A CUSTOM PATH TO THE CDI FILE WHICH IS NOT THE DEFAULT PATH\n"
          << "       YOU MANY NOT BE USING THE LATEST BTAGGING RECOMMENDATIONS         \n"
          << "*************************************************************************\n\n");
    }

    const unsigned int entries = xaodEvent.getEntries();
    totalEventsInFiles += entries;
    unsigned int firstEvent = 0;
    if (topConfig->numberOfEventsToSkip() > skippedEventsSoFar) {
      firstEvent = topConfig->numberOfEventsToSkip() - skippedEventsSoFar;
      skippedEventsSoFar += entries < firstEvent ? entries : firstEvent;
    }
    unsigned int entry;
    bool isFirst(true);
    for (entry = firstEvent; entry < entries; ++entry, ++totalYieldSoFar) {
      if (topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun()) break;

      if (entry % 100 == 0)
        ATH_MSG_INFO("Processing event " << totalYieldSoFar << " / " << totalYield
            << " (current file: " << entry << " / " << entries << ")");

      // clear the TStore - get rid of the last events objects
      xAOD::TActiveStore::store()->clear();
      // increment the event
      xaodEvent.getEntry(entry);

      // Pile up and MC event weight - used to normalize the cut flows
      float mcEventWeight(1.), pileupWeight(1.), zvtxWeight(1.);
      if (topConfig->isMC()) mcEventWeight = topScaleFactors->mcEventWeight();

      if (topConfig->doPileupReweighting() && !topConfig->isTruthDxAOD()) {
        top::check(topScaleFactors->executePileup(), "Failed to execute pileup reweighting");
        pileupWeight = topScaleFactors->pileupWeight();
      }

      // perform any operation common to both reco and truth level
      // currently we load the MC generator weights inside, if requested
      eventSaver->execute();

      ///-- Truth events --///
      if (topConfig->isMC()) {
        // Save, if requested, MC truth block, PDFInfo, TopPartons
        // This will be saved for every event

        // Run topPartonHistory
        if (topConfig->doTopPartonHistory()) top::check(topPartonHistory->execute(), "Failed to execute topPartonHistory");

        // calculate PDF weights
        if (topConfig->doLHAPDF()) top::check(PDF_SF->execute(),
                                              "Failed to execute PDF SF");

        eventSaver->saveTruthEvent();
        if(topConfig->doTopPartonLevel()) ++eventSavedTruth;

        // Upgrade analysis - only for truth DAODs when asking to do upgrade studies
        if (topConfig->isTruthDxAOD() && topConfig->HLLHC()) {
          top::ParticleLevelEvent upgradeEvent = upgradeLoader.load();

          //event selection
          const bool saveEventInOutputFile = eventSelectionManager.applyUpgradeLevel(upgradeEvent);

          if (saveEventInOutputFile) {
            eventSaver->saveUpgradeEvent(upgradeEvent);
            ++eventSavedUpgrade;
          }
        }

        // Particle level analysis, saved only for truth events passing fiducial selection

        // --------------------------------------------------
        // If the truth loader is active, perform truth loading.
        if (particleLevelLoader.active()) {
          // --------------------------------------------------
          // Get the top::TruthEvent for the current event
          top::ParticleLevelEvent particleLevelEvent = particleLevelLoader.load();

          //event selection
          const bool saveEventInOutputFile = eventSelectionManager.applyParticleLevel(particleLevelEvent);

          if (saveEventInOutputFile) {
            eventSaver->saveParticleLevelEvent(particleLevelEvent);
            ++eventSavedParticle;
          }
        }
      }
      if (totalYieldSoFar == 0 && topConfig->isMC() && topConfig->doLHAPDF()) {
        const xAOD::TruthEventContainer* truthEvent(nullptr);
        top::check(xaodEvent.retrieve(truthEvent, topConfig->sgKeyTruthEvent()), "Failed to retrieve TruthEvent container for LHAPDF");
        top::check(truthEvent->size() == 1, "TruthEvent container size != 1, not sure what to do with PDF reweighting");
        for (auto tePtr : *truthEvent) {
          for (auto& pdf : totalEventsPdfWeighted) {
            if (tePtr->isAvailable< std::vector<float> >("AnalysisTop_" + pdf.first + "_Weights")) {
              pdf.second->resize(tePtr->auxdata< std::vector<float> >("AnalysisTop_" + pdf.first + "_Weights").size());
            }
          }
        }
      }
      // on the first event, set the size of the vector of sum of LHE3 weights in case it needs to be calculated on the
      // fly

      if (topConfig->isMC()) {
        const xAOD::EventInfo* ei(nullptr);
        top::check(xaodEvent.retrieve(ei, topConfig->sgKeyEventInfo()),
                   "Failed to retrieve LHE3 weights from EventInfo");
        
        if(recalculateNominalWeightSum)
        {
          if (totalYieldSoFar == 0) ATH_MSG_INFO("Trying to recalculate nominal weights sum for TRUTH derivation");
          const size_t nominalWeightIndex = topConfig->nominalWeightIndex();
          totalEventsWeighted_temp += ei->mcEventWeights().at(nominalWeightIndex);
          totalEvents++;
        }
        
        if(topConfig->doMCGeneratorWeights())
        {
          unsigned int weightsSize = ei->mcEventWeights().size();
          if (recalc_LHE3) {
            if (totalYieldSoFar == 0) {
              totalEventsWeighted_LHE3_temp.resize(weightsSize);
              for (unsigned int i_LHE3 = 0; i_LHE3 < weightsSize; i_LHE3++) {
                totalEventsWeighted_LHE3_temp.at(i_LHE3) = ei->mcEventWeights().at(i_LHE3);
              }
              names_LHE3.resize(weightsSize);
              
              ToolHandle<PMGTools::IPMGTruthWeightTool> m_pmg_weightTool("PMGTruthWeightTool");
              if (m_pmg_weightTool.retrieve()) {
                const std::vector<std::string> &weight_names = m_pmg_weightTool->getWeightNames();
                if(weight_names.size() != weightsSize)
                {
                  ATH_MSG_INFO("In top-xaod, while calculating mc weights sums on the fly, names from PMG tools have different size wrt weight vector, we'll not retrieve weight names");
                  std::fill(names_LHE3.begin(), names_LHE3.end(), "?");
                }
                else{
                  for(unsigned int i_wgt=0; i_wgt<weight_names.size(); i_wgt++) names_LHE3[i_wgt]=weight_names[i_wgt];
                }
              }
              else{
                std::fill(names_LHE3.begin(), names_LHE3.end(), "?");
              }
            } else {
              for (unsigned int i_LHE3 = 0; i_LHE3 < weightsSize; i_LHE3++) {
                totalEventsWeighted_LHE3_temp.at(i_LHE3) = totalEventsWeighted_LHE3_temp.at(i_LHE3) +
                                                           ei->mcEventWeights().at(i_LHE3);
              }
            }
          } else if (weightsSize != names_LHE3.size()) {// don't recalc sum of weights, but cross-check the size of the
                                                        // vectors
            ATH_MSG_ERROR("Strange size inconsistency in the AllExecutedEvents* "
                "sum of weights  bookkeepers from the meta-data and the vector of "
                "LHE3 weights in the EventInfo container.");
            return 1;
          }
        }
      }
      ///-- End of Truth events -- start of reconstruction level events --///
      if (topConfig->isTruthDxAOD()) continue;


      ///-- We veto events that do not pass the GRL, trigger or have bad calorimeters --///
      ///-- No point calibrating and cutting on events that fail these cuts --///
      ///-- We only apply a veto if ALL selectors request a cut --///
      ///-- And if the user selects "OutputEvents SelectedEvents" --///

      ///-- Count initial events --///
      eventSelectionManager.countInitial(mcEventWeight, pileupWeight, zvtxWeight);

      ///-- Does event pass the GRL? (always true for MC) --///
      ///-- Only veto events when ALL user selectors request GRL --///
      bool passGRLVeto = eventCleaning->applyGRL();
      if (!passGRLVeto) continue;
      eventSelectionManager.countGRL(mcEventWeight, pileupWeight, zvtxWeight);

      ///-- Are the Tile and LAr calorimeters in a good state? (always true for MC) --///
      ///-- Only veto events when ALL user selectors request GOODCALO --///
      bool passGoodCalo = eventCleaning->applyGoodCalo();
      if (!passGoodCalo) continue;
      eventSelectionManager.countGoodCalo(mcEventWeight, pileupWeight, zvtxWeight);

      ///-- Do we have a Primary Vertex? --///
      ///-- Only veto events when ALL user selectors request PRIVTX -- ///
      bool passPriVtx = eventCleaning->applyPrimaryVertex();
      if (!passPriVtx) continue;
      eventSelectionManager.countPrimaryVertex(mcEventWeight, pileupWeight, zvtxWeight);

      ///-- Wondering which triggers are available ??? --///
      ///-- Uncomment this line and get ready for a LOT of output --///
      // eventCleaning->printTriggers();

      ///-- Does event pass any trigger ? --///
      ///-- Only veto events when ALL user selectors request TRIGDEC --///
      ///-- And all trigger fail --///
      bool passAnyTriggerVeto = eventCleaning->applyTrigger();
      if (!passAnyTriggerVeto) continue;

      ///-- Nominal objects -- ///
      ///-- Calibrate objects and make all required systematic copies --///
      top::check(systObjMaker->execute(true), "Failed to execute systObjMaker");

      ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later --///
      top::check(objectSelection->execute(true), "Failed to execute objectSelection");

      ///-- Recalculate MissingET based on object selection --///
      top::check(systObjMaker->recalculateMET(true), "Failed to recalculateMET with systObjMaker");

      ///-- Systematic objects -- ///
      ///-- Calibrate objects and make all required systematic copies --///
      top::check(systObjMaker->execute(false), "Failed to execute systObjMaker");

      ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later --///
      top::check(objectSelection->execute(false), "Failed to execute objectSelection");

      ///-- Recalculate MissingET based on object selection --///
      top::check(systObjMaker->recalculateMET(false), "Failed to recalculateMET with systObjMaker");


      ///-- Scale Factor calculation --///
      if (topConfig->isMC()) top::check(topScaleFactors->execute(), "Failed to calculate scale factors");

      if (topConfig->doTightEvents()) {
        ///-- Loop over all systematics and make a "top::Event" for each --///
        const xAOD::SystematicEventContainer* allSystematics = topEventMaker->systematicEvents(
          topConfig->sgKeyTopSystematicEvents());
        for (auto currentSystematic : *allSystematics) {
          if (!(currentSystematic->hashValue() == topConfig->nominalHashValue() || topConfig->doTightSysts())) continue;

          ///-- Make a top::Event --///
          top::Event topEvent = topEventMaker->makeTopEvent(currentSystematic);
          ///-- Apply event selection --///
          const bool passAnyEventSelection = eventSelectionManager.apply(topEvent, *currentSystematic);
          // check if we are using actual mu for mc16d or mc16e
          if (isFirst && topConfig->isMC()) {
            const int runNumber = topEvent.m_info->runNumber();
            if (runNumber >= 300000) {
              if ((!topConfig->isAFII() && topConfig->PileupActualMu_FS().size() == 0) || 
                (topConfig->isAFII() && topConfig->PileupActualMu_AF().size() == 0)) {
                ATH_MSG_WARNING("\n***************************************************************************************\nYou are running over mc16d or mc16e sample but you are not using actual mu reweighting!\nYou are strongly adviced to use it.\nCheck https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuideR21#PRW_and_Lumicalc_files\n***************************************************************************************\n");
              }
            }
          }
          currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
          topEvent.m_saveEvent = passAnyEventSelection;
          ///-- Save event - we defer to eventSaver the decision to write or not --///
          eventSaver->saveEvent(topEvent);

          ///-- For printout at the end of job, only count nominal events --///
          if (passAnyEventSelection &&
              currentSystematic->hashValue() == topConfig->nominalHashValue()) ++eventSavedReco;
        }
        ///-- End loop over all calibration systematics --///
      }
      ///-- End of tight events --///

      if (topConfig->doLooseEvents()) {
        ///-- Loop over all Loose systematics and make a "top::Event" for each --///
        const xAOD::SystematicEventContainer* allSystematicsLoose = topEventMaker->systematicEvents(
          topConfig->sgKeyTopSystematicEventsLoose());
        for (auto currentSystematic : *allSystematicsLoose) {
          if (!(currentSystematic->hashValue() == topConfig->nominalHashValue() || topConfig->doLooseSysts())) continue;

          ///-- Make a top::Event --///
          top::Event topEvent = topEventMaker->makeTopEvent(currentSystematic);
          ///-- Apply event selection --///
          const bool passAnyEventSelection = eventSelectionManager.apply(topEvent, *currentSystematic);

          // check if we are using actual mu for mc16d or mc16e
          if (isFirst && topConfig->isMC()) {
            const int runNumber = topEvent.m_info->runNumber();
            ATH_MSG_INFO("RunNumber: " << runNumber);
            if (runNumber >= 300000) {
              if ((!topConfig->isAFII() && topConfig->PileupActualMu_FS().size() == 0) || 
                (topConfig->isAFII() && topConfig->PileupActualMu_AF().size() == 0)) {
                ATH_MSG_WARNING("\n***************************************************************************************\nYou are running over mc16d or mc16e sample but you are not using actual mu reweighting!\nYou are strongly adviced to use it.\nCheck https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuideR21#PRW_and_Lumicalc_files\n***************************************************************************************\n");
              }
            }
          }
          currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
          topEvent.m_saveEvent = passAnyEventSelection;
          ///-- weights for matrix-method fakes estimate from IFF tools, only for nominal --///
          if (!topConfig->isMC() && topConfig->doFakesMMWeightsIFF() &&
          currentSystematic->hashValue() == topConfig->nominalHashValue()) {
            xAOD::IParticleContainer lepton(SG::VIEW_ELEMENTS);
            for (auto const& t : topEvent.m_electrons)
              lepton.push_back(static_cast<xAOD::Electron*>(t));
            for (auto const& t : topEvent.m_muons)
              lepton.push_back(static_cast<xAOD::Muon*>(t));

            topEvent.m_info->auxdecor<int>("njets") = topEvent.m_jets.size();
            for (const auto& tagWP : topConfig->bTagWP_available()) {
              if (tagWP.find("Continuous") != std::string::npos) continue;
              int nbjets = 0;
              for (const auto& jetPtr : topEvent.m_jets) {
                if (jetPtr->isAvailable<char>("isbtagged_" + tagWP)) {
                  nbjets += jetPtr->auxdataConst<char>("isbtagged_" + tagWP);
                }
              }
              topEvent.m_info->auxdecor<int>("nbjets_" + tagWP) = nbjets;
            }

            std::vector<float> mmweight;
            std::vector<std::vector<float> > mmweight_var;
            std::vector<std::vector<std::string> > mmweight_varname;
            for (unsigned int mmi = 0; mmi < topfakesMMWeightsIFF.size(); ++mmi) {
              top::check(topfakesMMWeightsIFF[mmi]->addEvent(lepton),
              "Failed to execute fakes mmweight IFF addEvent()");
              float asmWgt = 0.;
              top::check(topfakesMMWeightsIFF[mmi]->applySystematicVariation(
                {}), "Failed to execute fakes mmweight IFF applySystematicVariation()");
              top::check(topfakesMMWeightsIFF[mmi]->getEventWeight(asmWgt, FakesMMConfigIFF[mmi][1],
              FakesMMConfigIFF[mmi][2]), "Failed to execute fakes mmweight IFF getEventWeight()");

              std::vector<float> asmWgt_var(topfakesMMWeightsIFF[mmi]->affectingSystematics().size());
              std::vector<std::string> asmWgt_varname(topfakesMMWeightsIFF[mmi]->affectingSystematics().size());
              for (const auto& sysvar : topfakesMMWeightsIFF[mmi]->affectingSystematics()) {
                float mmweight_syst;
                top::check(topfakesMMWeightsIFF[mmi]->applySystematicVariation(
                  {sysvar}), "Failed to execute fakes mmweight IFF applySystematicVariation()");
                top::check(topfakesMMWeightsIFF[mmi]->getEventWeight(mmweight_syst, FakesMMConfigIFF[mmi][1],
                FakesMMConfigIFF[mmi][2]), "Failed to execute fakes mmweight IFF getEventWeight()");
                asmWgt_var.push_back(mmweight_syst);
                asmWgt_varname.push_back(sysvar.name());
              }
              mmweight.push_back(asmWgt);
              mmweight_var.push_back(asmWgt_var);
              mmweight_varname.push_back(asmWgt_varname);
            }
            topEvent.m_info->auxdecor<std::vector<float> >("ASMWeight") = mmweight;
            topEvent.m_info->auxdecor<std::vector<std::vector<float> > >("ASMWeight_Syst") = mmweight_var;
            topEvent.m_info->auxdecor<std::vector<std::vector<std::string> > >("ASMWeight_Systname") = mmweight_varname;
          }
          ///-- Save event - we defer to eventSaver the decision to write or not --///
          eventSaver->saveEvent(topEvent);

          ///-- For printout at the end of job, only count nominal events --///
          if (passAnyEventSelection &&
          currentSystematic->hashValue() == topConfig->nominalHashValue()) ++eventSavedRecoLoose;
        }
        ///-- End loop over all loose calibration systematics --///
      }
      ///-- End of loose events --///

      ///-- Needed for xAOD output, all systematics go into the same TTree --///
      eventSaver->saveEventToxAOD();
      isFirst = false;
    } //loop over events in current file
    if (tracker) tracker->addInputFile(inputFile->GetName(), entry - firstEvent);

    // do it at the end, so we can get the DS id from the first event
    // notice this is different from the normal sum of weights: all entries matter, not only the highest per file
    if (topConfig->doLHAPDF()) {
      bool pdfInCBK = false;
      if (xaodEvent.retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {
        // try to get the first entry
        std::string pdf_set = totalEventsPdfWeighted.begin()->first;
        std::string p = pdf_set + "_0";
        for (auto cbk : *cutBookKeepers) {
          std::string pdfName = cbk->name();
          if (p != pdfName) continue;
          pdfMetadataExists = true;
          pdfInCBK = true;
          // reload to set iterator back to original state
          if (!xaodEvent.retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {
          }
          break;
        }
      }
      // if the pdf info was in the CBK the PDFSumOfWeights retrieve never happens,
      // and cutBookKeepers, will keep its reference to the main CBK
      if (!pdfInCBK) { // only try this if the info is not in the CBK -- this is legacy stuff
        if (!xaodEvent.retrieveMetaInput(cutBookKeepers, "PDFSumOfWeights")) {
          pdfMetadataExists = false;
        }
      }
      if (pdfMetadataExists && topConfig->saveLHAPDFEvent() && topConfig->baseLHAPDF().empty()) {
        // just make a copy of it, but not all derivations have it, so just ignore it if it is not there
        for (auto& pdfentry : totalEventsPdfWeighted) {
          std::string pdf_set = pdfentry.first;
          for (size_t n = 0; n < totalEventsPdfWeighted[pdf_set]->size(); ++n) {
            std::string p = pdf_set + "_" + std::to_string(n);
            bool foundPdf = false;
            for (auto cbk : *cutBookKeepers) {
              std::string pdfName = cbk->name();
              if (p != pdfName) continue;
              totalEventsPdfWeighted[pdf_set]->at(n) = cbk->sumOfEventWeights();
              foundPdf = true;
              break;
            }
            if (!foundPdf) { // asked for PDF weighting, the PDF metadata exists, but this particular PDF is missing
              // crash hard
              ATH_MSG_ERROR(
                  "The force is not strong with us, young Padawan ...\n"
                  "You want to save weights to do PDF reweighting using '" << pdf_set <<
                  "', which I figure has " << totalEventsPdfWeighted[pdf_set]->size() << " sets on it.\n"
                  "There is metadata information for the sum of MC*PDF weights in PDFSumOfWeights, "
                  "but none seem to refer to '" << p << "' therefore I do not know how to estimate "
                  "the sum of weights before acceptance for this configuration.\n"
                  "May the force be with you in your next attempt.");
              return 1;
            }
          }
        }
        sumPdfWeights->Fill();
      }
    } //doLHAPDF
  } //loop over files

  if (doPerfStats > 0) xAOD::PerfStats::instance().stop(); // Stop the PerfStats timer

  if (topConfig->doLHAPDF()) top::check(PDF_SF->finalize(), "Failed to finalize PDF SF");

  // this fills the sum of weights
  // if the list of input files has more than
  // one type of dataset (ie: the first file is ttbar and the second one
  // is W+jets), then the Fill statement below needs to come in the end of
  // each file's processing instead ... but why would you do that?!
  if (recalc_LHE3) {// in case the sum of LHE3 weight has been re-calculated with double (but we need floats in the end)
    for (double d:totalEventsWeighted_LHE3_temp) totalEventsWeighted_LHE3.push_back(d);
  }
  if(recalculateNominalWeightSum)
  {
    totalEventsWeighted=totalEventsWeighted_temp;
  }
  sumWeights->Fill();
  outputFile->cd();

  if (topConfig->doLHAPDF()) {
    // Save sum of weights of PDF variations
    // Only do this if the metadata is not available
    if (totalEventsInFiles != totalEvents && !pdfMetadataExists) {
      ATH_MSG_ERROR("These are not the droids you are looking for.\n"
          "You are running over skimmed derivations. We cannot determine "
          "the sum of MC*PDF weights before skimming "
          "because no PDF metadata is available in the file!");
      return 1;
    }
    // save recomputed sum weights
    if ((!topConfig->baseLHAPDF().empty() || !pdfMetadataExists)) {
      for (auto& pdf_result : topConfig->getLHAPDFResults())
        *(totalEventsPdfWeighted[pdf_result.first]) = pdf_result.second;
      sumPdfWeights->Fill();
    }
  } //doLHAPDF

  outputFile->cd();
  eventSelectionManager.finalise();
  eventSaver->finalize();
  outputFile->cd();
  if (tracker) tracker->writeTree("AnalysisTracking");
  outputFile->Close();
  bool outputFileGood = !outputFile->TestBit(TFile::kWriteError);
  if (outputFileGood) {
    if (gSystem->Rename(outputFile->GetName(), settings->value("OutputFilename").c_str()) != 0) outputFileGood = false;
  }

  if (!topConfig->isTruthDxAOD()) {
    if (topConfig->doTightEvents())
      ATH_MSG_INFO("Events saved to output file nominal reconstruction tree: " << eventSavedReco);
    if (topConfig->doLooseEvents())
      ATH_MSG_INFO("Events saved to output file nominal loose reconstruction tree: " << eventSavedRecoLoose);
  }
  if (topConfig->isMC()) {
    ATH_MSG_INFO("Events saved to output file truth tree : " << eventSavedTruth);
    if (particleLevelLoader.active()) {
      ATH_MSG_INFO("Events saved to output file particle level tree : " << eventSavedParticle);
    }
    if (upgradeLoader.active()) {
      ATH_MSG_INFO("Events saved to output file upgrade tree : " << eventSavedUpgrade);
    }
  }
  ATH_MSG_INFO("Total sum-of-weights (for normalization) : " << totalEventsWeighted);

  //print some xAOD performance info
  if (doPerfStats == 1) xAOD::IOStats::instance().stats().Print("Summary");
  if (doPerfStats == 2) xAOD::IOStats::instance().stats().Print();

  if (!outputFileGood) {
    ATH_MSG_ERROR("ERROR: an I/O error occured while attempting to save the output file.");
    return 1;
  }

  return 0;
}
