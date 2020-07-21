/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: TopToolStore.cxx 808118 2017-07-11 17:41:22Z tpelzer $
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
  TopToolStore::TopToolStore(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr) {
    declareProperty("config", m_config);
  }

  StatusCode TopToolStore::initialize() {
    ATH_MSG_INFO("top::TopToolStore initialize...");

    m_trigger_CP_tools = std::make_unique<top::TriggerCPTools>("top::TriggerCPTools");
    top::check(m_trigger_CP_tools->setProperty("config", m_config),
               "Failed to share config with trigger CP tools");
    top::check(m_trigger_CP_tools->initialize(),
               "Failed to initialize trigger tools");

    m_other_CP_tools = std::make_unique<top::OtherCPTools>("top::OtherCPTools");
    top::check(m_other_CP_tools->setProperty("config", m_config),
               "Failed to share config with other CP tools");
    top::check(m_other_CP_tools->initialize(),
               "Failed to initialize other tools");

    m_egamma_CP_tools = std::make_unique<top::EgammaCPTools>("top::EgammaCPTools");
    top::check(m_egamma_CP_tools->setProperty("config", m_config),
               "Failed to share config with egamma CP tools");
    top::check(m_egamma_CP_tools->initialize(),
               "Failed to initialize egamma tools");

    m_muon_CP_tools = std::make_unique<top::MuonCPTools>("top::MuonCPTools");
    top::check(m_muon_CP_tools->setProperty("config", m_config),
               "Failed to share config with muon CP tools");
    top::check(m_muon_CP_tools->initialize(),
               "Failed to initialize muon tools");

    m_tau_CP_tools = std::make_unique<top::TauCPTools>("top::TauCPTools");
    top::check(m_tau_CP_tools->setProperty("config", m_config),
               "Failed to share config with tau CP tools");
    top::check(m_tau_CP_tools->initialize(),
               "Failed to initialize tau tools");

    m_flavor_tagging_CP_tools = std::make_unique<top::FlavorTaggingCPTools>("top::FlavorTaggingCPTools");
    top::check(m_flavor_tagging_CP_tools->setProperty("config", m_config),
               "Failed to share config with flavour tagging CP tools");
    top::check(m_flavor_tagging_CP_tools->initialize(),
               "Failed to initialize flavor tagging tools");

    m_boosted_tagging_CP_tools = std::make_unique<top::BoostedTaggingCPTools>("top::BoostedTaggingCPTools");
    top::check(m_boosted_tagging_CP_tools->setProperty("config", m_config),
               "Failed to share config with flavour tagging CP tools");
    top::check(m_boosted_tagging_CP_tools->initialize(),
               "Failed to initialize boosted tagging tools");

    m_jetMET_CP_tools = std::make_unique<top::JetMETCPTools>("top::JetMETCPTools");
    top::check(m_jetMET_CP_tools->setProperty("config", m_config),
               "Failed to share config with Jet/MET CP tools");
    top::check(m_jetMET_CP_tools->initialize(),
               "Failed to initialize Jet/MET tools");

    m_isolation_CP_tools = std::make_unique<top::IsolationCPTools>("top::IsolationCPTools");
    top::check(m_isolation_CP_tools->setProperty("config", m_config),
               "Failed to share config with isolation CP tools");
    top::check(m_isolation_CP_tools->initialize(),
               "Failed to initialize isolation tools");

    m_OR_CP_tools = std::make_unique<top::OverlapRemovalCPTools>("top::OverlapRemovalCPTools");
    top::check(m_OR_CP_tools->setProperty("config", m_config),
               "Failed to share config with overlap removal CP tools");
    top::check(m_OR_CP_tools->initialize(),
               "Failed to initialize overlap removal tools");

    m_ghost_track_CP_tools = std::make_unique<top::GhostTrackCPTools>("top::GhostTrackCPTools");
    top::check(m_ghost_track_CP_tools->setProperty("config", m_config),
               "Failed to share config with ghost track CP tools");
    top::check(m_ghost_track_CP_tools->initialize(),
               "Failed to initialize ghost track tools");

    m_track_CP_tools = std::make_unique<top::TrackCPTools>("top::TrackCPTools");
    top::check(m_track_CP_tools->setProperty("config", m_config),
               "Failed to share config with track CP tools");
    top::check(m_track_CP_tools->initialize(),
               "Failed to initialize track tools");


    return StatusCode::SUCCESS;
  }

  StatusCode TopToolStore::finalize() {
    ATH_MSG_INFO("top::TopToolStore finalize");
    return StatusCode::SUCCESS;
  }
}  // namespace top
