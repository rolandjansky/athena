/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopGhostTrackCPTools.h"

#include <map>
#include <string>
#include <utility>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"


namespace top {

GhostTrackCPTools::GhostTrackCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );
}

StatusCode GhostTrackCPTools::initialize() {
  ATH_MSG_INFO("top::GhostTrackCPTools initialize...");

  if (m_config->isTruthDxAOD()) {
    ATH_MSG_INFO("top::GhostTrackCPTools: no need to initialise anything on truth DxAOD");
    return StatusCode::SUCCESS;
  }

  if (!m_config->useJets() || !m_config->useJetGhostTrack()) {
    ATH_MSG_INFO("top::GhostTrackCPTools: no need to initialise anything since not using jets or tracks ghost-matched to jets");
    return StatusCode::SUCCESS;
  }

  if (!m_config->makeAllCPTools()) {
    ATH_MSG_INFO("top::GhostTrackCPTools: no need to initialise for mini-xAOD");
    return StatusCode::SUCCESS;
  }
  
  // This defines the run period (ranges) that are used by the
  // InDet::InDetTrackBiasingTool. The following inputs are possible:
  //     - No / empty input: Use the maximum range, that is [0;
  //       UInt32_t::max) as allowed run number value range. Initialise
  //       the tool with run number 0 (which will use 2015 precriptions).
  //     - A single run number: Configure the tool with this run number
  //       and to not apply checks / selection on the random run number of
  //       the actual events.
  //     - At least two values: Use this to define adjacent ranges, each
  //       of which is processed using a unique instance of the tool which
  //       was initialised using the lower bound of the range. From an
  //       input of size n, n-1 ranges are formed. The i-th element is the
  //       lower bound (incl) of the i-th range; the (i+1)-th element is
  //       the upper bound (excl) of the i-th range.
  m_runPeriods = {276262, 297730, 300909, 311482, 334738, 341650, 999999};
  
  m_config->runPeriodJetGhostTrack(m_runPeriods);
  
  top::check(setupSmearingTool(), "Failed to setup track smearing tools");
  top::check(setupBiasingTools(), "Failed to setup track biasing tools");
  top::check(setupTruthFilterTool(), "Failed to setup truth filter tools");
  top::check(setupJetTrackFilterTool(), "Failed to setup track filter tools");
  
  return StatusCode::SUCCESS;
}

StatusCode GhostTrackCPTools::setupSmearingTool(){

  if (asg::ToolStore::contains<InDet::InDetTrackSmearingTool>(m_smearingToolName)){
    m_smearingTool = asg::ToolStore::get<InDet::InDetTrackSmearingTool>(m_smearingToolName);
  } else {
    auto tool = new InDet::InDetTrackSmearingTool(m_smearingToolName);
    top::check(tool->initialize(), "Failure to initialize InDetTrackSmearingTool");
    m_smearingTool = tool;
    ATH_MSG_INFO(" Creating smearing tool "+m_smearingToolName );
  }
  return StatusCode::SUCCESS;
}

StatusCode GhostTrackCPTools::setupBiasingTools() {

  top::check(not m_runPeriods.empty(), "Assertion failed");

  // Two cases are possible:
  //     - Either a single run number was specified to the runPeriods
  //       parameter in which case we'll use exactly that run number, or
  //     - at least two numbers have been specified, which then define
  //       (potentially multiple) run number ranges.

  if (m_runPeriods.size() == 1){
    m_runPeriods.resize(1);

    const std::string biasToolName{m_biasToolPrefix + "_" + std::to_string(m_runPeriods[0])};

    if (asg::ToolStore::contains<InDet::InDetTrackBiasingTool>(biasToolName)){
      m_biasingTool[0] = asg::ToolStore::get<InDet::InDetTrackBiasingTool>(biasToolName);
    } else {
      auto tool = new InDet::InDetTrackBiasingTool(biasToolName);
      top::check(tool->setProperty("runNumber", m_runPeriods[0]),
                  "Failure to setProperty runNumber of InDetTrackBiasingTool "+biasToolName);
      top::check(tool->initialize(),
                  "Failure to initialize InDetTrackBiasingTool "+biasToolName);
      m_biasingTool[0] = tool;
    }
    ATH_MSG_INFO(" Creating biasing tool to calibrate for run#=" << m_runPeriods[0] <<": "+biasToolName );

  } else {
    m_biasingTool.resize(m_runPeriods.size() - 1);
    for (std::size_t i=0; i<m_runPeriods.size() - 1; ++i){
      const std::string biasToolName{m_biasToolPrefix
              + "_" + std::to_string(m_runPeriods[i])
              + "_" + std::to_string(m_runPeriods[i + 1])};
      ATH_MSG_INFO(" Creating biasing tool to calibrate for period [" << m_runPeriods[i] << ", " << m_runPeriods[i + 1] << "): "<<biasToolName );
      if (asg::ToolStore::contains<InDet::InDetTrackBiasingTool>(biasToolName)){
          m_biasingTool[i] =
              asg::ToolStore::get<InDet::InDetTrackBiasingTool>(biasToolName);
      } else {
          auto tool = new InDet::InDetTrackBiasingTool(biasToolName);
          top::check(tool->setProperty("runNumber", m_runPeriods[i]),
                      "Failure to setProperty runNumber of InDetTrackBiasingTool "+biasToolName);
          top::check(tool->initialize(),
                      "Failure to initialize InDetTrackBiasingTool "+biasToolName);
          m_biasingTool[i] = tool;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode GhostTrackCPTools::setupTruthFilterTool(){

  if (asg::ToolStore::contains<InDet::InDetTrackTruthOriginTool>(m_truthOriginToolName)){
    m_truthOriginTool = asg::ToolStore::get<InDet::InDetTrackTruthOriginTool>(m_truthOriginToolName);
  } else {
    auto tool = new InDet::InDetTrackTruthOriginTool(m_truthOriginToolName);
    top::check(tool->initialize(),
                "Failure to initialize InDetTrackTruthOriginTool "+m_truthOriginToolName);
    m_truthOriginTool = tool;
    ATH_MSG_INFO(" Creating truth origin tool "+m_truthOriginToolName );
  }

  if (asg::ToolStore::contains<InDet::InDetTrackTruthFilterTool>(m_truthFilterToolName)){
    m_truthFilterTool = asg::ToolStore::get<InDet::InDetTrackTruthFilterTool>(m_truthFilterToolName);
  } else {
    auto tool = new InDet::InDetTrackTruthFilterTool(m_truthFilterToolName);
    top::check(tool->setProperty("trackOriginTool",
                                  ToolHandle<InDet::IInDetTrackTruthOriginTool>{&(* m_truthOriginTool)}),
                "Failed to setProperty trackOriginTool of InDetTrackTruthFilterTool "+m_truthFilterToolName);
    top::check(tool->initialize(),
                "Failure to initialize InDetTrackTruthFilterTool "+m_truthFilterToolName);
    m_truthFilterTool = tool;
    ATH_MSG_INFO(" Creating truth filter tool "+m_truthFilterToolName );
  }
  return StatusCode::SUCCESS;
}

StatusCode GhostTrackCPTools::setupJetTrackFilterTool(){

  if (asg::ToolStore::contains<InDet::InDetTrackTruthOriginTool>(m_truthOriginToolName)){
    m_truthOriginTool = asg::ToolStore::get<InDet::InDetTrackTruthOriginTool>(m_truthOriginToolName);
  } else {
    auto tool = new InDet::InDetTrackTruthOriginTool(m_truthOriginToolName);
    top::check(tool->initialize(),
                "Failure to initialize InDetTrackTruthOriginTool "+m_truthOriginToolName);
    m_truthOriginTool = tool;
    ATH_MSG_INFO(" Creating truth origin tool "+m_truthOriginToolName );
  }

  if (asg::ToolStore::contains<InDet::JetTrackFilterTool>(m_jetTrackFilterToolName)){
    m_jetTrackFilterTool = asg::ToolStore::get<InDet::JetTrackFilterTool>(m_jetTrackFilterToolName);
  } else {
    auto tool = new InDet::JetTrackFilterTool(m_jetTrackFilterToolName);
    top::check(tool->setProperty("trackOriginTool",
                                  ToolHandle<InDet::IInDetTrackTruthOriginTool>{&(* m_truthOriginTool)}),
                "Failed to setProperty trackOriginTool of InDetTrackTruthFilterTool "+m_truthFilterToolName);
    top::check(tool->initialize(), "Failure to initialize JetTrackFilterTool");
    m_jetTrackFilterTool = tool;
    ATH_MSG_INFO(" Creating jet track filter tool" );
  }
  return StatusCode::SUCCESS;
}

}  // namespace top
