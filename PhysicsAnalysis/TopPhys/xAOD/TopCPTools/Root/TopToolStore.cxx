/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopToolStore.cxx 803886 2017-04-27 21:27:58Z iconnell $
#include "TopCPTools/TopToolStore.h"

#include <vector>
#include <string>
#include <limits>
#include <set>
#include <map>
#include <cstdlib>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

namespace top {

TopToolStore::TopToolStore( const std::string& name ) :
  asg::AsgTool( name ),
  m_config(nullptr) {
    declareProperty( "config" , m_config );
}

StatusCode TopToolStore::initialize() {
  ATH_MSG_INFO("top::TopToolStore initialize...");

  std::string release_series = "";
  const char* rel_temp = std::getenv("ROOTCORE_RELEASE_SERIES");
  if (rel_temp) {
    release_series = std::string(rel_temp);
  } else {
    ATH_MSG_WARNING("Can not access ROOTCORE_RELEASE_SERIES");
  }

  ATH_MSG_INFO( "Release variable : "+release_series );

  if (release_series == "23") {
    ATH_MSG_INFO("Setting release series to 2.3");
    m_release_series = 23;
  } else if (release_series == "24") {
    m_release_series = 24;
    ATH_MSG_INFO("Setting release series to 2.4");
  } else if (release_series == "25" ){
    m_release_series = 25;
    ATH_MSG_INFO("Setting release series to 2.6");
  } else {
    m_release_series = 24;
    ATH_MSG_WARNING("Unknown release series, assuming 2.4");
  }

  m_trigger_CP_tools = std::make_unique<top::TriggerCPTools>("top::TriggerCPTools");
  top::check(m_trigger_CP_tools->setProperty("config", m_config),
              "Failed to share config with trigger CP tools");
  top::check(m_trigger_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with trigger CP tools");
  top::check(m_trigger_CP_tools->initialize(),
              "Failed to initialize trigger tools");

  m_other_CP_tools = std::make_unique<top::OtherCPTools>("top::OtherCPTools");
  top::check(m_other_CP_tools->setProperty("config", m_config),
              "Failed to share config with other CP tools");
  top::check(m_other_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with other CP tools");
  top::check(m_other_CP_tools->initialize(),
              "Failed to initialize other tools");

  m_egamma_CP_tools = std::make_unique<top::EgammaCPTools>("top::EgammaCPTools");
  top::check(m_egamma_CP_tools->setProperty("config", m_config),
              "Failed to share config with egamma CP tools");
  top::check(m_egamma_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with egamma CP tools");
  top::check(m_egamma_CP_tools->initialize(),
              "Failed to initialize egamma tools");

  m_muon_CP_tools = std::make_unique<top::MuonCPTools>("top::MuonCPTools");
  top::check(m_muon_CP_tools->setProperty("config", m_config),
              "Failed to share config with muon CP tools");
  top::check(m_muon_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with muon CP tools");
  top::check(m_muon_CP_tools->initialize(),
              "Failed to initialize muon tools");

  m_tau_CP_tools = std::make_unique<top::TauCPTools>("top::TauCPTools");
  top::check(m_tau_CP_tools->setProperty("config", m_config),
              "Failed to share config with tau CP tools");
  top::check(m_tau_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with tau CP tools");
  top::check(m_tau_CP_tools->initialize(),
              "Failed to initialize tau tools");

  m_flavor_tagging_CP_tools = std::make_unique<top::FlavorTaggingCPTools>("top::FlavorTaggingCPTools");
  top::check(m_flavor_tagging_CP_tools->setProperty("config", m_config),
              "Failed to share config with flavour tagging CP tools");
  top::check(m_flavor_tagging_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with flavour tagging CP tools");
  top::check(m_flavor_tagging_CP_tools->initialize(),
              "Failed to initialize flavor tagging tools");

  m_jetMET_CP_tools = std::make_unique<top::JetMETCPTools>("top::JetMETCPTools");
  top::check(m_jetMET_CP_tools->setProperty("config", m_config),
              "Failed to share config with Jet/MET CP tools");
  top::check(m_jetMET_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with Jet/MET CP tools");
  top::check(m_jetMET_CP_tools->initialize(),
              "Failed to initialize Jet/MET tools");

  m_isolation_CP_tools = std::make_unique<top::IsolationCPTools>("top::IsolationCPTools");
  top::check(m_isolation_CP_tools->setProperty("config", m_config),
              "Failed to share config with isolation CP tools");
  top::check(m_isolation_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with isolation CP tools");
  top::check(m_isolation_CP_tools->initialize(),
              "Failed to initialize isolation tools");

  m_OR_CP_tools = std::make_unique<top::OverlapRemovalCPTools>("top::OverlapRemovalCPTools");
  top::check(m_OR_CP_tools->setProperty("config", m_config),
              "Failed to share config with overlap removal CP tools");
  top::check(m_OR_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with overlap removal CP tools");
  top::check(m_OR_CP_tools->initialize(),
              "Failed to initialize overlap removal tools");

  m_ghost_track_CP_tools = std::make_unique<top::GhostTrackCPTools>("top::GhostTrackCPTools");
  top::check(m_ghost_track_CP_tools->setProperty("config", m_config),
              "Failed to share config with ghost track CP tools");
  top::check(m_ghost_track_CP_tools->setProperty("release_series", m_release_series),
              "Failed to release_series series with ghost track CP tools");
  top::check(m_ghost_track_CP_tools->initialize(),
              "Failed to initialize ghost track tools");

  return StatusCode::SUCCESS;
}

StatusCode TopToolStore::finalize() {
  ATH_MSG_INFO("top::TopToolStore finalize");
  return StatusCode::SUCCESS;
}

}  // namespace top
