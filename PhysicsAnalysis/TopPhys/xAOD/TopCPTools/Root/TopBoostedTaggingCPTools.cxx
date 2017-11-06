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

  m_topTag_configFile_50 = "SmoothedTopTagger_AntiKt10LCTopoTrimmed_Tau32Split23FixedSignalEfficiency50_MC15c_20161209.dat" ;
  if (m_topTag_configFile_50 == "NFC")
      m_topTag_configFile_50 = "SmoothedNotContainedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency50_MC15c_20170318.dat";
  
  // build the 80% config file from the 50% one
  m_topTag_configFile_80 = m_topTag_configFile_50;
  int num=m_topTag_configFile_80.find("50_");
  m_topTag_configFile_80.replace(num,3,"80_");
  
  m_topTag50 = nullptr;
  m_topTag50 = std::unique_ptr<SmoothedTopTagger>( new SmoothedTopTagger( "topTag50" ) );
  top::check(m_topTag50->setProperty( "ConfigFile","SmoothedTopTaggers/"+m_topTag_configFile_50),"Failed to set property ConfigFile to topTag50");
  top::check(m_topTag50->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to topTag50");
  top::check(m_topTag50->initialize(),"Failed to retrieve topTag50 tool");
  
  m_topTag80 = nullptr;
  m_topTag80 = std::unique_ptr<SmoothedTopTagger>( new SmoothedTopTagger( "topTag80" ) );
  top::check(m_topTag80->setProperty( "ConfigFile","SmoothedTopTaggers/"+m_topTag_configFile_80),"Failed to set property ConfigFile to topTag80");
  top::check(m_topTag80->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to topTag80");
  top::check(m_topTag80->initialize(),"Failed to retrieve topTag80 tool");  
  
  // Boosted boson W/Z/H tagging
  m_WTag50 = nullptr;
  m_WTag50 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "WTag50" ) );
  top::check(m_WTag50->setProperty( "ConfigFile", m_WTag_configFile_50 ),"Failed to set property ConfigFile to WTag50");
  top::check(m_WTag50->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to WTag50");
  top::check(m_WTag50->initialize(),"Failed to retrieve W tight tagger tool");
  
  m_WTag80 = nullptr;
  m_WTag80 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "WTag80" ) );
  top::check(m_WTag80->setProperty( "ConfigFile", m_WTag_configFile_80 ),"Failed to set property ConfigFile to WTag80");
  top::check(m_WTag80->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to WTag80");
  top::check(m_WTag80->initialize(),"Failed to retrieve W loose tagger tool");
  
  m_ZTag50 = nullptr;
  m_ZTag50 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "ZTag50" ) );
  top::check(m_ZTag50->setProperty( "ConfigFile", m_ZTag_configFile_50 ),"Failed to set property ConfigFile to ZTag50");
  top::check(m_ZTag50->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to ZTag50");
  top::check(m_ZTag50->initialize(),"Failed to retrieve Z tight tagger tool");
  
  m_ZTag80 = nullptr;
  m_ZTag80 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "ZTag80" ) );
  top::check(m_ZTag80->setProperty( "ConfigFile", m_ZTag_configFile_80 ),"Failed to set property ConfigFile to ZTag80");
  top::check(m_ZTag80->setProperty( "DecorateJet",false),"Failed to set property ConfigFile to ZTag80");
  top::check(m_ZTag80->initialize(),"Failed to retrieve Z loose tagger tool");

  return StatusCode::SUCCESS;
}

}  // namespace top
