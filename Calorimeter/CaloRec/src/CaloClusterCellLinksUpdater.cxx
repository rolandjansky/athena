///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterCellLinksUpdater.cxx 
// Implementation file for class CaloClusterCellLinksUpdater
// Author: Walter Lampl
/////////////////////////////////////////////////////////////////// 

// Calo includes
#include "CaloClusterCellLinksUpdater.h"
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/errorcheck.h"

StatusCode CaloClusterCellLinksUpdater::initialize() {

  // Create a set of cluster names (clear out duplicate names):
  std::set<std::string> clusterKeysSet;
  for (const SG::ReadHandleKey<xAOD::CaloClusterContainer>& clusterKey : m_clusterKeys) {
    clusterKeysSet.insert(clusterKey.key());
  }

  m_clusterKeys.clear();
  for (std::string clusterKey : clusterKeysSet) {
    m_clusterKeys.push_back(clusterKey);
  }

  ATH_CHECK( m_clusterKeys.initialize() );
  ATH_CHECK( m_caloCellContainerKey.initialize() );

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Will update links for clusters in container(s): ";
    for (const SG::ReadHandleKey<xAOD::CaloClusterContainer>& clusterKey : m_clusterKeys)
      msg(MSG::INFO) << clusterKey.key() << ", ";
    msg(MSG::INFO) << "based on the CaloCellContainer " << m_caloCellContainerKey.key() << endmsg;
  }
  return StatusCode::SUCCESS;
}


StatusCode CaloClusterCellLinksUpdater::execute(const EventContext& ctx) const {
  
  //retrieve new CaloCellContainer
  SG::ReadHandle<CaloCellContainer> cellCont(m_caloCellContainerKey, ctx);
  ATH_CHECK( cellCont.isValid() );

  //Loop over cluster names
  for (const SG::ReadHandleKey<xAOD::CaloClusterContainer>& clusterKey : m_clusterKeys) {

    ATH_MSG_DEBUG("Now working on cluster container with key " << clusterKey.key());

    SG::ReadHandle<xAOD::CaloClusterContainer> clusterCont(clusterKey, ctx);

    //Loop over clusters in cluster container
    for (auto cluster : *clusterCont) {
      //Get CaloCellLinkContainer:
      const CaloClusterCellLink* cellLinks_cst=cluster->getCellLinks();
      if (!cellLinks_cst) {
	ATH_MSG_WARNING("Container " << clusterKey.key() << ", encountered cluster without cell links.");
	continue;
      }
      //cast-away const-ness. Note that this algo is a friend of <CaloClusterCellLink
      CaloClusterCellLink* cellLinks=const_cast<CaloClusterCellLink*>(cellLinks_cst);
      
      //use non-const iterators to loop over cells in cluster
      CaloClusterCellLink::iterator lnk_it=cellLinks->begin();
      CaloClusterCellLink::iterator lnk_it_e=cellLinks->end();
      for (;lnk_it!=lnk_it_e;++lnk_it) {
        if (lnk_it.index() >= cellLinks_cst->getCellContainer()->size()) {
          // FIXME: cluster links are screwed up in ESDs due to changes
          //        in output algorithm sequencing; see ATR-19778.
          //        For now, add some protection to avoid a crash in that case,
          //        but this isn't a real fix.
          continue;
        }
	//Ask cell for it's hash
	const IdentifierHash cellHash = (*lnk_it)->caloDDE()->calo_hash();
	const int newIdx=cellCont->findIndex(cellHash);
	if (newIdx<0) {
	  ATH_MSG_ERROR( "Cell with hash " << cellHash << " not found in new cell container with key "
                         << m_caloCellContainerKey.key() );
	  return StatusCode::FAILURE; //FIXME: More robust handling
	}
	
	//Update link:
	ATH_MSG_VERBOSE("Cell with hash " << cellHash << ": Replace index " << lnk_it.m_it->first << " with " << newIdx);
	lnk_it.m_it->first=newIdx;
      }//end loop over cells in this cluster

      //Replace cell container ptr (DataLink):
      ATH_MSG_VERBOSE("Replacing DataLink to CellContainer");
      cellLinks->m_cellCont=cellCont.cptr();
      //cellLinks->m_cellCont=DataLink<CaloCellContainer>(cellCont);
    }//end loop over clusters in cluster container
  }//end loop over cluster names
  
  return StatusCode::SUCCESS;
}

