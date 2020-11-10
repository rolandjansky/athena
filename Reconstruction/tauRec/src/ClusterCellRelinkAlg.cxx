/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
*/

#include "ClusterCellRelinkAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

/**
 * @brief Gaudi initialize method.
 */
StatusCode ClusterCellRelinkAlg::initialize()
{
  ATH_CHECK( m_cells.initialize() );
  ATH_CHECK( m_clustersInput.initialize() );
  ATH_CHECK( m_clustersOutput.initialize() );
  ATH_CHECK( m_cellLinksOutput.initialize() );

  return StatusCode::SUCCESS;
}

/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode ClusterCellRelinkAlg::execute (const EventContext& ctx) const
{
  SG::ReadHandle<CaloCellContainer> cells (m_cells, ctx);
  if(!cells.isValid()) {
    ATH_MSG_WARNING( "Collection " << m_cells.key() << " is not valid" );
    return StatusCode::SUCCESS;
  }
  
  // input clusters
  SG::ReadHandle<xAOD::CaloClusterContainer> clustersInput (m_clustersInput, ctx);
  if(!clustersInput.isValid()) {
    ATH_MSG_WARNING( "Collection " << m_clustersInput.key() << " is not valid" );
    return StatusCode::SUCCESS;
  }

  // create new clusters made of cells from another container
  SG::WriteHandle<xAOD::CaloClusterContainer> clustersOutputHandle (m_clustersOutput, ctx);
  ATH_CHECK( clustersOutputHandle.record(std::make_unique<xAOD::CaloClusterContainer>(), std::make_unique<xAOD::CaloClusterAuxContainer>()) );
  xAOD::CaloClusterContainer* clustersOutputContainer = clustersOutputHandle.ptr();
  clustersOutputContainer->reserve(clustersInput->size());

  for (const xAOD::CaloCluster* oldCluster : *clustersInput) { 
    xAOD::CaloCluster* newCluster = new xAOD::CaloCluster();
    clustersOutputContainer->push_back(newCluster);
    *newCluster = *oldCluster;
    newCluster->addCellLink(new CaloClusterCellLink(cells.ptr()));

    const CaloClusterCellLink* cellLinks = oldCluster->getCellLinks();
    CaloClusterCellLink::const_iterator lnk_it=cellLinks->begin();
    CaloClusterCellLink::const_iterator lnk_it_e=cellLinks->end();
    newCluster->getOwnCellLinks()->reserve(cellLinks->size());

    for (;lnk_it!=lnk_it_e;++lnk_it) {
      const IdentifierHash cellHash = (*lnk_it)->caloDDE()->calo_hash();
      const int newIdx=cells->findIndex(cellHash);
      newCluster->addCell(newIdx, lnk_it.weight());
    }
  }

  SG::WriteHandle<CaloClusterCellLinkContainer> cellLinksOutputHandle (m_cellLinksOutput, ctx);
  ATH_CHECK( CaloClusterStoreHelper::finalizeClusters(cellLinksOutputHandle, clustersOutputContainer) );

  return StatusCode::SUCCESS;
}
