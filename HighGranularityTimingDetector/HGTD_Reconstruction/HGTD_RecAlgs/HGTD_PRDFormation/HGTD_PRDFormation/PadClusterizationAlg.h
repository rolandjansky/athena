/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PRDFormation/PadClusterizationAlg.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date September, 2021
 *
 * @brief RDO objects get transformed into PrepRawData objects and recorded in
 * the event store.
 *
 * TODO: - for R22, the Read/WriteHandleKey interface changed: please update!
 */

#ifndef HGTD_RECALGS_PRDFORMATION_H
#define HGTD_RECALGS_PRDFORMATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"
#include "HGTD_RecToolInterfaces/IHGTD_PadClusterizationTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include <string>

class HGTD_ID;

namespace HGTD {

class PadClusterizationAlg : public AthAlgorithm {

public:
  PadClusterizationAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PadClusterizationAlg() {}
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  ToolHandle<HGTD::IHGTD_PadClusterizationTool> m_clusterization_tool;

  // FIXME: should look like this in master
  // SG::ReadHandleKey<HGTD::HGTD_RDOContainer> m_rdo_rh_key{
  //     this, "RDOContainerName", "HGTD_RDOs", "Name of the HGTD_RDO
  //     container"};
  //
  // SG::WriteHandleKey<HGTD::HGTD_ClusterContainer> m_prd_wh_key{
  //     this, "PRDContainerName", "HGTD_Clusters",
  //     "Name of the HGTD_Cluster container"};
  SG::ReadHandleKey<HGTD::HGTD_RDOContainer> m_rdo_rh_key;

  SG::WriteHandleKey<HGTD::HGTD_ClusterContainer> m_prd_wh_key;

  const HGTD_ID* m_hgtd_idhelper;
};

} // namespace HGTD

#endif // HGTD_RECALGS_PRDFORMATION_H
