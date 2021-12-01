/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLOCRECOTOOL_H
#define AFP_TDLOCRECOTOOL_H

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

#include "AFP_LocReco/IAFPTDLocRecoTrackAlgTool.h"
#include "AFP_LocReco/IAFPTDLocRecoTool.h"

#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" 

#include <string>
#include <vector>
#include <utility>


/// Algorithm reconstructing tracks from hits 
class AFP_TDLocRecoTool
  : public extends<AthAlgTool, IAFPTDLocRecoTool>
{
public:
  AFP_TDLocRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  ~AFP_TDLocRecoTool() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute(const EventContext& ctx) const override;

  /// Does nothing
  StatusCode finalize() override;

private:
  /// @brief Vector of tool handles to be used for tracks reconstruction
  /// 
  /// Each station has its own track reco algorithm. This array contains all of them
  ToolHandleArray<IAFPTDLocRecoTrackAlgTool> m_recoToolsList {this,"RecoToolsList",{},"List of AFP ToF track reconstruction tools"};

  /// @brief Array of *unique* write handle keys for track output containers
  ///
  /// Each write handle key has to be unique and in sync with m_recoToolsList. It's possible for several track reco algorithms contributing to the common container. Ideally, one should setup track reco containers (in AFP_LocReco/AFP_LocReco_joboption.py) and let the python script pick unique names.
  SG::WriteHandleKeyArray<xAOD::AFPToFTrackContainer> m_arrayOfWriteHandleKeys{this, "AFPToFTrackContainerList", {"AFPToFTrackContainer"}, "List of output containers"};
};

#endif	//AFP_TDLOCRECOTOOL_h
