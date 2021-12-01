/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECOTOOL_H
#define AFP_SIDLOCRECOTOOL_H

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

#include "AFP_LocReco/IAFPSiDLocRecoTrackAlgTool.h"
#include "AFP_LocReco/IAFP_SIDLocRecoTool.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <string>
#include <vector>
#include <utility>


/// Algorithm reconstructing tracks from hits clusters
class AFP_SIDLocRecoTool
  : public extends<AthAlgTool, IAFP_SIDLocRecoTool>
{
public:
  AFP_SIDLocRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  ~AFP_SIDLocRecoTool() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute(const EventContext& ctx) const override;

  /// Does nothing
  StatusCode finalize() override;

private:

  /// @brief Vector of tool handles to be used for tracks reconstruction
  /// 
  /// Each station has its own track reco algorithm. This array contains all of them
  ToolHandleArray<IAFPSiDLocRecoTrackAlgTool> m_recoToolsList {this,"RecoToolsList",{},"List of AFP track reconstruction tools"};

  /// @brief Array of *unique* write handle keys for track output containers
  ///
  /// Each write handle key has to be unique and in sync with m_recoToolsList. It's possible for several track reco algorithms contributing to the common container. Ideally, one should setup track reco containers (in AFP_LocReco/AFP_LocReco_joboption.py) and let the python script pick unique names.
  SG::WriteHandleKeyArray<xAOD::AFPTrackContainer> m_arrayOfWriteHandleKeys{this, "AFPTrackContainerList", {"AFPTrackContainer"}, "List of output containers"};

  /// @ brief Monitoring tool
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};
};

#endif
