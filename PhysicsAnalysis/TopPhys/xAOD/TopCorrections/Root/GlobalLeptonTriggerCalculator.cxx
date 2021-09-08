/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCorrections/GlobalLeptonTriggerCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEventInfo/EventInfo.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include <vector>


namespace top {
  GlobalLeptonTriggerCalculator::GlobalLeptonTriggerCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal"),
    m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose"),

    m_decor_triggerSF("AnalysisTop_Trigger_SF") {
    declareProperty("config", m_config);
  }

  StatusCode GlobalLeptonTriggerCalculator::initialize() {
    ATH_MSG_INFO(" top::GlobalLeptonTriggerCalculator initialize");

    if (m_config->doTightEvents()) top::check(m_globalTriggerSF.retrieve(),
                                              "Failed to retrieve global trigger SF tool");
    if (m_config->doLooseEvents()) top::check(
        m_globalTriggerSFLoose.retrieve(), "Failed to retrieve global trigger SF tool (loose)");

    // Retrieve the systematic names we stored
    for (auto& s : m_config->getGlobalTriggerElectronSystematics()) ATH_MSG_DEBUG(" - Electron systematics : " << s);
    for (auto& s : m_config->getGlobalTriggerMuonSystematics()) ATH_MSG_DEBUG(" - Muon systematics : " << s);
    for (auto& s : m_config->getGlobalTriggerPhotonSystematics()) ATH_MSG_DEBUG(" - Photon systematics : " << s);

    {
      std::vector<ToolHandle<IAsgElectronEfficiencyCorrectionTool> > tools;
      tools.reserve(m_config->getGlobalTriggerElectronTools().size());
      for (auto const& s : m_config->getGlobalTriggerElectronTools()) {
        ATH_MSG_DEBUG(" - Electron tools : " << s);
        tools.emplace_back(s);
        top::check(tools.back().retrieve(), "Failed to retrieve electron tool");
      }
      m_electronTools.swap(tools);
    }
    {
      std::vector<ToolHandle<CP::IMuonTriggerScaleFactors> > tools;
      tools.reserve(m_config->getGlobalTriggerMuonTools().size());
      for (auto const& s : m_config->getGlobalTriggerMuonTools()) {
        ATH_MSG_DEBUG(" - Muon tools : " << s);
        tools.emplace_back(s);
        top::check(tools.back().retrieve(), "Failed to retrieve muon tool");
      }
      m_muonTools.swap(tools);
    }
    {
      std::vector<ToolHandle<IAsgPhotonEfficiencyCorrectionTool> > tools;
      tools.reserve(m_config->getGlobalTriggerPhotonTools().size());
      for (auto const& s : m_config->getGlobalTriggerPhotonTools()) {
        ATH_MSG_DEBUG(" - Photon tools : " << s);
        tools.emplace_back(s);
        top::check(tools.back().retrieve(), "Failed to retrieve photon tool");
      }
      m_photonTools.swap(tools);
    }


    return StatusCode::SUCCESS;
  }

  StatusCode GlobalLeptonTriggerCalculator::execute() {
    ATH_MSG_DEBUG("Entered GlobalLeptonTriggerCalculator::execute");

    auto const& nominalHash = m_config->nominalHashValue();
    if (m_config->doTightEvents()) {
      xAOD::SystematicEventContainer const* systEvents = nullptr;
      top::check(evtStore()->retrieve(systEvents,
                                      m_config->sgKeyTopSystematicEvents()),
                 "Failed to get xAOD::SystematicEventContainer");
      for (xAOD::SystematicEvent* systEvent : *systEvents) {
        bool withScaleFactorVariations = (systEvent->hashValue() == nominalHash);
        processEvent(systEvent, withScaleFactorVariations);
      }
    }
    if (m_config->doLooseEvents()) {
      xAOD::SystematicEventContainer const* systEvents = nullptr;
      top::check(evtStore()->retrieve(systEvents,
                                      m_config->sgKeyTopSystematicEventsLoose()),
                 "Failed to get xAOD::SystematicEventContainer");
      for (xAOD::SystematicEvent* systEvent : *systEvents) {
        bool withScaleFactorVariations = (systEvent->hashValue() == nominalHash);
        processEvent(systEvent, withScaleFactorVariations);
      }
    }

    ATH_MSG_DEBUG("Leaving GlobalLeptonTriggerCalculator::execute");
    return StatusCode::SUCCESS;
  }

  void GlobalLeptonTriggerCalculator::processEvent(xAOD::SystematicEvent* systEvent, bool withScaleFactorVariations) {
    auto const& hash = systEvent->hashValue();
    bool isLoose = systEvent->isLooseEvent();

    std::string const& systname = m_config->systematicName(hash);
    ATH_MSG_DEBUG("Calculating trigger SF for variation " << systname << " (" << hash << ") with isLoose=" << isLoose);

    // Retrieve nominal muons, retrieve nominal electrons
    const xAOD::MuonContainer* muons(nullptr);
    const xAOD::ElectronContainer* electrons(nullptr);
    const xAOD::PhotonContainer* photons(nullptr);
    std::vector<const xAOD::Muon*> selectedMuons;
    std::vector<const xAOD::Electron*> selectedElectrons;
    std::vector<const xAOD::Photon*> selectedPhotons;
    
    if (m_config->useMuons()) top::check(evtStore()->retrieve(muons, m_config->sgKeyMuons(hash)), "Failed to retrieve muons");
    
    // Put into a vector
    for (size_t index : systEvent->goodMuons()) {
      selectedMuons.push_back(muons->at(index));
    }

    if (m_config->useElectrons()) top::check(evtStore()->retrieve(electrons, m_config->sgKeyElectrons(hash)), "Failed to retrieve electrons");
    // Put into a vector
    for (size_t index : systEvent->goodElectrons()) {
      selectedElectrons.push_back(electrons->at(index));
    }

    if (m_config->usePhotons()) top::check(evtStore()->retrieve(photons, m_config->sgKeyPhotons(hash)), "Failed to retrieve photons");
    // Put into a vector
    for (size_t index : systEvent->goodPhotons()) {
      selectedPhotons.push_back(photons->at(index));
    }

    // manage current electron trigger SF variation
    CP::SystematicSet electronSystSet;
    auto setCurrentElectronVariation = [this, &electronSystSet](std::string const& parameterName, int value) {
                                         electronSystSet.clear();
                                         if (!parameterName.empty()) electronSystSet.insert(CP::SystematicVariation(
                                                                                              parameterName, value));
                                         for (auto&& tool : m_electronTools) {
                                           top::check(tool->applySystematicVariation(
                                                        electronSystSet), "Failed to apply systematic");
                                         }
                                       };

    // manage current muon trigger SF variation
    CP::SystematicSet muonSystSet;
    auto setCurrentMuonVariation = [this, &muonSystSet](std::string const& parameterName, int value) {
                                     muonSystSet.clear();
                                     if (!parameterName.empty()) muonSystSet.insert(CP::SystematicVariation(
                                                                                      parameterName, value));
                                     for (auto&& tool : m_muonTools) {
                                       top::check(tool->applySystematicVariation(
                                                    muonSystSet), "Failed to apply systematic");
                                     }
                                   };

    // manage current photon trigger SF variation
    CP::SystematicSet photonSystSet;
    auto setCurrentPhotonVariation = [this, &photonSystSet](std::string const& parameterName, int value) {
                                       photonSystSet.clear();
                                       if (!parameterName.empty()) photonSystSet.insert(CP::SystematicVariation(
                                                                                        parameterName, value));
                                       for (auto&& tool : m_photonTools) {
                                         top::check(tool->applySystematicVariation(
                                                      photonSystSet), "Failed to apply systematic");
                                       }
                                     };

    // compute and store trigger SF for current variation
    auto&& globalTriggerTool = (isLoose ? m_globalTriggerSFLoose : m_globalTriggerSF);
    auto decorateEventForCurrentVariation = [&]() {
                                              double sf;

                                              if (selectedElectrons.empty() && selectedMuons.empty() && selectedPhotons.empty()) {
                                                sf = 1.;
                                              } else {
                                                sf = NAN;
                                                top::check(globalTriggerTool->getEfficiencyScaleFactor(
                                                             selectedElectrons,
                                                             selectedMuons,
                                                             selectedPhotons,
                                                             sf), "Failed to get global trigger SF");
                                              }
                                              std::string auxname(m_decor_triggerSF);
                                              auxname += "_";
                                              if (!(electronSystSet.empty() && muonSystSet.empty() && photonSystSet.empty())) {
                                                CP::SystematicSet systSet;
                                                systSet.insert(electronSystSet);
                                                systSet.insert(muonSystSet);
                                                systSet.insert(photonSystSet);
                                                auxname += systSet.name();
                                              }
                                              ATH_MSG_DEBUG("Adding decoration " << auxname << " = " << sf);
                                              systEvent->auxdecor<float>(auxname) = sf;
                                            };

    ///-- Set the nominal --///
    setCurrentElectronVariation("", 0);
    setCurrentMuonVariation("", 0);
    setCurrentPhotonVariation("", 0);
    decorateEventForCurrentVariation();

    ///-- Apply and calculate variations on nominal --///
    if (withScaleFactorVariations) {
      ATH_MSG_DEBUG("Calculating trigger SF variations");
      std::unordered_set<std::string> variations;
      ///-- Set nominal muon for electron systematic --///
      for (auto& s : m_config->getGlobalTriggerElectronSystematics()) {
        for (int val : {1, -1}) {
          setCurrentElectronVariation(s, val);
          decorateEventForCurrentVariation();
        }
      }
      ///-- Set nominal electron for muon systematic --///
      setCurrentElectronVariation("", 0);
      for (auto& s : m_config->getGlobalTriggerMuonSystematics()) {
        for (int val : {1, -1}) {
          setCurrentMuonVariation(s, val);
          decorateEventForCurrentVariation();
        }
      }
      setCurrentMuonVariation("", 0);

      for (auto& s : m_config->getGlobalTriggerPhotonSystematics()) {
        for (int val : {1, -1}) {
          setCurrentPhotonVariation(s, val);
          decorateEventForCurrentVariation();
        }
      }
      setCurrentPhotonVariation("", 0);
    }
  }
}
