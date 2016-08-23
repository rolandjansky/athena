/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterizationAlg.h"
#include "MuonClusterization/IMuonClusterizationTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

using namespace Muon;

MuonClusterizationAlg::MuonClusterizationAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
  , m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
  , m_clusterTool("Muon::MuonClusterizationTool/MuonClusterizationTool")
{
  declareProperty("TgcPrepDataContainer",  m_tgcPrdLocationInput  = "TGC_Measurements");
  declareProperty("TgcPrepDataContainerOutput", m_tgcPrdLocationOutput = "TGC_Clusters");
  declareProperty("RpcPrepDataContainer",  m_rpcPrdLocationInput  = "RPC_Measurements");
  declareProperty("RpcPrepDataContainerOutput", m_rpcPrdLocationOutput = "RPC_Clusters");
  declareProperty("IdHelperTool", m_idHelper);
  declareProperty("ClusterTool", m_clusterTool);
}

MuonClusterizationAlg::~MuonClusterizationAlg()
{

}

StatusCode MuonClusterizationAlg::initialize()
{
  if( AthAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

  if (m_idHelper.retrieve().isFailure()){
    msg(MSG::ERROR) <<"Could not get " << m_idHelper <<endmsg; 
    return StatusCode::FAILURE;
  }
  if (m_clusterTool.retrieve().isFailure()){
    msg(MSG::ERROR) <<"Could not get " << m_clusterTool <<endmsg; 
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
}

StatusCode MuonClusterizationAlg::execute()
{
  const TgcPrepDataContainer* tgcContainer = 0;
  if (evtStore()->retrieve(tgcContainer,m_tgcPrdLocationInput).isFailure() ) {
    ATH_MSG_WARNING("Could not find TgcPrepDataContainer at " << m_tgcPrdLocationInput);
    return StatusCode::RECOVERABLE;
  }

  const TgcPrepDataContainer* tgcContainerCluster = m_clusterTool->cluster(*tgcContainer);
  if (evtStore()->record(tgcContainerCluster,m_tgcPrdLocationOutput).isFailure() ) {
    ATH_MSG_WARNING("Could not record TgcPrepDataContainer at " << m_tgcPrdLocationOutput);
    return StatusCode::RECOVERABLE;
  }

  
  const RpcPrepDataContainer* rpcContainer = 0;
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
} // execute

StatusCode MuonClusterizationAlg::finalize()
{
  if( AthAlgorithm::finalize().isFailure() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
