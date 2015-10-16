/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloRec/CaloTowerxAODFromClusters.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

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
  return initBase();
}

StatusCode CaloTowerxAODFromClusters::execute()       
{ 
  const xAOD::CaloClusterContainer* inputClusterContainer;
  ATH_CHECK(evtStore()->retrieve(inputClusterContainer, m_inputClusterContainerKey));


  xAOD::CaloTowerContainer* caloTowerContainer=this->makeContainer();
  
  std::bitset< CALOTOWER_MAX_CELL_COUNT> addedCellsMap;

  for (auto it_cluster : (*inputClusterContainer)) {
    const CaloClusterCellLink* cellLinks=it_cluster->getCellLinks();
    if (!cellLinks) {
      ATH_MSG_ERROR("Cluster without cell links found  in collection: " << m_inputClusterContainerKey);
      return StatusCode::FAILURE;
    }

    for (const CaloCell* cell : *cellLinks) {
      //Ask cell for it's hash
      const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
      //Check it this cell is already part of reducedCellContainer
      if (!addedCellsMap.test(cellHash)) {
	addedCellsMap.set(cellHash);
	assert(cellHash<m_cellToTower.size());
	const auto& c2ts=m_cellToTower[cellHash];
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

