/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloTowerxAODFromClusters.h"


#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"

#ifndef CALOTOWER_MAX_CELL_COUNT
#define CALOTOWER_MAX_CELL_COUNT 200000
#endif


CaloTowerxAODFromClusters::CaloTowerxAODFromClusters(const std::string& name,ISvcLocator* pSvcLocator)
  : CaloTowerxAODAlgoBase(name,pSvcLocator)
  , m_inputClusterContainerKey("CaloCalTopoClusters")
{
  declareProperty("InputClusterContainer", m_inputClusterContainerKey );
}

CaloTowerxAODFromClusters::~CaloTowerxAODFromClusters()
{ }

StatusCode CaloTowerxAODFromClusters::initialize() {
  ATH_CHECK(m_inputClusterContainerKey.initialize());
  ATH_CHECK( initBase() );
  return StatusCode::SUCCESS;
}

StatusCode CaloTowerxAODFromClusters::execute(const EventContext& ctx) const
{ 
  const CellToTowerVec& cellToTower = getIndexCache(ctx);
  if(cellToTower.empty()) {
    ATH_MSG_ERROR( "Failed to compute the index cache");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::CaloClusterContainer> inputClusterContainer(m_inputClusterContainerKey, ctx);

  SG::WriteHandle<xAOD::CaloTowerContainer> caloTowerContainer =
    this->makeContainer(ctx);
  if (!caloTowerContainer.isValid())
    return StatusCode::FAILURE;
  
  std::bitset< CALOTOWER_MAX_CELL_COUNT> addedCellsMap;

  for (const xAOD::CaloCluster* it_cluster : *inputClusterContainer) {
    const CaloClusterCellLink* cellLinks=it_cluster->getCellLinks();
    if (!cellLinks) {
      ATH_MSG_ERROR("Cluster without cell links found  in collection: " << inputClusterContainer.name());
      return StatusCode::FAILURE;
    }

    for (const CaloCell* cell : *cellLinks) {
      //Ask cell for it's hash
      const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
      //Check it this cell is already part of reducedCellContainer
      if (!addedCellsMap.test(cellHash)) {
	addedCellsMap.set(cellHash);
	assert(cellHash<cellToTower.size());
	const auto& c2ts=cellToTower[cellHash];
	//Remember: A cell can contribute to more than one tower!
	for (const cellToTower_t& c2t : c2ts) {
	  (*caloTowerContainer)[c2t.m_towerIdx]->addEnergy(cell->e()*c2t.m_weight);
	}//end loop over towers coverd by this cell
      }//end cell has not been added yet
    }//end loop over cells in this cluster
  }//end loop over clusters
  return StatusCode::SUCCESS;
}
 
StatusCode CaloTowerxAODFromClusters::finalize()
{ return StatusCode::SUCCESS; }

