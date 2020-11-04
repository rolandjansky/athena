/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
*/

#include "tauRec/TauCellThinningAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ThinningHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "tauRecTools/HelperFunctions.h"
#include <vector>

/**
 * @brief Gaudi initialize method.
 */
StatusCode TauCellThinningAlg::initialize()
{
  ATH_CHECK( m_cells.initialize(m_streamName) );
  ATH_CHECK( m_cellLinks.initialize(m_streamName) );
  ATH_CHECK( m_taus.initialize() );

  return StatusCode::SUCCESS;
}

/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode TauCellThinningAlg::execute (const EventContext& ctx) const
{
  SG::ThinningHandle<CaloCellContainer> cells (m_cells, ctx);
  cells.thinAll();
  
  SG::ThinningHandle<CaloClusterCellLinkContainer> cellLinkHandle (m_cellLinks, ctx);
  cellLinkHandle.thinAll();

  SG::ReadHandle<xAOD::TauJetContainer> taus (m_taus, ctx);
  if (!taus.isValid()) {
    ATH_MSG_WARNING( "Collection " << m_taus.key() << " is not valid" );
    return StatusCode::SUCCESS;
  }

  for (const xAOD::TauJet* tau : *taus) {
    std::vector<const xAOD::CaloCluster*> clusters;
    ATH_CHECK( tauRecTools::GetJetClusterList(tau->jet(), clusters, m_useSubtractedCluster) );

    for (const xAOD::CaloCluster* cluster : clusters) {
      // only keep cells for clusters within 0.2
      if (cluster->p4().DeltaR(tau->p4())>0.2) continue;

      const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
      if (!cellLinks) {
	ATH_MSG_WARNING( "Cluster without cell links found! Cells cannot be written in xAOD." );
	continue;
      }
      
      if (cellLinks->getCellContainerLink().dataID() != m_cells.key()) {
	ATH_MSG_WARNING( "Cluster points to cell container "
			 << cellLinks->getCellContainerLink().dataID()
			 << " which is different from the cell container being thinned: "
			 << m_cells.key() << "; cluster skipped.");
	continue;
      }

      // cluster cell link thinning
      CaloClusterCellLinkContainer::const_iterator cellLinks_it = std::find(cellLinkHandle->begin(), cellLinkHandle->end(), cellLinks);
      if(cellLinks_it != cellLinkHandle->end()) {
	size_t link_index = std::distance(cellLinkHandle->begin(), cellLinks_it);
	cellLinkHandle.keep(link_index);
      }
      else {
	ATH_MSG_WARNING( "Could not find cluster cell link in " << m_cellLinks.key() << ", won't be saved in xAOD." );
      }

      // cell thinning
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
    }
  }
  
  return StatusCode::SUCCESS;
}
