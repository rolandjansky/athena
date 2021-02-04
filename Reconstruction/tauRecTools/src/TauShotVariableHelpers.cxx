/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauShotVariableHelpers.h"

/**
 * @brief implementation of photon shot variable calculation 
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

namespace TauShotVariableHelpers {

ANA_MSG_SOURCE(msgTauShotVariableHelpers, "TauShotVariableHelpers")


const CaloCell* getNeighbour(const CaloCell* cell, 
                             const CaloClusterCellLink& links, 
                             const CaloCell_ID* calo_id, 
                             const LArNeighbours::neighbourOption& option) {
  const CaloCell* neigCell = nullptr;

  std::vector<IdentifierHash> neighHashes;
  calo_id->get_neighbours(cell->caloDDE()->calo_hash(), option, neighHashes);
  
  // Loop all the cells, and find the required neighbour cell
  for (const auto cell : links) {
    const IdentifierHash& cellHash = cell->caloDDE()->calo_hash();

    // Check whether the cell is a neighbour cell 
    for (const IdentifierHash& neighHash : neighHashes) {
      if (cellHash == neighHash) {
        neigCell = cell;
        return neigCell;
      }
    }
  }

  return neigCell;
}


std::vector<std::vector<const CaloCell*> > getCellBlock(const xAOD::PFO& shot, const CaloCell_ID* calo_id) {
  using namespace TauShotVariableHelpers::msgTauShotVariableHelpers; 

  std::vector<std::vector<const CaloCell*>> cellBlock;

  int etaSize = 0;
  if (shot.attribute(xAOD::PFODetails::PFOAttributes::tauShots_nCellsInEta, etaSize) == false) {
    ANA_MSG_WARNING("Couldn't find nCellsInEta. Return empty cell block.");
    return cellBlock;
  }
  
  int seedHash = 0;
  if (shot.attribute(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash) == false) {
    ANA_MSG_WARNING("Couldn't find seed hash. Return empty cell block.");
    return cellBlock;
  }
  
  // Initialize the cell block
  std::vector<const CaloCell*> etaLayer;
  for (int etaIndex = 0; etaIndex < etaSize; ++ etaIndex) {
    etaLayer.push_back(nullptr);
  }
  int phiSize = 2;
  for (int phiIndex = 0; phiIndex < phiSize; ++phiIndex) {
    cellBlock.push_back(etaLayer);
  }

  // Get seed cell from shot cluster
  const xAOD::CaloCluster* cluster = shot.cluster(0);
  const CaloClusterCellLink* cellLinks = cluster->getCellLinks();

  const CaloCell* seedCell = nullptr;
  for (const auto cell : *cellLinks) {
     if (cell->caloDDE()->calo_hash() != (unsigned) seedHash) continue;
     seedCell = cell;
     break;
  }
  if (seedCell==nullptr) {
    ANA_MSG_WARNING("Couldn't find seed cell in shot cluster. Return empty cell block.");
    return cellBlock;
  }
  int mediumEtaIndex = etaSize/2;
  cellBlock.at(0).at(mediumEtaIndex) = seedCell;

  // Obtain the neighbour cells in the eta direction
  // -- Next in eta
  const CaloCell* lastCell = seedCell;
  int maxDepth = etaSize - mediumEtaIndex - 1;
  for (int depth = 1; depth < maxDepth + 1; ++depth) {
    lastCell = getNeighbour(lastCell, *cellLinks, calo_id, LArNeighbours::nextInEta);
    if (lastCell != nullptr) {
      cellBlock.at(0).at(mediumEtaIndex + depth) = lastCell;
    }
    else {
      break;
    }
  }

  // -- Previous in eta
  lastCell = seedCell;
  for (int depth = 1; depth < maxDepth + 1; ++depth) {
    lastCell = getNeighbour(lastCell, *cellLinks, calo_id, LArNeighbours::prevInEta);
    if (lastCell != nullptr) {
      cellBlock.at(0).at(mediumEtaIndex - depth) = lastCell;
    }
    else {
      break;
    }
  }

  // Merged cell
  const CaloCell* mergedCell = getNeighbour(seedCell, *cellLinks, calo_id, LArNeighbours::nextInPhi);
  if (mergedCell == nullptr) {
    mergedCell = getNeighbour(seedCell, *cellLinks, calo_id, LArNeighbours::prevInPhi);
  }

  if (mergedCell != nullptr) {
    cellBlock.at(1).at(mediumEtaIndex) = mergedCell;
  
    // Obtain the neighbour cells in the eta direction
    // -- Next in eta
    lastCell = mergedCell;
    for (int depth = 1; depth < maxDepth + 1; ++depth) {
      lastCell = getNeighbour(lastCell, *cellLinks, calo_id, LArNeighbours::nextInEta);
      if (lastCell != nullptr) {
        cellBlock.at(1).at(mediumEtaIndex + depth) = lastCell;
      }
      else {
        break;
      }
    }
  
    // -- Previous in eta
    lastCell = mergedCell;
    for (int depth = 1; depth < maxDepth + 1; ++depth) {
      lastCell = getNeighbour(lastCell, *cellLinks, calo_id, LArNeighbours::prevInEta);
      if (lastCell != nullptr) {
        cellBlock.at(1).at(mediumEtaIndex - depth) = lastCell;
      }
      else {
        break;
      }
    }
  } // End of mergedCell != nullptr

  return cellBlock;
}



float ptWindow(const std::vector<std::vector<const CaloCell*>>& shotCells, 
               int windowSize, 
               const ToolHandle<IHadronicCalibrationTool>& caloWeightTool) {
  // window size should be odd and smaller than eta window of shotCells
  if (windowSize%2 != 1) return 0.;
  
  int etaSize = shotCells.at(0).size();
  if (windowSize > etaSize) return 0.;
  
  int seedIndex = etaSize/2;
  int phiSize = shotCells.size();

  float ptWindow  = 0.;
  for (int etaIndex = 0; etaIndex != etaSize; ++etaIndex) {
    if (std::abs(etaIndex-seedIndex) > windowSize/2) continue;
    
    for (int phiIndex = 0; phiIndex != phiSize; ++phiIndex) {
      const CaloCell* cell = shotCells.at(phiIndex).at(etaIndex);
      if (cell != nullptr) {
        ptWindow += cell->pt() * caloWeightTool->wtCell(cell);
      }     
    }
  }

  return ptWindow;
}

} // End of namespace TauShotVariableHelpers 

#endif
