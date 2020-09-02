/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterizationAlg.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

using namespace Muon;

MuonClusterizationAlg::MuonClusterizationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_clusterTool("Muon::MuonClusterizationTool/MuonClusterizationTool")
{
  declareProperty("TgcPrepDataContainer",  m_tgcPrdLocationInput  = "TGC_Measurements");
  declareProperty("TgcPrepDataContainerOutput", m_tgcPrdLocationOutput = "TGC_Clusters");
  declareProperty("RpcPrepDataContainer",  m_rpcPrdLocationInput  = "RPC_Measurements");
  declareProperty("RpcPrepDataContainerOutput", m_rpcPrdLocationOutput = "RPC_Clusters");
  declareProperty("ClusterTool", m_clusterTool);
}

StatusCode MuonClusterizationAlg::initialize()
{
  ATH_CHECK(m_clusterTool.retrieve());
  return StatusCode::SUCCESS; 
}

StatusCode MuonClusterizationAlg::execute()
{
  const TgcPrepDataContainer* tgcContainer = nullptr;
  if (evtStore()->retrieve(tgcContainer,m_tgcPrdLocationInput).isFailure() ) {
    ATH_MSG_WARNING("Could not find TgcPrepDataContainer at " << m_tgcPrdLocationInput);
    return StatusCode::RECOVERABLE;
  }

  const TgcPrepDataContainer* tgcContainerCluster = m_clusterTool->cluster(*tgcContainer);
  if (evtStore()->record(tgcContainerCluster,m_tgcPrdLocationOutput).isFailure() ) {
    ATH_MSG_WARNING("Could not record TgcPrepDataContainer at " << m_tgcPrdLocationOutput);
    return StatusCode::RECOVERABLE;
  }

  const RpcPrepDataContainer* rpcContainer = nullptr;
  if (evtStore()->retrieve(rpcContainer,m_rpcPrdLocationInput).isFailure() ) {
    ATH_MSG_WARNING("Could not find RpcPrepDataContainer at " << m_rpcPrdLocationInput);
    return StatusCode::RECOVERABLE;
  }

  const RpcPrepDataContainer* rpcContainerCluster = m_clusterTool->cluster(*rpcContainer);
  if (evtStore()->record(rpcContainerCluster,m_rpcPrdLocationOutput).isFailure() ) {
    ATH_MSG_WARNING("Could not record RpcPrepDataContainer at " << m_rpcPrdLocationOutput);
    return StatusCode::RECOVERABLE;
  }

  return StatusCode::SUCCESS;
}
