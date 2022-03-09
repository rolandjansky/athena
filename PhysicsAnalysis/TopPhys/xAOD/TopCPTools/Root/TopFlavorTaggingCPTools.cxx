/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopFlavorTaggingCPTools.h"

#include <map>
#include <string>
#include <algorithm>
#include <iterator>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/Tokenize.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

namespace top {
  FlavorTaggingCPTools::FlavorTaggingCPTools(const std::string& name) :
    asg::AsgTool(name) {
    declareProperty("config", m_config);
  }

  StatusCode FlavorTaggingCPTools::initialize() {
    ATH_MSG_INFO("top::FlavorTaggingCPTools initialize...");

    if (m_config->isTruthDxAOD()) {
      ATH_MSG_INFO("top::FlavorTaggingCPTools: no need to initialise anything on truth DxAOD");
      return StatusCode::SUCCESS;
    }

    if (!m_config->useJets()) {
      ATH_MSG_INFO("top::FlavorTaggingCPTools: no need to initialise anything since not using jets");
      return StatusCode::SUCCESS;
    }

    static const std::string cdi_file_default =
      "xAODBTaggingEfficiency/13TeV/2021-22-13TeV-MC16-CDI-2021-12-02_v2.root";

    if (m_config->bTaggingCDIPath() != "Default") {
      if (m_config->bTaggingCDIPath() != cdi_file_default) {
        m_config->setPrintCDIpathWarning(true);
      }
      m_cdi_file = m_config->bTaggingCDIPath();
    } else {
      m_cdi_file = cdi_file_default;
    }
    // This ordering needs to match the indexing in TDP (for missing cases, we use default which gives a MC/MC of 1 as
    // its the same as the eff used in the calibration
    // Pythia6;Herwigpp;Pythia8;Sherpa(2.2);Sherpa(2.1);aMC@NLO+Pythia8;Herwig7.1.3;Sherpa228;Sherpa2210;Herwigpp721
    // Default changed from 410501 to 410470 in the CDI release of October 2018
    m_efficiency_maps = "default;410558;410470;410250;default;410464;411233;421152;700122;600666";

    const std::string caloJets_collection = m_config->sgKeyJets();
    const std::string trackJets_collection = m_config->sgKeyTrackJets();

    m_calib_file_path = PathResolverFindCalibFile(m_cdi_file);
    m_excluded_systs = m_config->bTagSystsExcludedFromEV() == "none" ? "" : m_config->bTagSystsExcludedFromEV();

    //------------------------------------------------------------
    // Loop through all the different working points we have and create a
    // BTaggingSelectionTool and corresponding BTaggingEfficiencyTool if the working point is calibrated.
    //------------------------------------------------------------

    // initialize selection tools, for both calibrated and uncalibrated WPs
    for (const auto& TaggerBtagWP : m_config->bTagAlgoWP()) {
      top::check(setupBtagSelectionTool(TaggerBtagWP, m_config->sgKeyJets(), m_config->jetPtcut(), m_config->jetEtacut()),
                 "Failed to initialize btag selection tool");
    }
    for (const auto& TaggerBtagWP : m_config->bTagAlgoWP_calib()) {
      top::check(setupBtagEfficiencyTool(TaggerBtagWP, m_config->sgKeyJets(), m_config->jetPtcut()),
                 "Failed to initialize btag selection tool");
    }

    if (m_config->useTrackJets()) {
      for (const auto& TaggerBtagWP : m_config->bTagAlgoWP_trkJet()) {
        top::check(setupBtagSelectionTool(TaggerBtagWP, m_config->sgKeyTrackJets(), m_config->trackJetPtcut(), m_config->trackJetEtacut(), true),
                   "Failed to initialize btag selection tool");
      }
      for (const auto& TaggerBtagWP : m_config->bTagAlgoWP_calib_trkJet()) {
        top::check(setupBtagEfficiencyTool(TaggerBtagWP, m_config->sgKeyTrackJets(), m_config->trackJetPtcut(), true),
                   "Failed to initialize btag selection tool");
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode FlavorTaggingCPTools::setupBtagSelectionTool(const std::pair<std::string, std::string>& btag_algo_WP,
                                                          const std::string& jetCollection,
                                                          double jetPtCut, double jetEtaCut,
                                                          bool trackJets) {
    const std::string bTagWPName = btag_algo_WP.first + "_" + btag_algo_WP.second;
    //------------------------------------------------------------
    // Setup BTaggingSelectionTool
    //------------------------------------------------------------
    std::string btagsel_tool_name = "BTaggingSelectionTool_" + bTagWPName + "_" + jetCollection;
    // due to a bug in the CDI files, track jets names are missing PV0 in the name
    const std::string jetAuthor = (trackJets ? erasePV0fromJetsName(jetCollection) : jetCollection);

    BTaggingSelectionTool* btagsel = new BTaggingSelectionTool(btagsel_tool_name);
    top::check(btagsel->setProperty("TaggerName", btag_algo_WP.first),
                "Failed to set b-tagging selecton tool TaggerName");
    top::check(btagsel->setProperty("JetAuthor", jetAuthor),
                "Failed to set b-tagging selection JetAuthor");
    top::check(btagsel->setProperty("FlvTagCutDefinitionsFileName", m_cdi_file),
                "Failed to set b-tagging selection tool CDI file");
    top::check(btagsel->setProperty("OperatingPoint", btag_algo_WP.second),
                "Failed to set b-tagging selection tool OperatingPoint");
    top::check(btagsel->setProperty("MinPt", jetPtCut),
                "Failed to set b-tagging selection tool MinPt");
    top::check(btagsel->setProperty("MaxEta", jetEtaCut),
                "Failed to set b-tagging selection tool MaxEta");
    top::check(btagsel->initialize(),
               "Failed to initialize b-tagging selection tool: " + btagsel_tool_name);
    m_btagging_selection_tools.push_back(btagsel);

    // for each algorithm (DL1r, DL1d, etc...) keep one selection tool instance for creating pb,pc,pu decorations
    // internally use map to make sure only one tool for each algorithm is stored
    m_config->addBTagAlgo(btag_algo_WP.first, btagsel_tool_name, trackJets);

    return StatusCode::SUCCESS;
  }

  StatusCode FlavorTaggingCPTools::setupBtagEfficiencyTool(const std::pair<std::string, std::string>& btag_algo_WP,
                                                          const std::string& jetCollection,
                                                          double jetPtCut,
                                                          bool trackJets) {
    const std::string bTagWPName = btag_algo_WP.first + "_" + btag_algo_WP.second;
    std::string btageff_tool_name = "BTaggingEfficiencyTool_" + bTagWPName + "_" + jetCollection;
    // due to a bug in the CDI files, track jets names are missing PV0 in the name
    const std::string jetAuthor = (trackJets ? erasePV0fromJetsName(jetCollection) : jetCollection);

    BTaggingEfficiencyTool* btageff = new BTaggingEfficiencyTool(btageff_tool_name);
    top::check(btageff->setProperty("TaggerName", btag_algo_WP.first),
                "Failed to set b-tagging TaggerName");
    top::check(btageff->setProperty("OperatingPoint", btag_algo_WP.second),
                "Failed to set b-tagging OperatingPoint");
    top::check(btageff->setProperty("JetAuthor", jetAuthor),
                "Failed to set b-tagging JetAuthor");
    top::check(btageff->setProperty("MinPt", jetPtCut),
                "Failed to set b-tagging selection tool MinPt");
    top::check(btageff->setProperty("EfficiencyFileName", m_calib_file_path),
                "Failed to set path to b-tagging CDI file");
    top::check(btageff->setProperty("ScaleFactorFileName", m_calib_file_path),
                "Failed to set path to b-tagging CDI file");
    top::check(btageff->setProperty("ScaleFactorBCalibration", m_config->bTaggingCalibration_B()),
                "Failed to set b-tagging calibration (B): " + m_config->bTaggingCalibration_B());
    top::check(btageff->setProperty("ScaleFactorCCalibration", m_config->bTaggingCalibration_C()),
                "Failed to set b-tagging calibration (C): " + m_config->bTaggingCalibration_C());
    // using same calibration for T as for C
    top::check(btageff->setProperty("ScaleFactorTCalibration", m_config->bTaggingCalibration_C()),
                "Failed to set b-tagging calibration (T): " + m_config->bTaggingCalibration_C());
    top::check(btageff->setProperty("ScaleFactorLightCalibration", m_config->bTaggingCalibration_Light()),
                "Failed to set b-tagging calibration (Light): " + m_config->bTaggingCalibration_Light());
    for (auto jet_flav : m_jet_flavors) {
      // 09/02/18 IC: The pseudo-continuous does not have MC/MC SF so we need to only apply default for this case
      // 08/05/18 Francesco La Ruffa: The pseudo-continuous has now its own MC/MC SFs, no needed to set default
      top::check(btageff->setProperty("Efficiency" + jet_flav + "Calibrations", m_efficiency_maps),
                  "Failed to set " + jet_flav + "-calibrations efficiency maps");
    }
    top::check(btageff->setProperty("ExcludeFromEigenVectorTreatment", m_excluded_systs),
               "Failed to set b-tagging systematics to exclude from EV treatment");
    top::check(btageff->initialize(), "Failed to initialize " + bTagWPName);
    // Check the excludedSysts - Cannot check before the tool is initialised
    top::check(this->checkExcludedSysts(btageff, m_excluded_systs),
               "Incorrect excluded systematics have been provided.");
    m_btagging_efficiency_tools.push_back(btageff);
    return StatusCode::SUCCESS;
  }

  std::string FlavorTaggingCPTools::erasePV0fromJetsName(std::string jetCollectionName) {
    const std::string pv0 = "PV0";
    auto it = jetCollectionName.find(pv0);
    if (it == std::string::npos) return jetCollectionName;
    jetCollectionName.erase(it, pv0.length());
    return jetCollectionName;
  }

  StatusCode FlavorTaggingCPTools::checkExcludedSysts(BTaggingEfficiencyTool* btageff, std::string excludedSysts) {
    // We pass the pointer to the btagging efficiency tool which is being created and also the excludedSysts string
    // which will be used
    // If the string is empty, then nothing to check
    if (excludedSysts == "") return StatusCode::SUCCESS;

    // Split by a semi-colon delimiter and then check the individual syst strings against the list from the CDI
    std::vector<std::string> listOfExcludedSysts;
    top::tokenize(excludedSysts, listOfExcludedSysts, ";");
    ATH_MSG_INFO(" ------------------------------------------------ ");
    ATH_MSG_INFO(" ------------- EXPERIMENTAL FEATURE ------------- ");
    ATH_MSG_INFO(" ------ Please provide feedback to TopReco ------ ");
    ATH_MSG_INFO(" ------------- EXPERIMENTAL FEATURE ------------- ");
    ATH_MSG_INFO(" ------------------------------------------------ ");
    ATH_MSG_INFO(" AnalysisTop - Checking excludedSysts for flavour tagging EV");
    ATH_MSG_INFO(" This has been split on the semi-colon delimiter to find...");
    for (auto s : listOfExcludedSysts) ATH_MSG_INFO("... " + s);
    // Get the map(string, vector<string>) from the CDI tool
    // Don't care about the flavours (this will be handled in the CDI)
    std::vector<std::string> listOfScaleFactorSystematics;
    for (auto flavour : btageff->listScaleFactorSystematics(false)) {
      for (auto sys : flavour.second) {
        listOfScaleFactorSystematics.push_back(sys);
      }
    }
    // Make this a unique set and then we need to check that all systematics provided by the user are expected by the
    // CDI
    std::set<std::string> setOfExcludedSysts, setOfScaleFactorSystematics;

    for (auto sys : listOfExcludedSysts) {
      if (setOfExcludedSysts.find(sys) == setOfExcludedSysts.end()) setOfExcludedSysts.insert(sys);
    }

    for (auto sys : listOfScaleFactorSystematics) {
      if (setOfScaleFactorSystematics.find(sys) == setOfScaleFactorSystematics.end()) setOfScaleFactorSystematics.insert(sys);
    }

    //
    std::vector<std::string> unionOfSystematics;
    std::set_intersection(setOfExcludedSysts.begin(), setOfExcludedSysts.end(),
                          setOfScaleFactorSystematics.begin(), setOfScaleFactorSystematics.end(),
                          std::back_inserter(unionOfSystematics));
    // Check we have the same systematics listed in unionOfSystematics
    if (unionOfSystematics.size() != listOfExcludedSysts.size()) {
      ATH_MSG_WARNING("Have not found all systematics listed to be excluded from b-tagging eigenvector method");
      ATH_MSG_INFO("Permitted values are...");
      for (auto sys : setOfScaleFactorSystematics) {
        ATH_MSG_INFO(" ... " + sys);
      }
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO(" Summary of EV impact ");
      for (auto sysRemove : listOfExcludedSysts) {
        std::string flavourAffected = "";
        for (auto flavour : btageff->listScaleFactorSystematics(false)) {
          for (auto sysCDI : flavour.second) {
            if (sysRemove == sysCDI) flavourAffected += flavour.first;
          }
        }
        ATH_MSG_INFO(" ... " + sysRemove + " -> Removed from calibration(s) : [" + flavourAffected + "]");
      }
      ATH_MSG_INFO(" These will be dynamically matched to systematic tree names (if available)");
      ATH_MSG_INFO(" All systematics are accepted by CDI file ");
    }
    // We have passed all the tests so now we store the systematics removed from the EV method and use a mapping to
    // ASG/AT naming and return
    ATH_MSG_INFO(" ------------------------------------------------ ");
    return StatusCode::SUCCESS;
  }


}  // namespace top
