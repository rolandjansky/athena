/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "BoostedJetTaggers/SmoothedTopTagger.h"
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
      const std::string& shortName = name.second;
      const std::string fullName = taggerType + "_" + shortName; // Name to idendify tagger
      const std::string origName = taggerType + ":" + shortName; // Original name set in the top config

      top::check(m_taggersConfigs.find(origName) != std::end(m_taggersConfigs),
                 ("Error in BoostedTaggingCPTools: Unknown tagger in the config file: " + origName).c_str());

      top::check(std::find(std::begin(m_taggersTypes), std::end(m_taggersTypes),
                           taggerType) != std::end(m_taggersTypes), "Error in BoostedTaggingCPTools: Unknown TAGGER_TYPE.");
      if (taggerType == "JSSWTopTaggerDNN") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],
                                                                         JSSWTopTaggerDNN),
                                                       "Failed to make " + origName);
      else if (taggerType == "SmoothedWZTagger" or taggerType == "SmoothedWZTaggerCNN") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],
                                                                              SmoothedWZTagger),
                                                            "Failed to make " + origName);

      m_taggers[fullName].setName(fullName);
      top::check(m_taggers[fullName].setProperty("ConfigFile",
                                                 m_taggersConfigs[origName]), "Failed to set ConfigFile for " + origName);
      top::check(m_taggers[fullName].setProperty("CalibArea",
                                                 m_taggersCalibAreas[taggerType]),
                 "Failed to set CalibArea for " + origName);
      // not all BJT taggers implement IsMC property -- only those that have calibration SFs
      // so we have to check here that we try to set this property only where applicable
      if (taggerType == "JSSWTopTaggerDNN" || taggerType == "SmoothedWZTagger") {
        top::check(m_taggers[fullName].setProperty("IsMC", m_config->isMC()), "Failed to set IsMC for " + origName);
      }
      top::check(m_taggers[fullName].initialize(), "Failed to initialize " + origName);

      // initialize SF uncertainty tools for supported WPs
      if (m_config->isMC()) {
        std::string jet_def = m_config->sgKeyLargeRJets();
        jet_def.erase(jet_def.length() - 4); // jet collection name sans 'Jets' suffix

        const std::string name = "JetSFuncert_" + fullName;
        try {
          const std::string& cfg = m_taggerSFsConfigs.at(origName);
          JetUncertaintiesTool* jet_SF_tmp = new JetUncertaintiesTool(name);

          top::check(jet_SF_tmp->setProperty("JetDefinition", jet_def), "Failed to set JetDefinition for " + name);
          top::check(jet_SF_tmp->setProperty("MCType", "MC16"), "Failed to set MCType for " + name);
          top::check(jet_SF_tmp->setProperty("ConfigFile", cfg), "Failed to set ConfigFile for " + name);
          top::check(jet_SF_tmp->setProperty("IsData", false), "Failed to set IsData for " + name);
          top::check(jet_SF_tmp->initialize(), "Failed to initialize " + name);
          m_tagSFuncertTool[fullName] = jet_SF_tmp;
          m_config->setCalibBoostedJetTagger(fullName, m_taggerSFsNames[origName]);
        } catch (std::out_of_range& e) {
          // skip taggers which do not yet have SFs available
          ATH_MSG_WARNING("Boosted jet tagger " + fullName + " is not yet calibrated! No SFs are available.");
        }
      }
    }

    return StatusCode::SUCCESS;
  }
  
  void BoostedTaggingCPTools::initTaggersMaps() {
    
    // Calib areas
    m_taggersCalibAreas["JSSWTopTaggerDNN"] = "JSSWTopTaggerDNN/Rel21/";
    m_taggersCalibAreas["SmoothedWZTagger"] = "SmoothedWZTaggers/Rel21/";
    m_taggersCalibAreas["SmoothedWZTaggerCNN"] = "Local"; // this is needed only until configs will be on cvmf
    
    // Supported tagger types
    m_taggersTypes = {
      "JSSWTopTaggerDNN", "SmoothedWZTagger", "SmoothedWZTaggerCNN"
    };
    
    // Supported jet collections
    m_jetCollections = {
      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
      "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
      "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
    };
    
    
    auto setConfig = [this](const std::string& taggerType, const std::string& tagger, const std::string& configName) {
      m_taggersConfigs[taggerType+":"+tagger] = configName;
    };
    
    if (m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
      
      // Top taggers
      std::string taggerType = "JSSWTopTaggerDNN";
      setConfig(taggerType,"DNNTaggerTopQuarkContained50","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16_20201216_50Eff.dat");
      setConfig(taggerType,"DNNTaggerTopQuarkContained80","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16_20201216_80Eff.dat");
      setConfig(taggerType,"DNNTaggerTopQuarkInclusive50","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16_20201216_50Eff.dat");
      setConfig(taggerType,"DNNTaggerTopQuarkInclusive80","JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16_20201216_80Eff.dat");
      
      // WZ taggers
      taggerType="SmoothedWZTagger";
      setConfig(taggerType,"SmoothWContained50","SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat");
      setConfig(taggerType,"SmoothWContained80","SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat");
      setConfig(taggerType,"SmoothZContained50","SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat");
      setConfig(taggerType,"SmoothZContained80","SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat");

    } 
    else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets") {
      // WZ taggers
      std::string taggerType="SmoothedWZTagger";
      setConfig(taggerType,"SmoothWContained2VarMaxSig","SmoothedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat");
      setConfig(taggerType,"SmoothZContained2VarMaxSig","SmoothedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat");
      setConfig(taggerType,"SmoothW3VarMaxSig","SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
      setConfig(taggerType,"SmoothZ3VarMaxSig","SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
    }
    else if (m_config->sgKeyLargeRJets() == "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets") {
      // WZ taggers
      // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BoostedJetTaggingRecommendationFullRun2#UFO_jets
      m_taggersCalibAreas["SmoothedWZTagger"] = "SmoothedWZTaggers/Rel21/February2022/";
      m_taggersCalibAreas["JSSWTopTaggerDNN"] = "JSSWTopTaggerDNN/Rel21/February2022/";

      // 3-Var
      std::string taggerType="SmoothedWZTagger";
      setConfig(taggerType,"SmoothWContained50","SmoothedContainedWTagger_AntiKt10UFOCSSKSoftDrop_FixedSignalEfficiency50_20220221.dat");
      setConfig(taggerType,"SmoothWContained80","SmoothedContainedWTagger_AntiKt10UFOCSSKSoftDrop_FixedSignalEfficiency80_20220221.dat");
      setConfig(taggerType,"SmoothZContained50","SmoothedContainedZTagger_AntiKt10UFOCSSKSoftDrop_FixedSignalEfficiency50_20220221.dat");
      setConfig(taggerType,"SmoothZContained80","SmoothedContainedZTagger_AntiKt10UFOCSSKSoftDrop_FixedSignalEfficiency80_20220221.dat");
      
      // ANN
      taggerType="JSSWTopTaggerANN";
      setConfig(taggerType,"JSSANN50","JSSANN50Tagger_AntiKt10UFOCSSKSoftDrop_Jan22.dat");
      setConfig(taggerType,"JSSANN80","JSSANN80Tagger_AntiKt10UFOCSSKSoftDrop_Jan22.dat");

      // DNN
      taggerType="JSSWTopTaggerDNN";
      setConfig(taggerType,"JSSDNNW50","JSSDNN50Tagger_AntiKt10UFOCSSKSoftDrop_Jan22.dat");
      setConfig(taggerType,"JSSDNNW80","JSSDNN50Tagger_AntiKt10UFOCSSKSoftDrop_Jan22.dat");

      // CNN
      taggerType="SmoothedWZTaggerCNN";
      setConfig(taggerType,"SmoothWContained50_3VarCNN_mX", "SmoothedContainedWTagger_AntiKt10VanillaSD_FixedSignalEfficiency50_3VarCNN_mX_v1.dat");
      setConfig(taggerType,"SmoothWContained80_3VarCNN_mX", "SmoothedContainedWTagger_AntiKt10VanillaSD_FixedSignalEfficiency80_3VarCNN_mX_v1.dat");
      setConfig(taggerType,"SmoothZContained50_3VarCNN_mX", "SmoothedContainedZTagger_AntiKt10VanillaSD_FixedSignalEfficiency50_3VarCNN_mX_v1.dat");
      setConfig(taggerType,"SmoothZContained80_3VarCNN_mX", "SmoothedContainedZTagger_AntiKt10VanillaSD_FixedSignalEfficiency80_3VarCNN_mX_v1.dat");

      setConfig(taggerType,"SmoothWContained50_3VarCNN_RZoom", "SmoothedContainedWTagger_AntiKt10VanillaSD_FixedSignalEfficiency50_3VarCNN_RZoom_v1.dat");
      setConfig(taggerType,"SmoothWContained80_3VarCNN_RZoom", "SmoothedContainedWTagger_AntiKt10VanillaSD_FixedSignalEfficiency80_3VarCNN_RZoom_v1.dat");
      setConfig(taggerType,"SmoothZContained50_3VarCNN_RZoom", "SmoothedContainedZTagger_AntiKt10VanillaSD_FixedSignalEfficiency50_3VarCNN_RZoom_v1.dat");
      setConfig(taggerType,"SmoothZContained80_3VarCNN_RZoom", "SmoothedContainedZTagger_AntiKt10VanillaSD_FixedSignalEfficiency80_3VarCNN_RZoom_v1.dat");

    }

  }
  
  
  
  void BoostedTaggingCPTools::initSFsMaps() {
    // Here we initialize SFs maps
    
    // This lambda function is universal for all taggers
    auto setMaps = [this](const std::string& configPath,const std::string& taggerType,const std::string& tagger, const std::string& configName) {
      m_taggerSFsConfigs[taggerType+":"+tagger] = configPath + "/" + configName;
      m_taggerSFsNames[taggerType+":"+tagger] = tagger+"_SF";
    };
    
    if (m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
      // Tagging scale factors availabel for AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
      
      const std::string configPath="rel21/Fall2020";
      //Top taggers
      std::string taggerType="JSSWTopTaggerDNN";
      setMaps(configPath,taggerType,"DNNTaggerTopQuarkContained50","R10_SF_LCTopo_TopTagContained_SigEff50.config");
      setMaps(configPath,taggerType,"DNNTaggerTopQuarkContained80","R10_SF_LCTopo_TopTagContained_SigEff80.config");
      setMaps(configPath,taggerType,"DNNTaggerTopQuarkInclusive50","R10_SF_LCTopo_TopTagInclusive_SigEff50.config");
      setMaps(configPath,taggerType,"DNNTaggerTopQuarkInclusive80","R10_SF_LCTopo_TopTagInclusive_SigEff80.config");
      //WZ taggers
      taggerType="SmoothedWZTagger";
      setMaps(configPath,taggerType,"SmoothWContained50","R10_SF_LCTopo_WTag_SigEff50.config");
      setMaps(configPath,taggerType,"SmoothWContained80","R10_SF_LCTopo_WTag_SigEff80.config");
      
    } else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets") {
      // Tagging scale factors availabel for AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets
      
      const std::string configPath="rel21/Summer2019/";
      //WZ taggers
      std::string taggerType="SmoothedWZTagger";
      setMaps(configPath,taggerType,"SmoothWContained2VarMaxSig","R10_SF_TCC_2VarSmooth_WTag.config");
      setMaps(configPath,taggerType,"SmoothZContained2VarMaxSig","R10_SF_TCC_2VarSmooth_ZTag.config");
    }
  }
  
  
}  // namespace top
