/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCorrections/ScaleFactorCalculator.h"

#include <string>

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEventInfo/EventInfo.h"

#include <sstream>

namespace top {
  ScaleFactorCalculator::ScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_photonSF(nullptr),
    m_electronSF(nullptr),
    m_fwdElectronSF(nullptr),
    m_muonSF(nullptr),
    m_tauSF(nullptr),
    m_jetSF(nullptr),
    m_btagSF(nullptr),
    m_pileupSF(nullptr),
    m_sherpa_22_reweight_tool("PMGSherpa22VJetsWeightTool"),
    m_globalLeptonTriggerSF(nullptr),
    m_pmg_truth_weight_tool("PMGTruthWeightTool"),
    m_sample_multiple_MCweights(false),
    m_nominal_weight_name("") {
    declareProperty("config", m_config);
  }

  StatusCode ScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::ScaleFactorCalculator initialize");

    m_photonSF = std::make_unique<top::PhotonScaleFactorCalculator>("top::PhotonScaleFactorCalculator");
    m_electronSF = std::make_unique<top::ElectronScaleFactorCalculator>("top::ElectronScaleFactorCalculator");
    m_fwdElectronSF = std::make_unique<top::FwdElectronScaleFactorCalculator>("top::FwdElectronScaleFactorCalculator");
    m_muonSF = std::make_unique<top::MuonScaleFactorCalculator>("top::MuonScaleFactorCalculator");
    m_tauSF = std::make_unique<top::TauScaleFactorCalculator>("top::TauScaleFactorCalculator");
    m_jetSF = std::make_unique<top::JetScaleFactorCalculator>("top::JetScaleFactorCalculator");
    m_btagSF = std::make_unique<top::BTagScaleFactorCalculator>("top::BTagScaleFactorCalculator");
    m_pileupSF = std::make_unique<top::PileupScaleFactorCalculator>("top::PileupScaleFactorCalculator");
    m_globalLeptonTriggerSF =
      std::make_unique<top::GlobalLeptonTriggerCalculator>("top::GlobalLeptonTriggerCalculator");

    if (m_config->isMC()) {
      if (m_config->usePhotons()) {
        top::check(m_photonSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_photonSF->initialize(), "Failed to initialize");
      }

      if (m_config->useElectrons()) {
        top::check(m_electronSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_electronSF->initialize(), "Failed to initialize");
      }

      if (m_config->useFwdElectrons()) {
        top::check(m_fwdElectronSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_fwdElectronSF->initialize(), "Failed to initialize");
      }

      if (m_config->useMuons() && !m_config->isTruthDxAOD()) {
        top::check(m_muonSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_muonSF->initialize(), "Failed to initialize");
      }

      if (m_config->useTaus()) {
        top::check(m_tauSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_tauSF->initialize(), "Failed to initialize");
      }

      if (m_config->useJets()) {
        top::check(m_jetSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_jetSF->initialize(), "Failed to initialize");

        top::check(m_btagSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_btagSF->initialize(), "Failed to initialize");
      }

      if (m_config->isSherpa22Vjets()) top::check(m_sherpa_22_reweight_tool.retrieve(),
                                                  "Failed to retrieve PMGSherpa22VJetsWeightTool");

      if ((m_config->useElectrons() || m_config->useMuons()) && m_config->useGlobalTrigger()) {
        top::check(m_globalLeptonTriggerSF->setProperty("config", m_config), "Failed to setProperty");
        top::check(m_globalLeptonTriggerSF->initialize(), "Failed to initalize");
      }

      top::check(initialize_nominal_MC_weight(), "Failed to initialize nominal MC weight in SF calculator");
    }

    if (m_config->doPileupReweighting()) {
      top::check(m_pileupSF->setProperty("config", m_config), "Failed to add config to pileup SF calculator");
      top::check(m_pileupSF->initialize(), "Failed to initialize pileup SF calculator");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode ScaleFactorCalculator::initialize_nominal_MC_weight() {
    // check if user force-requested to use plain MC weights vector index
    // in that case ignore all the additional checks if metadata broken
    // as well as the method to determine nominal weight by name
    if (m_config->forceNominalWeightFallbackIndex()) {
      ATH_MSG_WARNING("ForceNominalWeightFallbackIndex option was set to true."
          << "\nWill use weight with index: " << m_config->nominalWeightIndex()
          << " instead of determining it from metadata!");
      return StatusCode::SUCCESS;
    }
    ///-- Start using the PMG tool to get the nominal event weights --///
    // in case PMGTruthWeightTool init fails, e.g. due to broken metadata
    if (!m_pmg_truth_weight_tool.retrieve()) {
      // we don't know how many MC weights there are and user didn't specify nominal index
      if (m_config->MCweightsVectorSize() == size_t(-1)
          && m_config->nominalWeightIndex() == size_t(-1)) {
        ATH_MSG_ERROR("\nPMGTruthWeightTool instance could not be retrieved."
            << "We could not determine the number of MC generator weights in this sample.\n"
            << "We cannot determine the nominal MC weight. Please specify the index of "
            << "nominal MC weight via config option NominalWeightFallbackIndex.");
        return StatusCode::FAILURE;
      }
      // we know there are multiple MC weights and user didn't specify nominal index
      if (m_config->MCweightsVectorSize() > 1
          && m_config->nominalWeightIndex() == size_t(-1)) {
        ATH_MSG_ERROR("\nPMGTruthWeightTool instance could not be retrieved."
            << "We detect multiple MC generator weights in the sample.\n "
            << "We cannot determine which one is nominal. Please specify the index of "
            << "nominal MC weight via config option NominalWeightFallbackIndex.");
        return StatusCode::FAILURE;
      }
      // only one MC weight in the sample, this is sovable unambiguously
      if (m_config->MCweightsVectorSize() == 1) {
        m_config->setNominalWeightIndex(0);
        ATH_MSG_WARNING("PMGTruthWeightTool instance could not be retrieved."
            << "This sample has only one MC weight, will use that one.");
        return StatusCode::SUCCESS;
      }
      // possibly multiple weights, but the user already gave us fallback option
      ATH_MSG_WARNING("PMGTruthWeightTool instance could not be retrieved."
          << "Falling back to specified NominalWeightFallbackIndex "
          << m_config->nominalWeightIndex());
      return StatusCode::SUCCESS;
    }
    // PMGTruthWeightTool was initialized succesfully, let's see if we have weights
    const std::vector<std::string>& pmg_weight_names = m_pmg_truth_weight_tool->getWeightNames();
    m_sample_multiple_MCweights = (pmg_weight_names.size() > 1);
    if (!m_sample_multiple_MCweights) {
      ATH_MSG_WARNING("PMGTruthWeightTool did not find multiple MC generator weights in metadata for this sample."
          << "\nThis most likely means that the sample has only one weight."
          << "\nCheck the top-xaod output for PMGTruthWeightTool-related errors just to be sure.");
      if (m_config->nominalWeightIndex() != size_t(-1)) {
        ATH_MSG_WARNING("Using MC weight index " << m_config->nominalWeightIndex()
            << " as specified in config.");
      } else {
        ATH_MSG_WARNING("Will use 0th weight index. "
            << "If you want a different weight, specify it via config option NominalWeightFallbackIndex");
        m_config->setNominalWeightIndex(0);
      }
      return StatusCode::SUCCESS;
    }
    // here we try to find the first weight name from the NominalWeightNames config option, that matches any MC weight in the sample
    const std::vector<std::string> &nominal_weight_names = m_config->nominalWeightNames();
    bool found_match = false;
    std::vector<std::string> multiple_matches;
    for (const std::string& weight_name : nominal_weight_names) {
      // Check whether this weight name does exist
      if (m_pmg_truth_weight_tool->hasWeight(weight_name)) {
        // pick only the first match, but check if there are multiple matches -- that is a problem
        if (!found_match)
          m_nominal_weight_name = weight_name;
        found_match = true;
        multiple_matches.push_back(weight_name);
      }
    }
    // we have to find the index, because PMGTruthWeightTool has no method to give us the index
    auto weight_index = std::find(pmg_weight_names.begin(), pmg_weight_names.end(), m_nominal_weight_name);
    m_config->setNominalWeightName(m_nominal_weight_name);
    m_config->setNominalWeightIndex(weight_index - pmg_weight_names.begin());
    if (multiple_matches.size() > 1) {
      std::stringstream s_multiple_matches;
      for (const std::string &wname : multiple_matches)
        s_multiple_matches << "\"" << wname << "\"\n";
      ATH_MSG_WARNING("Multiple NominalWeightNames match for this MC sample!\n" + s_multiple_matches.str()
          + "\nThe one we will use is \"" + m_nominal_weight_name + "\". Check whether this is really correct!");
    }
    if (!found_match) {
      // if we get here, it means we are missing the correct name of the nominal weight
      // user has to find it in the sample meta-data and add it to AT config file
      ATH_MSG_ERROR("No MC weight matches any of the names specified by NominalWeightNames "
          "option\nThis may indicate a sample with non-standard nominal MC weight name!");
      std::stringstream weights_log;
      for (size_t w_indx=0; w_indx < pmg_weight_names.size(); ++w_indx) {
        weights_log << "Weight " << w_indx << ": \"" << pmg_weight_names.at(w_indx) << "\"\n";
      }
      ATH_MSG_ERROR("The following weight names are available based on sample metadata:\n" + weights_log.str()
          + "\nAdd the correct nominal weight name from this list into the NominalWeightNames option in your config file.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Using the following MC weight as nominal: \"" + m_config->nominalWeightName()
        << "\", index: " << m_config->nominalWeightIndex());
    return StatusCode::SUCCESS;
  }

  StatusCode ScaleFactorCalculator::execute() {
    if (m_config->isMC()) {
      if (m_config->usePhotons()) top::check(m_photonSF->execute(), "Failed to execute photon SF");
      if (m_config->useElectrons()) top::check(m_electronSF->execute(), "Failed to execute electron SF");
      if (m_config->useFwdElectrons()) top::check(m_fwdElectronSF->execute(), "Failed to execute fwd electron SF");
      if (m_config->useMuons() &&
          !m_config->isTruthDxAOD()) top::check(m_muonSF->execute(), "Failed to execute muon SF");
      if (m_config->useTaus()) top::check(m_tauSF->execute(), "Failed to execute tau SF");
      if (m_config->useJets()) top::check(m_jetSF->execute(), "Failed to execute jet SF");
      if (m_config->useJets()) {
        top::check(m_btagSF->execute(), "Failed to execute btag SF");
      }
      // Add Sherpa 22 weights directly here, if we get more
      // PMG tools for reweighting then we should consider making
      // a m_PMG_SF class, as with other corrections
      if (m_config->isSherpa22Vjets()) {
        const xAOD::EventInfo* eventInfo(nullptr);
        top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
        double sherpa_weight = m_sherpa_22_reweight_tool->getWeight();
        eventInfo->auxdecor<double>("Sherpa22VJetsWeight") = sherpa_weight;
      }
      if ((m_config->useElectrons() || m_config->useMuons()) && m_config->useGlobalTrigger()) {
        top::check(m_globalLeptonTriggerSF->execute(), "Failed to exectute global trigger SF");
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode ScaleFactorCalculator::executePileup() {
    if (m_config->doPileupReweighting()) top::check(m_pileupSF->execute(), "Failed to execute pileup SF");
    return StatusCode::SUCCESS;
  }

  float ScaleFactorCalculator::pileupWeight() const {
    float sf(1.);

    if (!m_config->isMC()) {
      return sf;
    }

    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()),
               "Failed to retrieve EventInfo");

    if (eventInfo->isAvailable<float>("PileupWeight")) sf = eventInfo->auxdataConst<float>("PileupWeight");

    return sf;
  }

  float ScaleFactorCalculator::mcEventWeight() const {
    float sf(1.);

    if (!m_config->isMC()) {
      return sf;
    }
    // Decorate the updated nominal weight if appropriate - note this is called early in top-xaod
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");

    // Check if the decoration is already present, and return it if so
    if (eventInfo->isAvailable<float>("AnalysisTop_eventWeight")) return eventInfo->auxdataConst<float>(
        "AnalysisTop_eventWeight");

    try {
      sf = eventInfo->mcEventWeights().at(m_config->nominalWeightIndex());
      eventInfo->auxdecor<float>("AnalysisTop_eventWeight") = sf;
    } catch (std::out_of_range &e) {
      ATH_MSG_ERROR("MC weight specified by index " << m_config->nominalWeightIndex()
          << " does not exist: ");
      throw;
    }
    return sf;
  }
}  // namespace top
