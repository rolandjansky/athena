/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_VERTEXRECOTOOL_H
#define AFP_VERTEXRECOTOOL_H


#include "AFP_VertexReco/IAFP_VertexRecoTool.h"
#include "AFP_VertexReco/IAFP_TimeRecoTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODForward/AFPVertexContainer.h"
#include "xAODForward/AFPVertexAuxContainer.h"

// monitoring tool to be added soon
// #include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <algorithm>


/// Algorithm reconstructing protons from tracks
class AFP_VertexRecoTool : public extends<AthAlgTool, IAFP_VertexRecoTool>
{
public:
  AFP_VertexRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  virtual ~AFP_VertexRecoTool() = default;

  virtual StatusCode  initialize() override;

  /// Run vertex reco tool
  virtual StatusCode execute(const EventContext& ctx) const override;

private:

  /// @brief Vector of tool handles to be used for vertex reconstruction
  /// 
  /// Several time reco algorithms will be implemented. This array contains all of them
  ToolHandleArray<IAFP_TimeRecoTool> m_recoToolsList {this,"RecoToolsList",{},"List of AFP vertex reconstruction tools"};

  /// @brief Array of *unique* write handle keys for vertex output container
  ///
  /// Each write handle key has to be unique and in sync with m_recoToolsList. It's possible for several time reco algorithms contributing to the common container. Ideally, one should setup vertex reco containers (in AFP_VertexReco/AFP_VertexReco_joboption.py) and let the python script pick unique names.
  SG::WriteHandleKeyArray<xAOD::AFPVertexContainer> m_arrayOfWriteHandleKeys{this, "AFPVertexContainerList", {"AFPVertexContainer"}, "List of output containers"};

  // monitoring tool to be added soon
  /// @ brief Monitoring tool
//   ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};
};

#endif
