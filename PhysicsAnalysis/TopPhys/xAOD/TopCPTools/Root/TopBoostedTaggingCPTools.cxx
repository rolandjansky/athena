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


    std::vector<std::pair<std::string, std::string> > boostedJetTaggers = m_config->boostedJetTaggers();
    if (boostedJetTaggers.size() == 0) {
      ATH_MSG_INFO("top::BoostedTaggingCPTools: boostedJetTagging not set. No need to initialise anything.");
      return StatusCode::SUCCESS;
    }


    top::check(
      m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" || m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
      "Error in BoostedTaggingCPTools: boosted jet taggers are not available for this large-R jet collection.");

    std::unordered_map<std::string, std::string > taggersConfigs;
    std::unordered_map<std::string, std::string > taggerSFconfigs;
    std::unordered_map<std::string, std::string > taggerSFnames;
    std::unordered_map<std::string, std::string > taggersCalibAreas;

    std::vector<std::string > taggersTypes = {
      "JSSWTopTaggerDNN", "SmoothedWZTagger"
    };

    taggersCalibAreas["JSSWTopTaggerDNN"] = "JSSWTopTaggerDNN/Rel21/";
    taggersCalibAreas["SmoothedWZTagger"] = "SmoothedWZTaggers/Rel21/";

    if (m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
      taggersConfigs["JSSWTopTaggerDNN:DNNTaggerTopQuarkContained50"] =
        "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_20190405_50Eff.dat";
      taggersConfigs["JSSWTopTaggerDNN:DNNTaggerTopQuarkContained80"] =
        "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_20190827_80Eff.dat";
      taggersConfigs["JSSWTopTaggerDNN:DNNTaggerTopQuarkInclusive50"] =
        "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16d_20190405_50Eff.dat";
      taggersConfigs["JSSWTopTaggerDNN:DNNTaggerTopQuarkInclusive80"] =
        "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16d_20190405_80Eff.dat";

      taggersConfigs["SmoothedWZTagger:SmoothWContained50"] =
        "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat";
      taggersConfigs["SmoothedWZTagger:SmoothWContained80"] =
        "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16d_20190410.dat";

      taggersConfigs["SmoothedWZTagger:SmoothZContained50"] =
        "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat";
      taggersConfigs["SmoothedWZTagger:SmoothZContained80"] =
        "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16d_20190410.dat";

      // tagger SF uncertainties
      taggerSFconfigs["JSSWTopTaggerDNN:DNNTaggerTopQuarkContained80"] =
        "rel21/Summer2019/R10_SF_LC_DNNContained80_TopTag.config";
      taggerSFnames["JSSWTopTaggerDNN:DNNTaggerTopQuarkContained80"] = "DNNTaggerTopQuarkContained80_SF";
    } else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets") {
      taggersConfigs["SmoothedWZTagger:SmoothWContained2VarMaxSig"] =
        "SmoothedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat";
      taggersConfigs["SmoothedWZTagger:SmoothZContained2VarMaxSig"] =
        "SmoothedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190809.dat";

      taggersConfigs["SmoothedWZTagger:SmoothW3VarMaxSig"] =
        "SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat";
      taggersConfigs["SmoothedWZTagger:SmoothZ3VarMaxSig"] =
        "SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat";

      // tagger SF uncertainties
      taggerSFconfigs["SmoothedWZTagger:SmoothWContained2VarMaxSig"] =
        "rel21/Summer2019/R10_SF_TCC_2VarSmooth_WTag.config";
      taggerSFnames["SmoothedWZTagger:SmoothWContained2VarMaxSig"] = "SmoothWContained2VarMaxSig_SF";
      taggerSFconfigs["SmoothedWZTagger:SmoothZContained2VarMaxSig"] =
        "rel21/Summer2019/R10_SF_TCC_2VarSmooth_ZTag.config";
      taggerSFnames["SmoothedWZTagger:SmoothZContained2VarMaxSig"] = "SmoothZContained2VarMaxSig_SF";
    }

    for (const std::pair<std::string, std::string>& name : boostedJetTaggers) {
      const std::string& taggerType = name.first;
      const std::string& shortName = name.second;
      const std::string fullName = taggerType + "_" + shortName; // Name to idendify tagger
      const std::string origName = taggerType + ":" + shortName; // Original name set in the top config

      top::check(taggersConfigs.find(origName) != taggersConfigs.end(),
                 ("Error in BoostedTaggingCPTools: Unknown tagger in the config file: " + origName).c_str());

      top::check(std::find(taggersTypes.begin(), taggersTypes.end(),
                           taggerType) != taggersTypes.end(), "Error in BoostedTaggingCPTools: Unknown TAGGER_TYPE.");
      if (taggerType == "JSSWTopTaggerDNN") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],
                                                                         JSSWTopTaggerDNN),
                                                       "Failed to make " + origName);
      else if (taggerType == "SmoothedWZTagger") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],
                                                                              SmoothedWZTagger),
                                                            "Failed to make " + origName);

      m_taggers[fullName].setName(fullName);
      top::check(m_taggers[fullName].setProperty("ConfigFile",
                                                 taggersConfigs[origName]), "Failed to set ConfigFile for " + origName);
      top::check(m_taggers[fullName].setProperty("ContainerName",
                                                 m_config->sgKeyLargeRJets()), "Failed to set ContainerName " + origName);
      top::check(m_taggers[fullName].setProperty("CalibArea",
                                                 taggersCalibAreas[taggerType]),
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
          const std::string& cfg = taggerSFconfigs.at(origName);
          JetUncertaintiesTool* jet_SF_tmp = new JetUncertaintiesTool(name);

          top::check(jet_SF_tmp->setProperty("JetDefinition", jet_def), "Failed to set JetDefinition for " + name);
          top::check(jet_SF_tmp->setProperty("MCType", "MC16"), "Failed to set MCType for " + name);
          top::check(jet_SF_tmp->setProperty("ConfigFile", cfg), "Failed to set MCType for " + name);
          top::check(jet_SF_tmp->setProperty("IsData", false), "Failed to set IsData for " + name);
          top::check(jet_SF_tmp->initialize(), "Failed to initialize " + name);
          m_tagSFuncertTool[fullName] = jet_SF_tmp;
          m_config->setCalibBoostedJetTagger(fullName, taggerSFnames[origName]);
        } catch (std::out_of_range& e) {
          // skip taggers which do not yet have SFs available
          ATH_MSG_WARNING("Boosted jet tagger " + fullName + " is not yet calibrated! No SFs are available.");
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}  // namespace top
