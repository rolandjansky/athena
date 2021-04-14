/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopTriggerCPTools.h"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Trigger include(s):
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/MatchFromCompositeTool.h"
#include "TrigTauMatching/TrigTauMatching.h"
#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "PATCore/PATCoreEnums.h"
#include "xAODBase/ObjectType.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>

namespace top {
  TriggerCPTools::TriggerCPTools(const std::string& name) :
    asg::AsgTool(name) {
    declareProperty("config", m_config);

    declareProperty("TrigConfigTool", m_trigConfTool);
    declareProperty("TrigDecisionTool", m_trigDecisionTool);
    declareProperty("TrigMatchTool", m_trigMatchTool);
    declareProperty("GlobalTriggerEffTool", m_globalTriggerEffTool);
    declareProperty("GlobalTriggerEffToolLoose", m_globalTriggerEffToolLoose);

    m_muonTool = asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors/MuonTrigEff");
    m_muonToolLoose = asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors/MuonTrigEffLoose");
  }

  StatusCode TriggerCPTools::initialize() {
    ATH_MSG_INFO("top::TriggerCPTools initialize...");

    if (m_config->makeAllCPTools()) {
      if (!m_config->isTruthDxAOD()) {
        ///-- The configuration tool --///
        const std::string trig_config_name = "TrigConf::xAODConfigTool";
        if (asg::ToolStore::contains<TrigConf::ITrigConfigTool>(trig_config_name)) {
          m_trigConfTool = asg::ToolStore::get<TrigConf::ITrigConfigTool>(trig_config_name);
        } else {
          TrigConf::xAODConfigTool* configTool = new TrigConf::xAODConfigTool(trig_config_name);
          top::check(configTool->initialize(), "Failed to initialize trigger config tool");
          m_trigConfTool = configTool;
        }

        ///-- The decision tool --///
        const std::string trig_decision_name = "Trig::TrigDecisionTool";
        if (asg::ToolStore::contains<Trig::TrigDecisionTool>(trig_decision_name)) {
          m_trigDecisionTool = asg::ToolStore::get<Trig::TrigDecisionTool>(trig_decision_name);
        } else {
          Trig::TrigDecisionTool* trigDecTool = new Trig::TrigDecisionTool(trig_decision_name);
          top::check(trigDecTool->setProperty("ConfigTool", m_trigConfTool), "Failed to setProperty");
          top::check(trigDecTool->setProperty("TrigDecisionKey", "xTrigDecision"), "Failed to setProperty");
          top::check(trigDecTool->initialize(), "Failed to initialize trigger decision tool");
          m_trigDecisionTool = trigDecTool;
        }

        // Trigger matching tool
        static const std::string trig_match_name = "Trig::MatchingTool";
        if (asg::ToolStore::contains<Trig::IMatchingTool>(trig_match_name)) {
          m_trigMatchTool = asg::ToolStore::get<Trig::IMatchingTool>(trig_match_name);
        } else {
          if (m_config->getDerivationStream() == "PHYS") {
            Trig::MatchFromCompositeTool* trigMatchTool = new Trig::MatchFromCompositeTool(trig_match_name);
            top::check(trigMatchTool->initialize(),
                       "Failed to initialize trig. matching tool");
            m_trigMatchTool = trigMatchTool;

          } else {
            Trig::MatchingTool* trigMatchTool = new Trig::MatchingTool(trig_match_name);
            top::check(trigMatchTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                       "Failed to set trigger decision tool to trigger matching tool");
            top::check(trigMatchTool->initialize(),
                       "Failed to initialize trig. matching tool");
            m_trigMatchTool = trigMatchTool;
          }
        }

        ///-- Tau matching --///
        if (m_config->useTaus()) {
          const std::string tauMatchName = "Trig::TrigTauMatchingTool";
          if (asg::ToolStore::contains<Trig::ITrigTauMatchingTool>(tauMatchName)) {
            m_trigMatchTauTool = asg::ToolStore::get<Trig::ITrigTauMatchingTool>(tauMatchName);
          } else {
            Trig::TrigTauMatchingTool* trigMatchTauTool = new Trig::TrigTauMatchingTool(tauMatchName);
            top::check(trigMatchTauTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                       "Failed to set trig dec tool for tau matching");
            top::check(trigMatchTauTool->initialize(),
                       "Failed to initialize tau trigger matching tool");
            m_trigMatchTauTool = trigMatchTauTool;
          }
        }
        ///-- Trigger global efficiency corrections --///
        if (m_config->useGlobalTrigger()) {
          top::check(this->initialiseGlobalTriggerEff(),
                     "Failed to construct global trigger efficiencies");
        }
      } else {
        ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything on truth DxAOD");
      }
    } else {
      ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything in mini-xAOD mode");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TriggerCPTools::initialiseGlobalTriggerEff() {
    StatusCode statusCode = StatusCode::SUCCESS;

    // utilities for TrigGlobEffCorr::ImportData
    TrigGlobEffCorr::ImportData triggerData;
    top::check(triggerData.importTriggers(), "failed to import trigger data");
    auto const& triggerDict = triggerData.getDictionary();
    std::unordered_map<std::string, TrigGlobEffCorr::ImportData::TrigDef> triggerDefs;
    for (auto&& kv : triggerData.getTriggerDefs()) {
      auto it = triggerDict.find(kv.first);
      if (it != triggerDict.end()) {
        triggerDefs[it->second] = kv.second;
      }
    }
    auto getTriggerLegs =
      [&](std::unordered_map<std::string, std::vector<std::pair<std::string, int> > > const& triggerCombination,
          std::unordered_map<std::string, std::set<std::string> >& electronLegsByPeriod,
          std::unordered_map<std::string, std::set<std::string> >& photonLegsByPeriod) {
        for (auto&& kv : triggerCombination) {
          std::string const& period = kv.first;
          for (auto const& trigKey : kv.second) {
            auto triggerDefsIt = triggerDefs.find(trigKey.first);
            if (triggerDefsIt == triggerDefs.end()) {
              statusCode = StatusCode::FAILURE;
              ATH_MSG_ERROR("unrecognized trigger `" << trigKey.first << "'");
              continue;
            }
            auto const& trigDef = triggerDefsIt->second;
            for (auto const& leg : trigDef.leg) {
              if (!leg) continue;
              std::string const& legname = triggerDict.at(leg);
              bool ok = true;
              xAOD::Type::ObjectType legtype = triggerData.associatedLeptonFlavour(legname, ok);
              if (!ok) {
                statusCode = StatusCode::FAILURE;
                ATH_MSG_ERROR("could not determine object type for trigger leg `" << legname <<
                  "'");
                continue;
              }
              switch (legtype) {
              case xAOD::Type::Electron:
                electronLegsByPeriod[period].insert(legname);
                break;

              case xAOD::Type::Muon:
                break;

              case xAOD::Type::Photon:
                photonLegsByPeriod[period].insert(legname);
                break;

              default:
                statusCode = StatusCode::FAILURE;
                ATH_MSG_ERROR(
                  "trigger leg `" << legname << "' has unsupported object type `" << legtype << "'");
                continue;
              }
            }
          }
        }
      };

    // Get trigger strings from configuration
    std::map<std::string, std::string> triggerCombination, triggerCombinationLoose;
    std::vector<std::string> electronSystematics, muonSystematics, photonSystematics, electronToolNames, muonToolNames, photonToolNames;
    std::unordered_map<std::string, std::vector<std::pair<std::string, int> > > const emptymap;
    std::unordered_map<std::string, std::vector<std::pair<std::string, int> > > const&
    triggersByPeriod = (m_config->doTightEvents() ? m_config->getGlobalTriggers() : emptymap),
      triggersByPeriodLoose = (m_config->doLooseEvents() ? m_config->getGlobalTriggersLoose() : emptymap);

    std::unordered_map<std::string, std::set<std::string> > electronLegsByPeriod, electronLegsByPeriodLoose, photonLegsByPeriod, photonLegsByPeriodLoose;
    getTriggerLegs(triggersByPeriod, electronLegsByPeriod, photonLegsByPeriod);
    getTriggerLegs(triggersByPeriodLoose, electronLegsByPeriodLoose, photonLegsByPeriodLoose);

    // Get quality
    std::string electronID, electronIDLoose, electronIsolation, electronIsolationLoose, muonQuality, muonQualityLoose;
    std::string photonIso, photonIsoLoose;
    if (m_config->doTightEvents()) {
      electronID = m_config->electronID();
      electronIsolation = m_config->electronIsolationSF();
      muonQuality = m_config->muonQuality();
      photonIso = m_config->photonIsolation();
    }
    if (m_config->doLooseEvents()) {
      electronIDLoose = m_config->electronIDLoose();
      electronIsolationLoose = m_config->electronIsolationSFLoose();
      muonQualityLoose = m_config->muonQualityLoose();
      photonIsoLoose = m_config->photonIsolationLoose();
    }

    // Tidy name for e/gamma
    electronID = mapWorkingPoints(electronID);
    electronIDLoose = mapWorkingPoints(electronIDLoose);
    // This is hopefully only temporary
    electronIsolation = mapWorkingPoints(electronIsolation);
    electronIsolationLoose = mapWorkingPoints(electronIsolationLoose);

    // Create electron trigger SF and Eff tools
    ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools, electronSFTools, electronEffToolsLoose,
                                                          electronSFToolsLoose;
    std::map<std::string, std::string> legsPerTool, legsPerToolLoose;
    int nTools = 0;

    // Loop over the triggers found (electrons - tight)
    for (auto& y_t : electronLegsByPeriod) {
      std::string year = y_t.first;
      for (auto& trigKey : y_t.second) {
        nTools++;
        for (int j = 0; j < 2; ++j) { // one tool instance for efficiencies, another for scale factors
          ATH_MSG_INFO("TIGHT " << year << " " << trigKey << " " << electronID << " " << electronIsolation);
          auto t = m_electronToolsFactory.emplace(
            m_electronToolsFactory.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEff_" + std::to_string(
              j) + "_" + std::to_string(nTools));
          top::check(t->setProperty("MapFilePath",
                                    "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt"),
                     "Fail");
          top::check(t->setProperty("TriggerKey",
                                    (j ? year + "_" + trigKey : "Eff_" + year + "_" + trigKey)),
                     "Failed to set TriggerKey");
          if (electronID != "None") top::check(t->setProperty("IdKey", electronID), "Failed to set IdKey");
          if (electronIsolation != "None") top::check(t->setProperty("IsoKey",
                                                                     electronIsolation), "Failed to set IsoKey");
          top::check(t->setProperty("CorrelationModel", "TOTAL"), "Failed to set CorrelationModel");
          top::check(t->setProperty("ForceDataType",
                                    (int) PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
          top::check(t->setProperty("OutputLevel", MSG::INFO), "Failed to set OutputLevel");
          top::check(t->initialize(), "Failed to initalise");
          // Using syntax from examples
          auto& handles = j ? electronSFTools : electronEffTools;
          handles.push_back(t->getHandle());
          std::string name = handles[handles.size() - 1].name();
          legsPerTool[name] = trigKey + " [" + year + "]";
          ATH_MSG_INFO("TIGHT " << name << " -> " << trigKey);
          electronToolNames.push_back(name);
          // Special - Record the systematic names from the efficiency tool (not SF tool)
          if (electronSystematics.size() == 0 && j == 1) {
            for (auto& s :
                 handles[handles.size() - 1]->recommendedSystematics().getBaseNames()) electronSystematics.push_back(s);

          }
        }
      }
    }

    // Loop over the triggers found (electrons - loose)
    nTools = 0;
    for (auto& y_t : electronLegsByPeriodLoose) {
      std::string year = y_t.first;
      for (auto& trigKey : y_t.second) {
        nTools++;
        for (int j = 0; j < 2; ++j) { // one tool instance for efficiencies, another for scale factors
          ATH_MSG_INFO("LOOSE " << year << " " << trigKey << " " << electronIDLoose << " " << electronIsolationLoose);
          auto tLoose = m_electronToolsFactoryLoose.emplace(
            m_electronToolsFactoryLoose.end(), "AsgElectronEfficiencyCorrectionTool/ElTrigEffLoose_" + std::to_string(
              j) + "_" + std::to_string(nTools));
          top::check(tLoose->setProperty("MapFilePath",
                                         "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt"),
                     "Fail");
          top::check(tLoose->setProperty("TriggerKey",
                                         (j ? year + "_" + trigKey : "Eff_" + year + "_" + trigKey)),
                     "Failed to set TriggerKey");
          if (electronIDLoose !=
              "None") top::check(tLoose->setProperty("IdKey", electronIDLoose), "Failed to set IdKey");
          if (electronIsolationLoose != "None") top::check(tLoose->setProperty("IsoKey",
                                                                               electronIsolationLoose),
                                                           "Failed to set IsoKey");
          top::check(tLoose->setProperty("CorrelationModel", "TOTAL"), "Failed to set CorrelationModel");
          top::check(tLoose->setProperty("ForceDataType",
                                         (int) PATCore::ParticleDataType::Full), "Failed to set ForceDataType");
          top::check(tLoose->setProperty("OutputLevel", MSG::INFO), "Failed to set OutputLevel");
          top::check(tLoose->initialize(), "Failed to initalise");
          // Using syntax from examples
          auto& handlesLoose = j ? electronSFToolsLoose : electronEffToolsLoose;
          handlesLoose.push_back(tLoose->getHandle());
          std::string name = handlesLoose[handlesLoose.size() - 1].name();
          legsPerToolLoose[name] = trigKey + " [" + year + "]";
          ATH_MSG_INFO("LOOSE " << name << " -> " << trigKey);
          electronToolNames.push_back(name);
          // Special - Record the systematic names from the efficiency tool (not SF tool)
          if (electronSystematics.size() == 0 && j == 1) {
            for (auto& s :
                 handlesLoose[handlesLoose.size() -
                              1]->recommendedSystematics().getBaseNames()) electronSystematics.push_back(s);
          }
        }
      }
    }

    // Create muon trigger SF tool
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools;
    ToolHandleArray<CP::IMuonTriggerScaleFactors> muonToolsLoose;

    if (m_config->doTightEvents()) {
      if (muonQuality != "None") top::check(m_muonTool.setProperty("MuonQuality",
                                                                   muonQuality), "Failed to set MuonQuality");
      top::check(m_muonTool.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF");
      top::check(m_muonTool.initialize(), "Failed to initialise");
      muonTools.push_back(m_muonTool.getHandle());
      ATH_MSG_INFO("Muon tool name (tight) " << muonTools[muonTools.size() - 1].name());
      muonToolNames.push_back(muonTools[muonTools.size() - 1].name());
      // Special - Get muon systematics
      if (muonSystematics.size() == 0) {
        for (auto& s:
        muonTools[muonTools.size() - 1]->recommendedSystematics().getBaseNames()) muonSystematics.push_back(s);
      }
    }

    if (m_config->doLooseEvents()) {
      if (muonQualityLoose != "None") top::check(m_muonToolLoose.setProperty("MuonQuality",
                                                                             muonQualityLoose),
        "Failed to set MuonQuality");
      top::check(m_muonToolLoose.setProperty("AllowZeroSF", true), "Failed to set AllowZeroSF");
      top::check(m_muonToolLoose.initialize(), "Failed to initialise");
      muonToolsLoose.push_back(m_muonToolLoose.getHandle());
      ATH_MSG_INFO("Muon tool name (loose) " << muonToolsLoose[muonToolsLoose.size() - 1].name());
      muonToolNames.push_back(muonToolsLoose[muonToolsLoose.size() - 1].name());
      // Special - Get muon systematics
      if (muonSystematics.size() == 0) {
        for (auto& s: muonToolsLoose[muonToolsLoose.size() - 1]->recommendedSystematics().getBaseNames()) muonSystematics.push_back(s);
      }
    }

    // Setup photon tools
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonEffTools;
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonSFTools;
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonEffToolsLoose;
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonSFToolsLoose;

    const std::string photonKey      = PhotonKeys(triggersByPeriod);
    const std::string photonKeyLoose = PhotonKeys(triggersByPeriodLoose);

    static const std::string mapPath = "PhotonEfficiencyCorrection/2015_2018/rel21.2/Summer2020_Rec_v1/map2.txt";
    if (photonKey != "" || photonKeyLoose != "") {
      if (m_config->doTightEvents()) {
        nTools = 0;
        top::check(CheckPhotonIsolation(photonIso), "Unsupported photon isolation for photon triggers provided: " + photonIso);
        for (auto& y_t : photonLegsByPeriod) {
          const std::string year = y_t.first;
          for (auto& trigKey : y_t.second) {
            nTools++;
            for(int j=0;j<2;++j) { /// two instances: 0 -> MC efficiencies, 1 -> SFs
              const std::string nameTool = "AsgPhotonEfficiencyCorrectionTool/" + std::string(j? "PhTrigEff" : "PhTrigSF") + "_" + std::to_string(nTools);
              auto t = m_photonToolsFactory.emplace(m_photonToolsFactory.end(), nameTool);
              top::check(t->setProperty("MapFilePath", mapPath.c_str()), "Cannot set MapFilePath");
              top::check(t->setProperty("TriggerKey", std::string(j ? "" : "Eff_") + photonKey), "Cannot set TriggerKey");
              top::check(t->setProperty("IsoKey", photonIso), "Cannot set IsoKey");
              top::check(t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Cannot set ForceDataType");
              top::check(t->setProperty("OutputLevel", MSG::ERROR), "Cannot set OutputLevel");
              top::check(t->initialize(), "Cannot initialise the photon tools");
              auto& photonHandles = (j? photonSFTools : photonEffTools);
              photonHandles.push_back(t->getHandle());
              const std::string name = photonHandles.back().name();
              legsPerTool[name] = trigKey + " [" + year + "]";
              photonToolNames.push_back(name);
              if (photonSystematics.size() == 0 && j == 1) {
                for (const auto& s : photonHandles.back()->recommendedSystematics().getBaseNames()) {
                  photonSystematics.push_back(s);    
                }
              }
            }
          }
        }
      }
      
      if (m_config->doLooseEvents()) {
        nTools = 0;
        top::check(CheckPhotonIsolation(photonIsoLoose), "Unsupported photon isolation for photon triggers provided: " + photonIsoLoose);
        for (auto& y_t : photonLegsByPeriodLoose) {
          const std::string year = y_t.first;
          for (auto& trigKey : y_t.second) {
            nTools++;
            for(int j=0;j<2;++j) { /// two instances: 0 -> MC efficiencies, 1 -> SFs
              const std::string nameTool = "AsgPhotonEfficiencyCorrectionTool/" + std::string(j? "PhTrigEff" : "PhTrigSF") + "_" + std::to_string(nTools);
              auto tLoose = m_photonToolsFactoryLoose.emplace(m_photonToolsFactoryLoose.end(), nameTool);
              top::check(tLoose->setProperty("MapFilePath", mapPath.c_str()), "Cannot set MapFilePath");
              top::check(tLoose->setProperty("TriggerKey", std::string(j ? "" : "Eff_") + photonKeyLoose), "Cannot set TriggerKey");
              top::check(tLoose->setProperty("IsoKey", photonIsoLoose), "Cannot set IsoKey");
              top::check(tLoose->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full), "Cannot set ForceDataType");
              top::check(tLoose->setProperty("OutputLevel", MSG::ERROR), "Cannot set OutputLevel");
              top::check(tLoose->initialize(), "Cannot initialise the photon tools");
              auto& photonHandlesLoose = (j? photonSFToolsLoose : photonEffToolsLoose);
              photonHandlesLoose.push_back(tLoose->getHandle());
              const std::string name = photonHandlesLoose.back().name();
              legsPerToolLoose[name] = trigKey + " [" + year + "]";
              photonToolNames.push_back(name);
              if (photonSystematics.size() == 0 && j == 1) {
                for (const auto& s : photonHandlesLoose.back()->recommendedSystematics().getBaseNames()) {
                  photonSystematics.push_back(s);    
                }
              }
            }
          }
        }
      }
    }

    auto combineStrings = [](const std::vector<std::pair<std::string, int> >& input ) {
      std::vector<std::string> tmp;
      for (const auto& i : input) {
        tmp.push_back(i.first);
      }
      return boost::algorithm::join(tmp, " || ");
    };

    for (auto& key : triggersByPeriod) {
      if (triggerCombination.find(key.first) == triggerCombination.end()) {
        triggerCombination[key.first] = "";
      } else {
        triggerCombination[key.first] += " || ";
      }
      triggerCombination[key.first] += combineStrings(key.second);
    }
    for (auto& key : triggersByPeriodLoose) {
      if (triggerCombinationLoose.find(key.first) == triggerCombinationLoose.end()) {
        triggerCombinationLoose[key.first] = "";
      } else {
        triggerCombinationLoose[key.first] += " || ";
      }
      triggerCombinationLoose[key.first] += combineStrings(key.second);
    }

    // Print out what we configured
    for (auto kv: triggerCombination) ATH_MSG_DEBUG("TRIG (TIGHT): " << kv.first << " -> " << kv.second);
    for (auto kv: triggerCombinationLoose) ATH_MSG_DEBUG("TRIG (LOOSE): " << kv.first << " -> " << kv.second);

    // Make the global trigger tool
    if (m_config->doTightEvents()) {
      TrigGlobalEfficiencyCorrectionTool* globalTriggerEffTool = new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobal");
      top::check(globalTriggerEffTool->setProperty("ElectronEfficiencyTools", electronEffTools), "Failed to attach electron efficiency tools");
      top::check(globalTriggerEffTool->setProperty("ElectronScaleFactorTools", electronSFTools), "Failed to attach electron scale factor tools");
      top::check(globalTriggerEffTool->setProperty("MuonTools", muonTools), "Failed to attach muon tools");
      top::check(globalTriggerEffTool->setProperty("PhotonEfficiencyTools", photonEffTools), "Failed to attach photon eff tools");
      top::check(globalTriggerEffTool->setProperty("PhotonScaleFactorTools", photonSFTools), "Failed to attach photon SF tools");
      top::check(globalTriggerEffTool->setProperty("ListOfLegsPerTool", legsPerTool), "Failed to define list of legs per tool");
      top::check(globalTriggerEffTool->setProperty("TriggerCombination", triggerCombination), "Failed to define trigger combination");
      top::check(globalTriggerEffTool->setProperty("TriggerMatchingTool", m_trigMatchTool), "Failed to set TriggerMatchingTool");
      // Setting MSG::ERROR to avoid flooding output with invalid efficiency warnings before event selection is complete
      top::check(globalTriggerEffTool->setProperty("OutputLevel", MSG::ERROR), "Failed to set message level");
      top::check(globalTriggerEffTool->initialize(), "Failed to initalise");
      m_globalTriggerEffTool = globalTriggerEffTool;
    }
    if (m_config->doLooseEvents()) {
      TrigGlobalEfficiencyCorrectionTool* globalTriggerEffToolLoose = new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose");
      top::check(globalTriggerEffToolLoose->setProperty("ElectronEfficiencyTools", electronEffToolsLoose), "Failed to attach electron efficiency tools");
      top::check(globalTriggerEffToolLoose->setProperty("ElectronScaleFactorTools", electronSFToolsLoose), "Failed to attach electron scale factor tools");
      top::check(globalTriggerEffToolLoose->setProperty("MuonTools", muonToolsLoose), "Failed to attach muon tools");
      top::check(globalTriggerEffToolLoose->setProperty("PhotonEfficiencyTools", photonEffToolsLoose), "Failed to attach photon eff tools");
      top::check(globalTriggerEffToolLoose->setProperty("PhotonScaleFactorTools", photonSFToolsLoose), "Failed to attach photon SF tools");
      top::check(globalTriggerEffToolLoose->setProperty("ListOfLegsPerTool", legsPerToolLoose), "Failed to define list of legs per tool");
      top::check(globalTriggerEffToolLoose->setProperty("TriggerCombination", triggerCombinationLoose), "Failed to define trigger combination");
      top::check(globalTriggerEffToolLoose->setProperty("TriggerMatchingTool", m_trigMatchTool), "Failed to set TriggerMatchingTool");
      // Setting MSG::ERROR to avoid flooding output with invalid efficiency warnings before event selection is complete
      top::check(globalTriggerEffToolLoose->setProperty("OutputLevel", MSG::ERROR), "Failed to set message level");
      top::check(globalTriggerEffToolLoose->initialize(), "Failed to initalise");
      m_globalTriggerEffToolLoose = globalTriggerEffToolLoose;
    }

    // Set information about systematics inside TopConfig
    m_config->setGlobalTriggerConfiguration(electronSystematics, muonSystematics, photonSystematics, electronToolNames, muonToolNames, photonToolNames);

    return statusCode;
  }

  std::string TriggerCPTools::mapWorkingPoints(const std::string& type) {
    std::string working_point = "";
    // ID
    if (type == "LooseAndBLayerLLH" || type == "LooseBLayer" || type == "LooseAndBLayerLH") {
      working_point = "LooseBLayer";
    }
    if (type == "MediumLLH" || type == "Medium" || type == "MediumLH") {
      working_point = "Medium";
    }
    if (type == "TightLLH" || type == "Tight" || type == "TightLH") {
      working_point = "Tight";
    }

    // Temporary ISO map to handle the mess that is EGamma+IFF right now...
    if (type.find("Pflow") != std::string::npos) {
      ATH_MSG_WARNING("You selected a Pflow isolation WP for at least one of your electron collections - BE WARNED THAT THESE ARE NOT YET READY TO BE RELEASED FOR USE IN PHYSICS ANALYSES AND OF COURSE DON'T HAVE ASSOCIATED SCALE FACTORS YET!!!");
      if (type == "PflowLoose") working_point = "FCLoose";
      if (type == "PflowTight") working_point = "FCTight";
    }
    if (type == "Tight") working_point = "FCTight";
    if (type == "Loose") working_point = "FCLoose";
    if (type == "HighPtCaloOnly") working_point = "FCHighPtCaloOnly";
    if (type == "TightTrackOnly") working_point = "Gradient";
    if (type == "TightTrackOnly_FixedRad") working_point = "Gradient";
    if (type == "FCTight" || type == "FCLoose" || type == "FCHighPtCaloOnly" || type == "Gradient") working_point = type;

    return working_point;
  }

  std::string TriggerCPTools::PhotonKeys(const std::unordered_map<std::string, std::vector<std::pair<std::string, int> > >& map) const {
    // check of the trigger names are one of the supported
    std::string result("");
    if (map.empty()) return result;

    const std::vector<std::pair<std::string,std::string> > supported = {{"2015","3g15_loose"},
                                                                        {"2016","3g20_loose"},
                                                                        {"2017","2g25_loose_g15_loose"},
                                                                        {"2018","2g25_loose_g15_loose"}};

    // check if the keys for each year match the supported photon trigger
    bool isPhoton(true);
    for (const auto& isupported : supported) {
      auto it = map.find(isupported.first);
      if (it == map.end()) continue;
      const std::vector<std::pair<std::string, int> > keys = it->second;
      if (std::find_if(keys.begin(), keys.end(), [&isupported](const std::pair<std::string, int>& pair){return isupported.second == pair.first;}) == keys.end()) {
        isPhoton = false;
      }
    }

    if (isPhoton) {
      result = "TRI_PH_2015_g15_loose_2016_g20_loose_2017_2018_g15_loose";
    }

    return result;
  }

  StatusCode TriggerCPTools::CheckPhotonIsolation(const std::string& isol) const {
    // only these isolations are available for photons triggers
    // we need to check for these as the code otherwise crashed 
    // with a meaningless error
    static const std::vector<std::string> allowedIso = {"TightCaloOnly", "Loose"};

    if (std::find(allowedIso.begin(), allowedIso.end(), isol) == allowedIso.end()) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
}  // namespace top
