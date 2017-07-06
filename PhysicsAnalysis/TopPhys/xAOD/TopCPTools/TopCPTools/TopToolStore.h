/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopToolStore.h 802731 2017-04-11 16:35:41Z tpelzer $
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
  * $Revision: 802731 $
  * $Date: 2017-04-11 18:35:41 +0200 (Tue, 11 Apr 2017) $
  *
  **/

// system include(s):
#include <vector>
#include <memory>

// Top includes
#include "TopCPTools/TopGhostTrackCPTools.h"
#include "TopCPTools/TopFlavorTaggingCPTools.h"
#include "TopCPTools/TopEgammaCPTools.h"
#include "TopCPTools/TopMuonCPTools.h"
#include "TopCPTools/TopIsolationCPTools.h"
#include "TopCPTools/TopTauCPTools.h"
#include "TopCPTools/TopJetMETCPTools.h"
#include "TopCPTools/TopTriggerCPTools.h"
#include "TopCPTools/TopOverlapRemovalCPTools.h"
#include "TopCPTools/TopOtherCPTools.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"

namespace top{

  // Forward declaration(s):
  class TopConfig;

  class OverlapRemovalCPTools;
  class OtherCPTools;
  class FlavorTaggingCPTools;
  class TriggerCPTools;
  class EgammaCPTools;
  class MuonCPTools;
  class IsolationCPTools;
  class TauCPTools;
  class JetMETCPTools;

  class TopToolStore final : public asg::AsgTool {
    public:
      explicit TopToolStore( const std::string& name );
      virtual ~TopToolStore() {}

      TopToolStore(const TopToolStore& rhs) = delete;
      TopToolStore(TopToolStore&& rhs) = delete;
      TopToolStore& operator=(const TopToolStore& rhs) = delete;

      StatusCode initialize();
      StatusCode finalize();

    private:
      std::shared_ptr<top::TopConfig> m_config;

      int m_release_series = 24;  // Default to 2.4

      std::unique_ptr<top::OverlapRemovalCPTools>   m_OR_CP_tools;
      std::unique_ptr<top::OtherCPTools>            m_other_CP_tools;
      std::unique_ptr<top::FlavorTaggingCPTools>    m_flavor_tagging_CP_tools;
      std::unique_ptr<top::TriggerCPTools>          m_trigger_CP_tools;
      std::unique_ptr<top::EgammaCPTools>           m_egamma_CP_tools;
      std::unique_ptr<top::MuonCPTools>             m_muon_CP_tools;
      std::unique_ptr<top::IsolationCPTools>        m_isolation_CP_tools;
      std::unique_ptr<top::TauCPTools>              m_tau_CP_tools;
      std::unique_ptr<top::JetMETCPTools>           m_jetMET_CP_tools;
      std::unique_ptr<top::GhostTrackCPTools>       m_ghost_track_CP_tools;
  };

} // namespace

#endif
