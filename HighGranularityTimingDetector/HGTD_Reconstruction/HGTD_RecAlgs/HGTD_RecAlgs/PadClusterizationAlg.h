/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecAlgs/PadClusterizationAlg.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @brief RDO objects get transformed into PrepRawData objects and recorded in
 * the event store.
 */

#ifndef HGTD_RECALGS_PRDFORMATION_H
#define HGTD_RECALGS_PRDFORMATION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RawData/HGTD_RDO_Container.h"
#include "HGTD_RecToolInterfaces/IHGTD_PadClusterizationTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include <string>

class HGTD_ID;

namespace HGTD {

class PadClusterizationAlg : public AthReentrantAlgorithm {

public:
  PadClusterizationAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PadClusterizationAlg() {}
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  ToolHandle<IHGTD_PadClusterizationTool> m_clusterization_tool{this, "ClusterizationTool", "HGTD::SinglePadClusterTool", "Tool for translating HGTD_RDO objects into HGTD_PrepRawData clusters"};

  SG::ReadHandleKey<HGTD_RDO_Container> m_rdo_rh_key{this, "RDOContainerName", "HGTD_RDOs", "Name of the HGTD_RDO container"};
  SG::WriteHandleKey<HGTD_ClusterContainer> m_prd_wh_key{this, "PRDContainerName", "HGTD_Clusters", "Name of the HGTD_Cluster container"};

  const HGTD_ID* m_hgtd_idhelper{nullptr};
};

} // namespace HGTD

#endif // HGTD_RECALGS_PRDFORMATION_H
