/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloThinCellsByClusterAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Thin calorimeter cells not associated with clusters.
 */


#include "CaloThinCellsByClusterAlg.h"
#include "CaloUtils/CaloCellList.h"
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/ReadHandle.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloThinCellsByClusterAlg::initialize()
{
  ATH_CHECK( m_cells.initialize (m_streamName) );
  ATH_CHECK( m_clusters.initialize() );
  ATH_CHECK(m_caloMgrKey.initialize());

  if (!m_samplingNames.empty()) {
    ATH_CHECK( decodeSamplings() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode CaloThinCellsByClusterAlg::execute (const EventContext& ctx) const
{
  SG::ThinningHandle<CaloCellContainer> cells (m_cells, ctx);
  cells.thinAll();

  SG::ReadHandle<xAOD::CaloClusterContainer> clusters (m_clusters, ctx);
  if(!clusters.isValid()){
     ATH_MSG_WARNING( "Collection  " << m_clusters.key()<<" is not valid");
     return StatusCode::SUCCESS;
  }


  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;


  for (const xAOD::CaloCluster* clust : *clusters) {
    const CaloClusterCellLink* cellLinks = clust->getCellLinks();
    if (!cellLinks) {
      ATH_MSG_WARNING( "  Cluster without cell links found  in collection: " << m_clusters.key()
                       << "   ===> cells cannot be written in AOD as requested ! " );
      continue;
    }

    if (cellLinks->getCellContainerLink().dataID() != m_cells.key()) {
      ATH_MSG_WARNING( "  Cluster points to cell container "
                       << cellLinks->getCellContainerLink().dataID()
                       << " which is different from the cell container being thinned: "
                       << m_cells.key() << "; cluster skipped.");
      continue;
    }

    // nb. not using range-based for here because we don't want to actually
    //     dereference the iterators.
    CaloClusterCellLink::const_iterator it = cellLinks->begin();
    CaloClusterCellLink::const_iterator end = cellLinks->end();
    for (; it != end; ++it) {
      if (it.index() >= cells->size()) {
        ATH_MSG_WARNING( "  Cell index " << it.index()
                         << " is larger than the number of cells in "
                         << m_cells.key() << " (" << cells->size() << ")" );
        continue;
      }
      cells.keep (it.index());
    }

    // add additional layer cells
    if (!m_validSamplings.empty()) {	
      double eta = clust->eta();
      double phi = clust->phi();
      double deta = clust->getClusterEtaSize() * 0.025;
      double dphi = clust->getClusterPhiSize() * 0.025;
      // get cell lists for each sampling we want to add
      for (int isamp : m_validSamplings) {
        CaloCellList cell_list (caloDDMgr,cells.cptr());
        cell_list.select (eta, phi, deta, dphi, isamp);

        ATH_MSG_DEBUG( "sampling " << isamp
                       << ", size of list = " << cell_list.ncells()
                       << ", eta = " << eta
                       << ", phi = " << phi
                       << ", deta = " << deta
                       << ", dphi = " << dphi );

        for (const CaloCell* cell : cell_list) {
          const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
          int index = cells->findIndex (cellHash);
          if (index < 0) {
            ATH_MSG_WARNING( "  Cell hash " << cellHash
                             << " not found in cell container "
                             << m_cells.key() );
            continue;
          }
          cells.keep (index);
        } // end of cell loop
      } // end of sampling loop
    } // endif m_addSamplingCells
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Decode the provided list of calorimeter samplings.
 */
StatusCode CaloThinCellsByClusterAlg::decodeSamplings()
{
  for (const std::string& name : m_samplingNames) {
    CaloSampling::CaloSample samp = CaloSampling::getSampling (name);
    if (samp == CaloSampling::Unknown) {
      ATH_MSG_ERROR (  "Calorimeter sampling" << name
                       << " is not a valid Calorimeter sampling name and will be ignored! " );
    }
    else {
      m_validSamplings.push_back (static_cast<int> (samp));
    }
  }

  std::sort (m_validSamplings.begin(), m_validSamplings.end());
  auto it = std::unique (m_validSamplings.begin(), m_validSamplings.end());
  m_validSamplings.erase (it, m_validSamplings.end());

  return StatusCode::SUCCESS;
}
