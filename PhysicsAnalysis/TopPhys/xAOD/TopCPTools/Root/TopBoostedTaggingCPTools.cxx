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

// Boosted tagging includes
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

namespace top {

BoostedTaggingCPTools::BoostedTaggingCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );
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
  
  
  std::vector<std::pair<std::string, std::string> > boostedJetTaggers= m_config->boostedJetTaggers();
  if (boostedJetTaggers.size()==0) {
    ATH_MSG_INFO("top::BoostedTaggingCPTools: boostedJetTagging not set. No need to initialise anything.");
    return StatusCode::SUCCESS;
  }
  
  
  std::unordered_map<std::string,std::string > taggersConfigs;
  std::unordered_map<std::string,std::string > taggersCalibAreas;
  
  std::vector<std::string > taggersTypes = {"JSSWTopTaggerDNN","SmoothedWZTagger"};
  
  taggersCalibAreas["JSSWTopTaggerDNN"] = "JSSWTopTaggerDNN/Rel21/";
  taggersCalibAreas["SmoothedWZTagger"] = "SmoothedWZTaggers/Rel21/";
  
  if( m_config->sgKeyLargeRJets() == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets") {
    taggersConfigs["JSSWTopTaggerDNN:TopQuarkContained50"] = "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_20190405_50Eff.dat";
    taggersConfigs["JSSWTopTaggerDNN:TopQuarkContained80"] = "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC16d_20190405_80Eff.dat";
    taggersConfigs["JSSWTopTaggerDNN:TopQuarkInclusive50"] = "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16d_20190405_50Eff.dat";
    taggersConfigs["JSSWTopTaggerDNN:TopQuarkInclusive80"] = "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16d_20190405_80Eff.dat";
    
    taggersConfigs["SmoothedWZTagger:SmoothedContainedWTagger50"] = "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat";
    taggersConfigs["SmoothedWZTagger:SmoothedContainedWTagger80"] = "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16d_20190410.dat";
    
    taggersConfigs["SmoothedWZTagger:SmoothedContainedZTagger50"] = "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16d_20190410.dat";
    taggersConfigs["SmoothedWZTagger:SmoothedContainedZTagger80"] = "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16d_20190410.dat";
  }
  else if (m_config->sgKeyLargeRJets() == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20") {
    
    taggersConfigs["SmoothedWZTagger:SmoothedWTagger2VarMaxSignificance"] = "SmoothedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190410.dat";
    taggersConfigs["SmoothedWZTagger:SmoothedZTagger2VarMaxSignificance"] = "SmoothedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190410.dat";
    
    taggersConfigs["SmoothedWZTagger:SmoothedContainedWTagger3VarMaxSignificance"] = "SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat";
    taggersConfigs["SmoothedWZTagger:SmoothedContainedZTagger3VarMaxSignificance"] = "SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat";
    
    
  }

  for(std::pair<std::string, std::string> name : boostedJetTaggers){
    
    std::string taggerType = name.first;
    std::string shortName = name.second;
    std::string fullName= taggerType + "_" + shortName; // Name to idendify tagger
    std::string origName= taggerType + ":" + shortName; // Original name set in the top config
    
    std::cout << "taggerName: " << origName << std::endl;
   
    top::check(std::find(taggersTypes.begin(),taggersTypes.end(),taggerType)!=taggersTypes.end(),"Error in BoostedTaggingCPTools: Unknown TAGGER_TYPE.");
    
    if(taggerType=="JSSWTopTaggerDNN") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],JSSWTopTaggerDNN), "Failed to make " + origName );
    else if(taggerType=="SmoothedWZTagger") top::check(ASG_MAKE_ANA_TOOL(m_taggers[fullName],SmoothedWZTagger), "Failed to make " + origName );
    
    m_taggers[fullName].setName(fullName);
    top::check(m_taggers[fullName].setProperty( "ConfigFile", taggersConfigs[origName]),"Failed to set ConfigFile for "+origName);
    top::check(m_taggers[fullName].setProperty( "CalibArea", taggersCalibAreas[taggerType]),"Failed to set CalibArea for " + origName);
    top::check(m_taggers[fullName].initialize(),"Failed to initialize " + origName);
  
  }

  return StatusCode::SUCCESS;
}

}  // namespace top
