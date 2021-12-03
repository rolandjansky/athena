/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECOTOOL_H
#define AFP_GLOBRECOTOOL_H


#include "AFP_GlobReco/IAFP_ProtonRecoTool.h"
#include "AFP_GlobReco/IAFP_GlobRecoTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProtonAuxContainer.h"

// monitoring tool to be added soon
// #include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <algorithm>


/// Algorithm reconstructing protons from tracks
class AFP_GlobRecoTool : public extends<AthAlgTool, IAFP_GlobRecoTool>
{
public:
  AFP_GlobRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  ~AFP_GlobRecoTool() = default;

  StatusCode initialize() override;

  /// Run proton reco tool
  StatusCode execute(const EventContext& ctx) const override;

private:

  /// @brief Vector of tool handles to be used for proton reconstruction
  /// 
  /// Each station has its own track reco algorithm. This array contains all of them
  ToolHandleArray<IAFP_ProtonRecoTool> m_recoToolsList {this,"RecoToolsList",{},"List of AFP proton reconstruction tools"};

  /// @brief Array of *unique* write handle keys for track output containers
  ///
  /// Each write handle key has to be unique and in sync with m_recoToolsList. It's possible for several track reco algorithms contributing to the common container. Ideally, one should setup track reco containers (in AFP_GlobReco/AFP_GlobReco_joboption.py) and let the python script pick unique names.
  SG::WriteHandleKeyArray<xAOD::AFPProtonContainer> m_arrayOfWriteHandleKeys{this, "AFPProtonContainerList", {"AFPProtonContainer"}, "List of output containers"};

  // monitoring tool to be added soon
  /// @ brief Monitoring tool
//   ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};
};

#endif
