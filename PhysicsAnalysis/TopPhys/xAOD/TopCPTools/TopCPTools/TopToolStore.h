/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: TopToolStore.h 808118 2017-07-11 17:41:22Z tpelzer $
#ifndef ANALYSISTOP_TOPCPTOOLS_TOPTOOLSTORE_H
#define ANALYSISTOP_TOPCPTOOLS_TOPTOOLSTORE_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief TopToolStore
 *   Configure all asg::AsgTools and place into asg::ToolStore
 *   This will allow other tools and algorithms to retrieve the tools
 *   This means that we can share tools and only configure them once
 *
 * $Revision: 808118 $
 * $Date: 2017-07-11 18:41:22 +0100 (Tue, 11 Jul 2017) $
 *
 **/

// system include(s):
#include <vector>
#include <memory>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"

// Top includes
#include "TopCPTools/TopTrackCPTools.h"
#include "TopCPTools/TopGhostTrackCPTools.h"
#include "TopCPTools/TopFlavorTaggingCPTools.h"
#include "TopCPTools/TopBoostedTaggingCPTools.h"
#include "TopCPTools/TopEgammaCPTools.h"
#include "TopCPTools/TopMuonCPTools.h"
#include "TopCPTools/TopIsolationCPTools.h"
#include "TopCPTools/TopTauCPTools.h"
#include "TopCPTools/TopJetMETCPTools.h"
#include "TopCPTools/TopTriggerCPTools.h"
#include "TopCPTools/TopOverlapRemovalCPTools.h"
#include "TopCPTools/TopOtherCPTools.h"

namespace top {
  // Forward declaration(s):
  class TopConfig;

  class TopToolStore final: public asg::AsgTool {
  public:
    explicit TopToolStore(const std::string& name);
    virtual ~TopToolStore() {}

    TopToolStore(const TopToolStore& rhs) = delete;
    TopToolStore(TopToolStore&& rhs) = delete;
    TopToolStore& operator = (const TopToolStore& rhs) = delete;

    StatusCode initialize();
    StatusCode finalize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::unique_ptr<OverlapRemovalCPTools>   m_OR_CP_tools;
    std::unique_ptr<OtherCPTools>            m_other_CP_tools;
    std::unique_ptr<FlavorTaggingCPTools>    m_flavor_tagging_CP_tools;
    std::unique_ptr<BoostedTaggingCPTools>   m_boosted_tagging_CP_tools;
    std::unique_ptr<TriggerCPTools>          m_trigger_CP_tools;
    std::unique_ptr<EgammaCPTools>           m_egamma_CP_tools;
    std::unique_ptr<MuonCPTools>             m_muon_CP_tools;
    std::unique_ptr<IsolationCPTools>        m_isolation_CP_tools;
    std::unique_ptr<TauCPTools>              m_tau_CP_tools;
    std::unique_ptr<JetMETCPTools>           m_jetMET_CP_tools;
    std::unique_ptr<GhostTrackCPTools>       m_ghost_track_CP_tools;
    std::unique_ptr<TrackCPTools>            m_track_CP_tools;

  };
} // namespace

#endif
