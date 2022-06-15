#include "TopAnalysis/TopxAODAlg.h"
#include "EventLoop/IWorker.h"
#include "TopAnalysis/Tools.h"
#include "TopCPTools/TopToolStore.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopCorrections/PDFScaleFactorCalculator.h"
#include "TopCorrections/ScaleFactorCalculator.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopObjectSelectionTools/EventCleaningSelection.h"
#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopParticleLevel/ParticleLevelLoader.h"
#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopSystematicObjectMaker/ObjectCollectionMaker.h"
// for I/O performance monitoring
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/ReadStats.h"

namespace top {
TopxAODAlg::TopxAODAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator) {
  declareProperty("ConfigPath", m_configPath, "Path to config file to parse by top-el");
  declareProperty("LibraryNames", m_libraryNames, "Names of loaded .so libraries");
  declareProperty("DerivationStream", m_derivationStream, "DerivationStream of this sample");
  declareProperty("CrashOnMetaDataFaile", m_crashOnMetaDataFail,
                  "Crash if metadata reading failed");
  declareProperty("StreamName", m_streamName, "The name of the output stream");
  declareProperty("FTAGMapIndex", m_FTAGMapIndex,
                  "Index of the showering algorithm for FTAG MC/MC SFs");
  declareProperty("ShoweringAlgorithm", m_showeringAlgo,
                  "Name of the showering algorithm for FTAG MC/MC SFs");
  declareProperty("DataTakingYear", m_year,
                  "Year of data taking (primarily for trigger configuration)");
  declareProperty("SkipEvents", m_firstEvent, "From which event entry to start (skip before)");
}

StatusCode TopxAODAlg::initialize() {
  ANA_CHECK(requestBeginInputFile());
  ANA_CHECK(requestEndInputFile());

  m_settings.reset(top::ConfigurationSettings::get(true));
  m_settings->loadFromFile(m_configPath);

  m_config.reset(new top::TopConfig());
  // read FileMetadata for our TopConfig class
  const xAOD::FileMetaData *fmd = nullptr;
  if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
    ANA_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
    top::readMetaData(fmd, m_config);
  } else {
    ATH_MSG_ERROR("Could not retrieve FileMetadata. We thus cannot reliably pre-determine DSID, "
                  "whether this is MC, and simulation type.");
    if (m_crashOnMetaDataFail) {
      return StatusCode::FAILURE;
    }
  }

  m_config->setConfigSettings(m_settings.get());

  if (m_config->isMC() && !m_config->isTruthDxAOD()) {
    // at this point we should also configure showering algorithm for FTAG MC/MC SFs
    m_config->setMapIndex(m_FTAGMapIndex);
    m_config->setShoweringAlgorithm(m_showeringAlgo);
  }

  // trigger configuration
  m_config->SetYear(m_year);
  m_config->SetTriggersToYear(m_config->isMC());

  m_firstEvent = m_config->numberOfEventsToSkip();

  // analysis tracking
  {
    bool useTracking = true;
    m_settings->retrieve("WriteTrackingData", useTracking);
    if (useTracking) {
      m_tracker.reset(new top::AnalysisTrackingHelper());
    }
  }

  // I/O Performance stats?
  // Summary gives a summary
  // Full gives detailed info on each collection in the file
  if (m_settings->value("PerfStats") == "Summary")
    m_doPerfStats = 1;

  if (m_settings->value("PerfStats") == "Full")
    m_doPerfStats = 2;

  // Setup all asg::AsgTools
  m_topTools = new top::TopToolStore("top::TopToolStore");
  top::check(m_topTools->setProperty("config", m_config), "Failed to setProperty of topTools");
  top::check(m_topTools->initialize(), "Failed to initialize topTools");

  if (m_config->isMC() && !m_pmg_weightTool.retrieve()) {
    ATH_MSG_ERROR("Cannot retrieve PMGTruthWeightTool");
    return StatusCode::FAILURE;
  }

  // EventCleaningSelection
  // Decorates EventInfo with GRL decision (always true for MC)
  // Evaluates all triggers requested from all selections
  // Trigger matching
  // Check for good Tile and LAr calorimeters
  // Let's check this passes before running expensive calibrations
  // If ANY selection does not request a trigger, we won't veto any events
  m_eventCleaning = new top::EventCleaningSelection("top::EventCleaningSelection");
  top::check(m_eventCleaning->setProperty("config", m_config),
             "Failed to setProperty of triggerGRLSelection");
  top::check(m_eventCleaning->initialize(), "Failed to initialize triggerGRLSelection");
  m_eventCleaning->setEventSelections(m_settings->selections());

  // Systematic object collection making
  m_systObjMaker = new top::ObjectCollectionMaker("top::ObjectCollectionMaker");
  top::check(m_systObjMaker->setProperty("config", m_config),
             "Failed to setProperty of systObjMaker");
  if (!m_config->isTruthDxAOD())
    top::check(m_systObjMaker->initialize(), "Failed to initialize systObjMaker");

  // object selection
  m_objectSelection = top::loadObjectSelection(m_config);
  m_objectSelection->print(msg(MSG::Level::INFO));

  // setup event-level cuts
  m_outputFile = wk()->getOutputFile(m_streamName);
  m_eventSelectionManager.reset(new top::EventSelectionManager(
      m_settings->selections(), m_outputFile, m_libraryNames, m_config));

  // The loader tool for top::ParticleLevelEvent objects
  m_particleLevelLoader = new top::ParticleLevelLoader(m_config);

  // The loader tool for Upgrade objects
  //  top::UpgradeObjectLoader upgradeLoader(m_config);

  // Fix the configuration - it now knows about:
  //     * all objects collections to work with
  //     * all systematic variations
  m_config->fixConfiguration();

  // OK let's printout the TopConfig
  ATH_MSG_INFO(*m_config << "\n");

  if (m_tracker)
    m_tracker->setTopConfig(m_config);

  // Top parton history for MC events
  //  This is quite ugly and simple, it will be harmonized with in the future
  //  along with all other factory methods (it's not a factory method right now)
  if (m_settings->value("TopPartonHistory") == "ttbar") {
    m_topPartonHistory = new top::CalcTtbarPartonHistory("top::CalcTtbarPartonHistory");
    top::check(m_topPartonHistory->setProperty("config", m_config),
               "Failed to setProperty of top::CalcTtbarPartonHistory");
  } else {
    ATH_MSG_ERROR("Only ttbar TopPartonHistory is supported for the moment!");
    return StatusCode::FAILURE;
  }

  // LHAPDF SF calculation
  m_PDF_SF = new top::PDFScaleFactorCalculator("top::PDFScaleFactorCalculator");
  if (m_config->doLHAPDF()) {
    top::check(m_PDF_SF->setProperty("config", m_config),
               "Failed to set config for PDF SF Calculator");
    top::check(m_PDF_SF->initialize(), "Failed to initialize PDF SF calculator");
  }

  // make top::Event objects
  m_topEventMaker = new top::TopEventMaker("top::TopEventMaker");
  top::check(m_topEventMaker->setProperty("config", m_config),
             "Failed to setProperty of top::TopEventMaker");
  top::check(m_topEventMaker->initialize(), "Failed to initialize top::TopEventMaker");
  // Debug messages?
  // m_topEventMaker.msg().setLevel(MSG::DEBUG);

  m_topScaleFactors = new top::ScaleFactorCalculator("top::ScaleFactorCalculator");
  top::check(m_topScaleFactors->setProperty("config", m_config),
             "Failed to setProperty of top::ScaleFactorCalculator");
  top::check(m_topScaleFactors->initialize(), "Failed to initialize  top::ScaleFactorCalculator");

  // TODO asymptotic matrix method

  // A list of extra branches that we create in this code and want to save
  std::vector<std::string> extraBranches;
  m_eventSelectionManager->addExtraBranches(extraBranches);

  // output file format (and any user-modified code)
  m_eventSaver.reset(top::loadEventSaver(m_config));
  m_eventSaver->initialize(m_config, m_outputFile, extraBranches);

  // save sum of weights before derivations
  m_outputFile->cd();
  m_sumWeights = new TTree("sumWeights", "");
  m_boostedTaggersSFSysNames = m_config->boostedTaggersSFSysNames();
  m_DSID = m_config->getDSID();
  m_isAFII = m_config->isAFII();

  m_sumWeights->Branch("dsid", &m_DSID);
  m_sumWeights->Branch("isAFII", &m_isAFII);
  m_sumWeights->Branch("totalEventsWeighted", &m_totalEventsWeighted);
  if (m_config->doMCGeneratorWeights()) { // the main problem is that we don't have the list of
                                          // names a priori
    m_sumWeights->Branch("totalEventsWeighted_mc_generator_weights", &m_totalEventsWeighted_LHE3);
    m_sumWeights->Branch("names_mc_generator_weights", &m_names_LHE3);
  }
  m_sumWeights->Branch("totalEvents", &m_totalEvents, "totalEvents/l");

  for (auto &it : m_boostedTaggersSFSysNames) {
    m_sumWeights->Branch(("sysNames_" + it.first).c_str(), &it.second);
  }

  unsigned int dsidPdf = m_config->getDSID();
  if (m_config->doLHAPDF()) {
    m_sumPdfWeights = new TTree("PDFsumWeights", "");
    m_sumPdfWeights->Branch("dsid", &dsidPdf);
    for (const auto &pdf_set : m_config->LHAPDFSets()) {
      m_totalEventsPdfWeighted[pdf_set] = std::make_unique<std::vector<float>>();
      m_sumPdfWeights->Branch(pdf_set.c_str(), m_totalEventsPdfWeighted[pdf_set].get());
    }
  }

  // the analysis loop
  ATH_MSG_INFO("Starting event loop\n");

  // Performance stats
  if (m_doPerfStats > 0)
    xAOD::PerfStats::instance().start(); // start Perfstats timer

  return StatusCode::SUCCESS;
}

StatusCode TopxAODAlg::beginInputFile() {
  // Sum of weights and raw number of entries before skimming in current file
  double sumW_file = 0;
  ULong64_t initialEvents = 0;

  // vector of MC generator weights and their names in current file
  std::vector<float> LHE3_sumW_file;
  std::vector<std::string> LHE3_names_file;
  const xAOD::CutBookkeeperContainer *cutBookKeepers = nullptr;

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("Bookkeepers are not read for TRUTH derivations");
  } else {
    top::check(inputMetaStore()->retrieve(cutBookKeepers, "CutBookkeepers"),
               "Cannot retrieve CutBookkeepers");
    if (m_config->isMC()) {

      // here we attempt to name the CutBookkeepers based on the MC weight names
      const std::vector<std::string> &weight_names = m_pmg_weightTool->getWeightNames();
      // try to retrieve CutBookKeepers for LHE3Weights first
      top::parseCutBookkeepers(*evtStore()->event(), weight_names.size(), LHE3_names_file,
                               LHE3_sumW_file);
      // if we have MC generator weights, we rename the bookkeepers in sumWeights TTree to match the
      // weight names from MetaData
      top::renameCutBookkeepers(LHE3_names_file, weight_names);

      // raw number of events taken from "AllExecutedEvents" bookkeeper, which corresponds to 0th MC
      // weight but these are raw entries, so doesn't matter if 0th MC weight is nominal or not
      initialEvents = top::getRawEventsBookkeeper(cutBookKeepers);

      // determine the nominal sum of weight -- we already found the nominal weight in
      // ScaleFactorCalculator
      const size_t nominalWeightIndex = m_config->nominalWeightIndex();
      sumW_file = LHE3_sumW_file.at(nominalWeightIndex);
    } else {
      initialEvents = top::getRawEventsBookkeeper(cutBookKeepers);
      sumW_file = initialEvents; // this is data, it's the same number...
    }
  }

  m_totalEventsWeighted += sumW_file;
  m_totalEvents += initialEvents;

  // now we must fill two vectors in sync for MCGeneratorWeights sum of weights
  if (m_config->doMCGeneratorWeights() && m_config->isMC()) {
    if (m_totalEventsWeighted_LHE3.size() != 0) {
      if (m_totalEventsWeighted_LHE3.size() != LHE3_sumW_file.size() ||
          m_names_LHE3.size() != LHE3_names_file.size() ||
          m_names_LHE3.size() != m_totalEventsWeighted_LHE3.size()) {
        ATH_MSG_ERROR("Strange inconsistency of vector sizes in sum of LHE3 weights calculation.");
        return StatusCode::FAILURE;
      }
      for (unsigned int i_genweights = 0; i_genweights < LHE3_names_file.size(); i_genweights++) {
        if (m_names_LHE3.at(i_genweights) != LHE3_names_file.at(i_genweights)) {
          ATH_MSG_ERROR("Strange inconsistency in the vector of weight names in sum of LHE3 "
                        "weights calculation.");
          return StatusCode::FAILURE;
        } else {
          m_totalEventsWeighted_LHE3.at(i_genweights) =
              m_totalEventsWeighted_LHE3.at(i_genweights) + LHE3_sumW_file.at(i_genweights);
        }
      }
    } else {
      for (unsigned int i_genweights = 0; i_genweights < LHE3_names_file.size(); i_genweights++) {
        m_names_LHE3.push_back(LHE3_names_file.at(i_genweights));
        m_totalEventsWeighted_LHE3.push_back(LHE3_sumW_file.at(i_genweights));
      }
    }
    if (!m_names_LHE3.empty()) {
      ATH_MSG_INFO(
          "The sum of weights for the following LHE3 weights were retrieved from the input file:");
      MsgStream &msgInfo = msg(MSG::Level::INFO);
      for (std::string s : m_names_LHE3)
        msgInfo << s << " ";
      msgInfo << std::endl;
      msgInfo.doOutput();
    } else {
      ATH_MSG_INFO(
          "No sum of LHE3 weights could be found in meta-data. Will try to recompute these sums.\n"
          "This only works on un-skimmed derivations, and the names of these weights may be "
          "unknown (but we'll try to read them from the PMG tool");
      m_recalc_LHE3 = true;
    }
  }

  if (m_config->isTruthDxAOD())
    m_recalculateNominalWeightSum = true;

  if (m_config->printCDIpathWarning()) {
    ATH_MSG_WARNING(
        "\n*************************************************************************\n"
        << "YOU ARE USING A CUSTOM PATH TO THE CDI FILE WHICH IS NOT THE DEFAULT PATH\n"
        << "       YOU MANY NOT BE USING THE LATEST BTAGGING RECOMMENDATIONS         \n"
        << "*************************************************************************\n\n");
  }
  if (m_config->printEgammaCalibModelWarning()) {
    ATH_MSG_WARNING(
        "\n*************************************************************************\n"
        << "          YOU HAVE CHANGED DEFAULT EGAMMA CALIBRATION MODEL             \n"
        << " TO USE DEFAULT MODEL, REMOVE 'EGammaCalibrationModel' FROM CONFIG FILE \n"
        << "*************************************************************************\n\n");
  }

  if (m_config->printEIDFileWarning()) {
    ATH_MSG_WARNING(
        "\n*************************************************************************\n"
        << "       YOU ARE USING THIS CUSTOM PATH TO THE ELECTRON ID SF FILE:        \n\n"
        << m_config->electronIDSFFilePath() << "(Tight) \n\n"
        << m_config->electronIDSFFileLoosePath() << "(Loose) \n\n"
        << "               INSTEAD OF THE MOST RECENT RECOMMENDED MAP                \n"
        << "       YOU MANY NOT BE USING THE LATEST ELECTRON ID RECOMMENDATIONS      \n"
        << "*************************************************************************\n\n");
  }

  return StatusCode::SUCCESS;
}

StatusCode TopxAODAlg::execute() {
  ++m_entry;
  ++m_totalYieldSoFar;

  // Pile up and MC event weight - used to normalize the cut flows
  float mcEventWeight(1.), pileupWeight(1.);
  if (m_config->isMC())
    mcEventWeight = m_topScaleFactors->mcEventWeight();

  if (m_config->doPileupReweighting() && !m_config->isTruthDxAOD()) {
    top::check(m_topScaleFactors->executePileup(), "Failed to execute pileup reweighting");
    pileupWeight = m_topScaleFactors->pileupWeight();
  }

  if (m_config->isMC()) {
    // if requested, pre-calculate TopPartons and PDF info
    // Run topPartonHistory
    if (m_config->doTopPartonHistory())
      top::check(m_topPartonHistory->execute(), "Failed to execute m_topPartonHistory");

    // calculate PDF weights
    if (m_config->doLHAPDF())
      top::check(m_PDF_SF->execute(), "Failed to execute PDF SF");
  }

  // perform any operation common to both reco and truth level
  // currently we load the MC generator weights and PDFs if requested
  m_eventSaver->execute();

  ///-- Truth events --///
  if (m_config->isMC()) {

    // Save, if requested, MC truth block, PDFInfo, TopPartons
    m_eventSaver->saveTruthEvent();
    if (m_config->doTopPartonLevel())
      ++m_eventSavedTruth;

    // Particle level analysis, saved only for truth events passing fiducial selection

    // --------------------------------------------------
    // If the truth loader is active, perform truth loading.
    if (m_particleLevelLoader->active()) {
      // --------------------------------------------------
      // Get the top::TruthEvent for the current event
      top::ParticleLevelEvent particleLevelEvent = m_particleLevelLoader->load();

      // event selection
      const bool saveEventInOutputFile =
          m_eventSelectionManager->applyParticleLevel(particleLevelEvent);

      if (saveEventInOutputFile) {
        m_eventSaver->saveParticleLevelEvent(particleLevelEvent);
        ++m_eventSavedParticle;
      }
    }
  }
  if (m_totalYieldSoFar == 0 && m_config->isMC() && m_config->doLHAPDF()) {
    const xAOD::TruthEventContainer *truthEvent(nullptr);
    top::check(evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()),
               "Failed to retrieve TruthEvent container for LHAPDF");
    top::check(truthEvent->size() == 1,
               "TruthEvent container size != 1, not sure what to do with PDF reweighting");
    for (auto tePtr : *truthEvent) {
      for (auto &pdf : m_totalEventsPdfWeighted) {
        if (tePtr->isAvailable<std::vector<float>>("AnalysisTop_" + pdf.first + "_Weights")) {
          pdf.second->resize(
              tePtr->auxdata<std::vector<float>>("AnalysisTop_" + pdf.first + "_Weights").size());
        }
      }
    }
  }
  // on the first event, set the size of the vector of sum of LHE3 weights in case it needs to be
  // calculated on the fly

  if (m_config->isMC()) {
    const xAOD::EventInfo *ei(nullptr);
    top::check(evtStore()->retrieve(ei, m_config->sgKeyEventInfo()),
               "Failed to retrieve LHE3 weights from EventInfo");

    if (m_recalculateNominalWeightSum) {
      if (m_totalYieldSoFar == 0)
        ATH_MSG_INFO("Trying to recalculate nominal weights sum for TRUTH derivation");
      const size_t nominalWeightIndex = m_config->nominalWeightIndex();
      m_totalEventsWeighted_temp += ei->mcEventWeights().at(nominalWeightIndex);
      m_totalEvents++;
    }

    if (m_config->doMCGeneratorWeights()) {
      unsigned int weightsSize = ei->mcEventWeights().size();
      if (m_recalc_LHE3) {
        if (m_totalYieldSoFar == 0) {
          m_totalEventsWeighted_LHE3_temp.resize(weightsSize);
          for (unsigned int i_LHE3 = 0; i_LHE3 < weightsSize; i_LHE3++) {
            m_totalEventsWeighted_LHE3_temp.at(i_LHE3) = ei->mcEventWeights().at(i_LHE3);
          }
          m_names_LHE3.resize(weightsSize);

          const std::vector<std::string> &weight_names = m_pmg_weightTool->getWeightNames();
          if (weight_names.size() != weightsSize) {
            ATH_MSG_INFO(
                "In top-xaod, while calculating mc weights sums on the fly, names from PMG tools "
                "have different size wrt weight vector, we'll not retrieve weight names");
            std::fill(m_names_LHE3.begin(), m_names_LHE3.end(), "?");
          } else {
            for (unsigned int i_wgt = 0; i_wgt < weight_names.size(); i_wgt++)
              m_names_LHE3[i_wgt] = weight_names[i_wgt];
          }
        } else {
          for (unsigned int i_LHE3 = 0; i_LHE3 < weightsSize; i_LHE3++) {
            m_totalEventsWeighted_LHE3_temp.at(i_LHE3) =
                m_totalEventsWeighted_LHE3_temp.at(i_LHE3) + ei->mcEventWeights().at(i_LHE3);
          }
        }
        // if recalc sum of weights not requested, we still cross-check the size of the vectors
      } else if (weightsSize != m_names_LHE3.size()) {
        ATH_MSG_ERROR("Strange size inconsistency in the AllExecutedEvents* "
                      "sum of weights  bookkeepers from the meta-data and the vector of "
                      "LHE3 weights in the EventInfo container.");
        return StatusCode::FAILURE;
      }
    }
  }
  ///-- End of Truth events -- start of reconstruction level events --///
  if (m_config->isTruthDxAOD())
    return StatusCode::SUCCESS;

  ///-- We veto events that do not pass the GRL, trigger or have bad calorimeters --///
  ///-- No point calibrating and cutting on events that fail these cuts --///
  ///-- We only apply a veto if ALL selectors request a cut --///
  ///-- And if the user selects "OutputEvents SelectedEvents" --///

  ///-- Count initial events --///
  m_eventSelectionManager->countInitial(mcEventWeight, pileupWeight);

  ///-- Does event pass the GRL? (always true for MC) --///
  ///-- Only veto events when ALL user selectors request GRL --///
  bool passGRLVeto = m_eventCleaning->applyGRL();
  if (!passGRLVeto) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  m_eventSelectionManager->countGRL(mcEventWeight, pileupWeight);

  ///-- Are the Tile and LAr calorimeters in a good state? (always true for MC) --///
  ///-- Only veto events when ALL user selectors request GOODCALO --///
  bool passGoodCalo = m_eventCleaning->applyGoodCalo();
  if (!passGoodCalo) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  m_eventSelectionManager->countGoodCalo(mcEventWeight, pileupWeight);

  ///-- Do we have a Primary Vertex? --///
  ///-- Only veto events when ALL user selectors request PRIVTX -- ///
  bool passPriVtx = m_eventCleaning->applyPrimaryVertex();
  if (!passPriVtx) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  m_eventSelectionManager->countPrimaryVertex(mcEventWeight, pileupWeight);

  ///-- Wondering which triggers are available ??? --///
  ///-- Uncomment this line and get ready for a LOT of output --///
  // m_eventCleaning->printTriggers();

  ///-- Does event pass any trigger ? --///
  ///-- Only veto events when ALL user selectors request TRIGDEC --///
  ///-- And all trigger fail --///
  bool passAnyTriggerVeto = m_eventCleaning->applyTrigger();
  if (!passAnyTriggerVeto) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  ///-- Nominal objects -- ///
  ///-- Calibrate objects and make all required systematic copies --///
  top::check(m_systObjMaker->execute(true), "Failed to execute systObjMaker");

  ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later
  ///--///
  top::check(m_objectSelection->execute(true), "Failed to execute objectSelection");

  ///-- Recalculate MissingET based on object selection --///
  top::check(m_systObjMaker->recalculateMET(true), "Failed to recalculateMET with systObjMaker");

  ///-- Systematic objects -- ///
  ///-- Calibrate objects and make all required systematic copies --///
  top::check(m_systObjMaker->execute(false), "Failed to execute systObjMaker");

  ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later
  ///--///
  top::check(m_objectSelection->execute(false), "Failed to execute objectSelection");

  ///-- Recalculate MissingET based on object selection --///
  top::check(m_systObjMaker->recalculateMET(false), "Failed to recalculateMET with systObjMaker");

  ///-- Scale Factor calculation --///
  if (m_config->isMC())
    top::check(m_topScaleFactors->execute(), "Failed to calculate scale factors");

  if (m_config->doTightEvents()) {
    ///-- Loop over all systematics and make a "top::Event" for each --///
    const xAOD::SystematicEventContainer *allSystematics =
        m_topEventMaker->systematicEvents(m_config->sgKeyTopSystematicEvents());
    for (auto currentSystematic : *allSystematics) {
      if (!(currentSystematic->hashValue() == m_config->nominalHashValue() ||
            m_config->doTightSysts()))
        continue;

      ///-- Make a top::Event --///
      top::Event topEvent = m_topEventMaker->makeTopEvent(currentSystematic);
      ///-- Apply event selection --///
      const bool passAnyEventSelection =
          m_eventSelectionManager->apply(topEvent, *currentSystematic);
      // check if we are using actual mu for mc16d or mc16e
      if (m_isFirst && m_config->isMC()) {
        const int runNumber = topEvent.m_info->runNumber();
        if (runNumber >= 300000) {
          if ((!m_config->isAFII() && m_config->PileupActualMu_FS().size() == 0) ||
              (m_config->isAFII() && m_config->PileupActualMu_AF().size() == 0)) {
            ATH_MSG_WARNING(
                "\n********************************************************************************"
                "*******\nYou are running over mc16d or mc16e sample but you are not using actual "
                "mu reweighting!\nYou are strongly adviced to use it.\nCheck "
                "https://twiki.cern.ch/twiki/bin/view/AtlasProtected/"
                "TopxAODStartGuideR21#PRW_and_Lumicalc_files\n*************************************"
                "**************************************************\n");
          }
        }
      }
      currentSystematic->auxdecor<char>(m_config->passEventSelectionDecoration()) =
          passAnyEventSelection ? 1 : 0;
      topEvent.m_saveEvent = passAnyEventSelection;
      ///-- Save event - we defer to eventSaver the decision to write or not --///
      m_eventSaver->saveEvent(topEvent);

      ///-- For printout at the end of job, only count nominal events --///
      if (passAnyEventSelection && currentSystematic->hashValue() == m_config->nominalHashValue())
        ++m_eventSavedReco;
    }
    ///-- End loop over all calibration systematics --///
  }
  ///-- End of tight events --///

  if (m_config->doLooseEvents()) {
    ///-- Loop over all Loose systematics and make a "top::Event" for each --///
    const xAOD::SystematicEventContainer *allSystematicsLoose =
        m_topEventMaker->systematicEvents(m_config->sgKeyTopSystematicEventsLoose());
    for (auto currentSystematic : *allSystematicsLoose) {
      if (!(currentSystematic->hashValue() == m_config->nominalHashValue() ||
            m_config->doLooseSysts()))
        continue;

      ///-- Make a top::Event --///
      top::Event topEvent = m_topEventMaker->makeTopEvent(currentSystematic);
      ///-- Apply event selection --///
      const bool passAnyEventSelection =
          m_eventSelectionManager->apply(topEvent, *currentSystematic);

      // check if we are using actual mu for mc16d or mc16e
      if (m_isFirst && m_config->isMC()) {
        const int runNumber = topEvent.m_info->runNumber();
        if (runNumber >= 300000) {
          if ((!m_config->isAFII() && m_config->PileupActualMu_FS().size() == 0) ||
              (m_config->isAFII() && m_config->PileupActualMu_AF().size() == 0)) {
            ATH_MSG_WARNING(
                "\n********************************************************************************"
                "*******\nYou are running over mc16d or mc16e sample but you are not using actual "
                "mu reweighting!\nYou are strongly adviced to use it.\nCheck "
                "https://twiki.cern.ch/twiki/bin/view/AtlasProtected/"
                "TopxAODStartGuideR21#PRW_and_Lumicalc_files\n*************************************"
                "**************************************************\n");
          }
        }
      }
      currentSystematic->auxdecor<char>(m_config->passEventSelectionDecoration()) =
          passAnyEventSelection ? 1 : 0;
      topEvent.m_saveEvent = passAnyEventSelection;
      ///-- weights for matrix-method fakes estimate from IFF tools, only for nominal --///
      if (!m_config->isMC() && m_config->doFakesMMWeightsIFF() &&
          currentSystematic->hashValue() == m_config->nominalHashValue()) {
        xAOD::IParticleContainer lepton(SG::VIEW_ELEMENTS);
        for (xAOD::Electron *t : topEvent.m_electrons)
          lepton.push_back(static_cast<xAOD::Electron *>(t));
        for (xAOD::Muon *t : topEvent.m_muons)
          lepton.push_back(static_cast<xAOD::Muon *>(t));

        // TODO implement asymptotic matrix method
      }
      ///-- Save event - we defer to eventSaver the decision to write or not --///
      m_eventSaver->saveEvent(topEvent);

      ///-- For printout at the end of job, only count nominal events --///
      if (passAnyEventSelection && currentSystematic->hashValue() == m_config->nominalHashValue())
        ++m_eventSavedRecoLoose;
    }
    ///-- End loop over all loose calibration systematics --///
  }
  ///-- End of loose events --///

  ///-- Needed for xAOD output, all systematics go into the same TTree --///
  m_eventSaver->saveEventToxAOD();
  m_isFirst = false;

  return StatusCode::SUCCESS;
}

StatusCode TopxAODAlg::endInputFile() {

  // TODO this calculation may be incorrect if EL is instructed to skip some events
  if (m_tracker)
    m_tracker->addInputFile(wk()->inputFile()->GetName(), m_entry);

  // do it at the end, so we can get the DS id from the first event
  // notice this is different from the normal sum of weights: all entries matter, not only the
  // highest per file
  if (m_config->doLHAPDF()) {
    bool pdfInCBK = false;
    const xAOD::CutBookkeeperContainer *cutBookKeepers = nullptr;
    if (inputMetaStore()->retrieve(cutBookKeepers, "CutBookkeepers")) {
      // try to get the first entry
      std::string pdf_set = m_totalEventsPdfWeighted.begin()->first;
      std::string p = pdf_set + "_0";
      for (const xAOD::CutBookkeeper *cbk : *cutBookKeepers) {
        std::string pdfName = cbk->name();
        if (p != pdfName)
          continue;
        m_pdfMetadataExists = true;
        pdfInCBK = true;
        // reload to set iterator back to original state
        if (!inputMetaStore()->retrieve(cutBookKeepers, "CutBookkeepers")) {
        }
        break;
      }
    }
    // if the pdf info was in the CBK the PDFSumOfWeights retrieve never happens,
    // and m_cutBookKeepers, will keep its reference to the main CBK
    if (!pdfInCBK) { // only try this if the info is not in the CBK -- this is legacy stuff
      if (!inputMetaStore()->retrieve(cutBookKeepers, "PDFSumOfWeights")) {
        m_pdfMetadataExists = false;
      }
    }
    if (m_pdfMetadataExists && m_config->saveLHAPDFEvent() && m_config->baseLHAPDF().empty()) {
      // just make a copy of it, but not all derivations have it, so just ignore it if it is not
      // there
      for (auto &pdfentry : m_totalEventsPdfWeighted) {
        std::string pdf_set = pdfentry.first;
        for (size_t n = 0; n < m_totalEventsPdfWeighted[pdf_set]->size(); ++n) {
          std::string p = pdf_set + "_" + std::to_string(n);
          bool foundPdf = false;
          for (const xAOD::CutBookkeeper *cbk : *cutBookKeepers) {
            std::string pdfName = cbk->name();
            if (p != pdfName)
              continue;
            m_totalEventsPdfWeighted[pdf_set]->at(n) = cbk->sumOfEventWeights();
            foundPdf = true;
            break;
          }
          if (!foundPdf) { // asked for PDF weighting, the PDF metadata exists, but this particular
                           // PDF is missing
            // crash hard
            ATH_MSG_ERROR("The force is not strong with us, young Padawan ...\n"
                          "You want to save weights to do PDF reweighting using '"
                          << pdf_set << "', which I figure has "
                          << m_totalEventsPdfWeighted[pdf_set]->size()
                          << " sets on it.\n"
                             "There is metadata information for the sum of MC*PDF weights in "
                             "PDFSumOfWeights, "
                             "but none seem to refer to '"
                          << p
                          << "' therefore I do not know how to estimate "
                             "the sum of weights before acceptance for this configuration.\n"
                             "May the force be with you in your next attempt.");
            return StatusCode::FAILURE;
          }
        }
      }
      m_sumPdfWeights->Fill();
    }
  } // doLHAPDF

  return StatusCode::SUCCESS;
}

StatusCode TopxAODAlg::finalize() {

  if (m_doPerfStats > 0)
    xAOD::PerfStats::instance().stop(); // Stop the PerfStats timer

  if (m_config->doLHAPDF())
    top::check(m_PDF_SF->finalize(), "Failed to finalize PDF SF");

  // this fills the sum of weights
  // if the list of input files has more than
  // one type of dataset (ie: the first file is ttbar and the second one
  // is W+jets), then the Fill statement below needs to come in the end of
  // each file's processing instead ... but why would you do that?!
  if (m_recalc_LHE3) { // in case the sum of LHE3 weight has been re-calculated with double (but we
                       // need floats in the end)
    for (double d : m_totalEventsWeighted_LHE3_temp)
      m_totalEventsWeighted_LHE3.push_back(d);
  }
  if (m_recalculateNominalWeightSum) {
    m_totalEventsWeighted = m_totalEventsWeighted_temp;
  }
  m_sumWeights->Fill();
  m_outputFile->cd();

  if (m_config->doLHAPDF()) {
    // Save sum of weights of PDF variations
    // Only do this if the metadata is not available
    if (m_totalEventsInFiles != m_totalEvents && !m_pdfMetadataExists) {
      ATH_MSG_ERROR("These are not the droids you are looking for.\n"
                    "You are running over skimmed derivations. We cannot determine "
                    "the sum of MC*PDF weights before skimming "
                    "because no PDF metadata is available in the file!");
      return StatusCode::FAILURE;
    }
    // save recomputed sum weights
    if ((!m_config->baseLHAPDF().empty() || !m_pdfMetadataExists)) {
      for (auto &pdf_result : m_config->getLHAPDFResults())
        *(m_totalEventsPdfWeighted[pdf_result.first]) = pdf_result.second;
      m_sumPdfWeights->Fill();
    }
  } // doLHAPDF

  m_outputFile->cd();
  m_eventSelectionManager->finalise();
  m_eventSaver->finalize();
  m_outputFile->cd();
  if (m_tracker)
    m_tracker->writeTree("AnalysisTracking");

  if (!m_config->isTruthDxAOD()) {
    if (m_config->doTightEvents())
      ATH_MSG_INFO("Events saved to output file nominal reconstruction tree: " << m_eventSavedReco);
    if (m_config->doLooseEvents())
      ATH_MSG_INFO("Events saved to output file nominal loose reconstruction tree: "
                   << m_eventSavedRecoLoose);
  }
  if (m_config->isMC()) {
    ATH_MSG_INFO("Events saved to output file truth tree : " << m_eventSavedTruth);
    if (m_particleLevelLoader->active()) {
      ATH_MSG_INFO("Events saved to output file particle level tree : " << m_eventSavedParticle);
    }
  }
  ATH_MSG_INFO("Total sum-of-weights (for normalization) : " << m_totalEventsWeighted);

  // print some xAOD performance info
  if (m_doPerfStats == 1)
    xAOD::IOStats::instance().stats().Print("Summary");
  if (m_doPerfStats == 2)
    xAOD::IOStats::instance().stats().Print();

  return StatusCode::SUCCESS;
}

} // namespace top
