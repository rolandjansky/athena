/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopBoostedTaggingCPTools.h"

#include <map>
#include <string>
#include <algorithm>
#include <iterator>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/Tokenize.h"
#include "TopEvent/EventTools.h"

// JetUncertaintiesTool for the tagger SFs
#include "JetUncertainties/JetUncertaintiesTool.h"

// Boosted tagging includes
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

namespace top {
  BoostedTaggingCPTools::BoostedTaggingCPTools(const std::string& name) :
    asg::AsgTool(name) {
    declareProperty("config", m_config);
  }

  StatusCode BoostedTaggingCPTools::initialize() {
    ATH_MSG_INFO("top::BoostedTaggingCPTools initialize...");

    if (m_config->isTruthDxAOD()) {
      ATH_MSG_INFO("top::BoostedTaggingCPTools: no need to initialise anything on truth DxAOD");
      return StatusCode::SUCCESS;
    }

    if (!m_config->useLargeRJets()) {
      ATH_MSG_INFO("top::BoostedTaggingCPTools: no need to initialise anything since not using large-R jets");
      return StatusCode::SUCCESS;
    }


    const std::vector<std::pair<std::string, std::string> >& boostedJetTaggers = m_config->boostedJetTaggers();
    if (boostedJetTaggers.size() == 0) {
      ATH_MSG_INFO("top::BoostedTaggingCPTools: boostedJetTagging not set. No need to initialise anything.");
      return StatusCode::SUCCESS;
    }

    initTaggersMaps();
    initSFsMaps();

    top::check(std::find(std::begin(m_jetCollections),std::end(m_jetCollections),m_config->sgKeyLargeRJets())!=std::end(m_jetCollections),
      "Error in BoostedTaggingCPTools: boosted jet taggers are not available for this large-R jet collection.");

    for (const std::pair<std::string, std::string>& name : boostedJetTaggers) {
      const std::string& taggerType = name.first;
      const std::string& taggerName = name.second;

      top::check(m_taggersConfigs.find(taggerName) != std::end(m_taggersConfigs),
                 ("Error in BoostedTaggingCPTools: Unknown tagger in the config file: " + taggerName).c_str());

      top::check(std::find(m_taggersTypes.begin(), m_taggersTypes.end(), taggerType) != m_taggersTypes.end(),
                 "Error in BoostedTaggingCPTools: Unknown TAGGER_TYPE.");
      if (taggerType == "JSSWTopTaggerDNN")
        top::check(ASG_MAKE_ANA_TOOL(m_taggers[taggerName], JSSWTopTaggerDNN), "Failed to make " + taggerName);
      else if (taggerType == "SmoothedWZTagger")
        top::check(ASG_MAKE_ANA_TOOL(m_taggers[taggerName], SmoothedWZTagger), "Failed to make " + taggerName);

      m_taggers[taggerName].setName(taggerName);
      top::check(m_taggers[taggerName].setProperty("ConfigFile", m_taggersConfigs[taggerName]),
                 "Failed to set ConfigFile for " + taggerName);
      top::check(m_taggers[taggerName].setProperty("ContainerName", m_config->sgKeyLargeRJets()),
                 "Failed to set ContainerName " + taggerName);
      top::check(m_taggers[taggerName].setProperty("CalibArea", m_taggersCalibAreas[taggerType]),
                 "Failed to set CalibArea for " + taggerName);
      // not all BJT taggers implement IsMC property -- only those that have calibration SFs
      // so we have to check here that we try to set this property only where applicable
      if (taggerType == "JSSWTopTaggerDNN" || taggerType == "SmoothedWZTagger") {
        top::check(m_taggers[taggerName].setProperty("IsMC", m_config->isMC()),
                   "Failed to set IsMC for " + taggerName);
      }
      top::check(m_taggers[taggerName].initialize(), "Failed to initialize " + taggerName);

      // initialize SF uncertainty tools for supported WPs
      if (m_config->isMC()) {
        std::string jet_def = m_config->sgKeyLargeRJets();
        jet_def.erase(jet_def.length() - 4); // jet collection name sans 'Jets' suffix

        const std::string name = "JetSFuncert_" + taggerName;
        try {
          const std::string& cfg = m_taggerSFsConfigs.at(taggerName);
          if (m_config->applyBoostedJetTaggersUncertainties()) {
            JetUncertaintiesTool* jet_SF_tmp = new JetUncertaintiesTool(name);

            top::check(jet_SF_tmp->setProperty("JetDefinition", jet_def), "Failed to set JetDefinition for " + name);
            top::check(jet_SF_tmp->setProperty("MCType", "MC16"), "Failed to set MCType for " + name);
            top::check(jet_SF_tmp->setProperty("ConfigFile", cfg), "Failed to set ConfigFile for " + name);
            top::check(jet_SF_tmp->setProperty("IsData", false), "Failed to set IsData for " + name);
            top::check(jet_SF_tmp->initialize(), "Failed to initialize " + name);
            m_tagSFuncertTool[taggerName] = jet_SF_tmp;
          }
          m_config->setCalibBoostedJetTagger(taggerName, m_taggerSFsNames[taggerName]);
        } catch (std::out_of_range& e) {
          // skip taggers which do not yet have SFs available
          ATH_MSG_WARNING("Boosted jet tagger " + taggerName + " is not yet calibrated! No SFs are available.");
        }
      }
    }

    return StatusCode::SUCCESS;
  }
  
  void BoostedTaggingCPTools::initTaggersMaps() {
    
    // Calib areas
    m_taggersCalibAreas["JSSWTopTaggerDNN"] = "JSSWTopTaggerDNN/Rel21/";
    m_taggersCalibAreas["SmoothedWZTagger"] = "SmoothedWZTaggers/Rel21/";
    
    // Supported tagger types
    m_taggersTypes = {
      "JSSWTopTaggerDNN", "SmoothedWZTagger"
    };
    
    // Supported jet collections
    m_jetCollections = {
      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
      "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"
    };
    
    
    auto setConfig = [this](const std::string& tagger, const std::string& configName) {
      m_taggersConfigs[tagger] = configName;
    };
    
    if (m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
      
      // Top taggers
      setConfig("DNNTaggerTopQuarkContained50","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16_20201216_50Eff.dat");
      setConfig("DNNTaggerTopQuarkContained80","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16_20201216_80Eff.dat");
      setConfig("DNNTaggerTopQuarkInclusive50","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16_20201216_50Eff.dat");
      setConfig("DNNTaggerTopQuarkInclusive80","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16_20201216_80Eff.dat");
      
      // WZ taggers
      setConfig("SmoothWContained50","SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat");
      setConfig("SmoothWContained80","SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat");
      setConfig("SmoothZContained50","SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat");
      setConfig("SmoothZContained80","SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat");

    } else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets") {
      // WZ taggers
      setConfig("SmoothWContained2VarMaxSig","SmoothedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat");
      setConfig("SmoothZContained2VarMaxSig","SmoothedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat");
      setConfig("SmoothW3VarMaxSig","SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
      setConfig("SmoothZ3VarMaxSig","SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
    }

  }
  
  
  
  void BoostedTaggingCPTools::initSFsMaps() {
    // Here we initialize SFs maps
    
    // This lambda function is universal for all taggers
    auto setMaps = [this](const std::string& configPath, const std::string& tagger, const std::string& configName) {
      m_taggerSFsConfigs[tagger] = configPath + "/" + configName;
      m_taggerSFsNames[tagger] = tagger+"_SF";
    };
    
    if (m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
      // Tagging scale factors availabel for AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
      
      const std::string configPath="rel21/Fall2020";
      //Top taggers
      setMaps(configPath,"DNNTaggerTopQuarkContained50","R10_SF_LCTopo_TopTagContained_SigEff50.config");
      setMaps(configPath,"DNNTaggerTopQuarkContained80","R10_SF_LCTopo_TopTagContained_SigEff80.config");
      setMaps(configPath,"DNNTaggerTopQuarkInclusive50","R10_SF_LCTopo_TopTagInclusive_SigEff50.config");
      setMaps(configPath,"DNNTaggerTopQuarkInclusive80","R10_SF_LCTopo_TopTagInclusive_SigEff80.config");
      //WZ taggers
      setMaps(configPath,"SmoothWContained50","R10_SF_LCTopo_WTag_SigEff50.config");
      setMaps(configPath,"SmoothWContained80","R10_SF_LCTopo_WTag_SigEff80.config");
      
    } else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets") {
      // Tagging scale factors available for some of the taggers for TCC jets
      
      const std::string configPath="rel21/Summer2019/";
      //WZ taggers
      setMaps(configPath,"SmoothWContained2VarMaxSig","R10_SF_TCC_2VarSmooth_WTag.config");
      setMaps(configPath,"SmoothZContained2VarMaxSig","R10_SF_TCC_2VarSmooth_ZTag.config");
    }
  }
  
  
}  // namespace top
