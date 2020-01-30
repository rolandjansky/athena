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
    m_has_weight_metadata(false),
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

      top::check(m_pmg_truth_weight_tool.retrieve(), "Failed to retrieve PMGTruthWeightTool");
      top::check(initialize_nominal_MC_weight(), "Failed to initialize nominal MC weight in SF calculator");
    }

    if (m_config->doPileupReweighting()) {
      top::check(m_pileupSF->setProperty("config", m_config), "Failed to add config to pileup SF calculator");
      top::check(m_pileupSF->initialize(), "Failed to initialize pileup SF calculator");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode ScaleFactorCalculator::initialize_nominal_MC_weight() {
    ///-- Start using the PMG tool to get the nominal event weights --///
    // we will later need to know if this sample has any MC weight variations
    // some samples don't have any, and we have to fallback to taking 0th weight from EventInfo
    const std::vector<std::string>& pmg_weight_names = m_pmg_truth_weight_tool->getWeightNames();
    m_has_weight_metadata = (pmg_weight_names.size() > 0);
    if (!m_has_weight_metadata) {
      ATH_MSG_WARNING("PMGTruthWeightTool did not find any MC generator weights metadata for this sample.");
      ATH_MSG_WARNING("Will use 0th weight from the eventInfo.");
      m_config->setDetectedNominalWeightIndex(0);
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
    m_config->setDetectedNominalWeightName(m_nominal_weight_name);
    m_config->setDetectedNominalWeightIndex(weight_index - pmg_weight_names.begin());
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

    ATH_MSG_INFO("Using the following MC weight as nominal: " + m_config->detectedNominalWeightName());
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

    if (m_has_weight_metadata) {
      sf = m_pmg_truth_weight_tool->getWeight(m_nominal_weight_name);
      ///-- Decorate the event info with this weight and return --///
      eventInfo->auxdecor<float>("AnalysisTop_eventWeight") = sf;
      return sf;
    } else {
      sf = eventInfo->mcEventWeights()[0];
      eventInfo->auxdecor<float>("AnalysisTop_eventWeight") = sf;
      return sf;
    }
  }
}  // namespace top
