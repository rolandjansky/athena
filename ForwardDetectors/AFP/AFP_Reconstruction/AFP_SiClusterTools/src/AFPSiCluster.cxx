/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_SiClusterTools/AFPSiCluster.h"

AFPSiCluster::AFPSiCluster(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

AFPSiCluster::~AFPSiCluster()
{
}

StatusCode AFPSiCluster::initialize()
{
  ATH_MSG_DEBUG("in AFPSiCluster::initialize()");
  CHECK( AthReentrantAlgorithm::initialize() );
  
  // reco track tool
  if (m_clusterRecoTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_clusterRecoTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_clusterRecoTool );
  }
  
  CHECK( m_clusterContainerKey.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode AFPSiCluster::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode AFPSiCluster::execute(const EventContext& ctx) const
{
  // reconstruct clusters
  auto clustr=std::make_unique<xAOD::AFPSiHitsClusterContainer>();
  auto clustrAux=std::make_unique<xAOD::AFPSiHitsClusterAuxContainer>();
  clustr->setStore(clustrAux.get());

  if (m_clusterRecoTool->clusterHits(clustr, ctx).isFailure() ) {
    ATH_MSG_WARNING("Pixel clusters creation failed. Aborting cluster reconstruction.");
    
    // create empty containers first
    auto clustr2=std::make_unique<xAOD::AFPSiHitsClusterContainer>();
    auto clustr2Aux=std::make_unique<xAOD::AFPSiHitsClusterAuxContainer>();
    clustr2->setStore(clustr2Aux.get());
    SG::WriteHandle<xAOD::AFPSiHitsClusterContainer> cluster2Container(m_clusterContainerKey, ctx);
    ATH_CHECK( cluster2Container.record(std::move(clustr2), std::move(clustr2Aux)) );
    
    return StatusCode::SUCCESS;
  }

  SG::WriteHandle<xAOD::AFPSiHitsClusterContainer> clusterContainer(m_clusterContainerKey, ctx);
  ATH_CHECK( clusterContainer.record(std::move(clustr), std::move(clustrAux)) );
  
  return StatusCode::SUCCESS;
}
