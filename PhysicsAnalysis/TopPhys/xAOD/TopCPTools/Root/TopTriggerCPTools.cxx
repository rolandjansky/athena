/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopTriggerCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Trigger include(s):
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "TrigTauMatching/TrigTauMatching.h"

namespace top {

TriggerCPTools::TriggerCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "TrigConfigTool", m_trigConfTool );
  declareProperty( "TrigDecisionTool", m_trigDecisionTool );
  declareProperty( "TrigMatchTool", m_trigMatchTool );
}

StatusCode TriggerCPTools::initialize() {
  ATH_MSG_INFO("top::TriggerCPTools initialize...");

  if (m_config->makeAllCPTools()) {
    if (!m_config->isTruthDxAOD()) {
      ///-- The configuration tool --///
      const std::string trig_config_name = "TrigConf::xAODConfigTool";
      if ( asg::ToolStore::contains<TrigConf::ITrigConfigTool>(trig_config_name) ) {
        m_trigConfTool = asg::ToolStore::get<TrigConf::ITrigConfigTool>(trig_config_name);
      } else {
        TrigConf::xAODConfigTool* configTool = new TrigConf::xAODConfigTool(trig_config_name);
        top::check( configTool->initialize() , "Failed to initialize trigger config tool" );
        m_trigConfTool = configTool;
      }

      ///-- The decision tool --///
      const std::string trig_decision_name = "Trig::TrigDecisionTool";
      if ( asg::ToolStore::contains<Trig::TrigDecisionTool>(trig_decision_name) ) {
        m_trigDecisionTool = asg::ToolStore::get<Trig::TrigDecisionTool>(trig_decision_name);
      } else {
        Trig::TrigDecisionTool* trigDecTool = new Trig::TrigDecisionTool(trig_decision_name);
        top::check( trigDecTool->setProperty("ConfigTool", m_trigConfTool ) , "Failed to setProperty" );
        top::check( trigDecTool->setProperty("TrigDecisionKey", "xTrigDecision") , "Failed to setProperty" );
        top::check( trigDecTool->initialize() , "Failed to initialize trigger decision tool" );
        m_trigDecisionTool = trigDecTool;
      }

      // Trigger matching tool
      const std::string trig_match_name = "Trig::MatchingTool";
      if (asg::ToolStore::contains<Trig::IMatchingTool>(trig_match_name)) {
        m_trigMatchTool = asg::ToolStore::get<Trig::IMatchingTool>(trig_match_name);
      } else {
        Trig::MatchingTool* trigMatchTool = new Trig::MatchingTool(trig_match_name);
        top::check(trigMatchTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                  "Failed to set trigger decision tool to trigger matching tool");
        top::check(trigMatchTool->initialize(),
                  "Failed to initialize trig. matching tool");
        m_trigMatchTool = trigMatchTool;
      }

      ///-- Tau matching --///
      if( m_config->useTaus() ){
        const std::string tauMatchName = "Trig::TrigTauMatchingTool";
        if ( asg::ToolStore::contains<Trig::ITrigTauMatchingTool>(tauMatchName) ) {
          m_trigMatchTauTool = asg::ToolStore::get<Trig::ITrigTauMatchingTool>(tauMatchName);
        }
        else {
          Trig::TrigTauMatchingTool* trigMatchTauTool = new Trig::TrigTauMatchingTool(tauMatchName);
          top::check( trigMatchTauTool->setProperty("TrigDecisionTool", m_trigDecisionTool),
                      "Failed to set trig dec tool for tau matching" );
          top::check( trigMatchTauTool->initialize(),
                      "Failed to initialize tau trigger matching tool" );
          m_trigMatchTauTool = trigMatchTauTool;
        }
      }
    }
    else {
      ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything on truth DxAOD");
    }
  }
  else {
    ATH_MSG_INFO("top::TriggerCPTools: no need to initialise anything in mini-xAOD mode");
  }

  return StatusCode::SUCCESS;
}

}  // namespace top
