/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauShotFinder.h"
#include "TauShotVariableHelpers.h"
#include "tauRecTools/HelperFunctions.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFO.h"

#include <boost/scoped_ptr.hpp>



TauShotFinder::TauShotFinder(const std::string& name) :
    TauRecToolBase(name) {
}



StatusCode TauShotFinder::initialize() {
  
  ATH_CHECK(m_caloWeightTool.retrieve());
  ATH_CHECK(m_caloCellInputContainer.initialize());
  ATH_CHECK(detStore()->retrieve (m_calo_id, "CaloCell_ID"));
  ATH_CHECK(m_caloMgrKey.initialize());
  return StatusCode::SUCCESS;
}



StatusCode TauShotFinder::executeShotFinder(xAOD::TauJet& tau, xAOD::CaloClusterContainer& shotClusterContainer,
					    xAOD::PFOContainer& shotPFOContainer) const {

  // Any tau needs to have shot PFO vectors. Set empty vectors before nTrack cut
  std::vector<ElementLink<xAOD::PFOContainer>> empty;
  tau.setShotPFOLinks(empty);
  
  // Only run on 0-5 prong taus 
  if (!tauRecTools::doPi0andShots(tau)) {
     return StatusCode::SUCCESS;
  }
  
  SG::ReadHandle<CaloCellContainer> caloCellInHandle( m_caloCellInputContainer );
  if (!caloCellInHandle.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << caloCellInHandle.key());
    return StatusCode::FAILURE;
  }
  const CaloCellContainer *cellContainer = caloCellInHandle.cptr();

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;
  // Select seed cells:
  // -- dR < 0.4, EM1, pt > 100
  // -- largest pt among the neighbours in eta direction 
  // -- no other seed cell as neighbour in eta direction 
  std::vector<const CaloCell*> seedCells = selectSeedCells(tau, *cellContainer, caloDDMgr);
  ATH_MSG_DEBUG("seedCells.size() = " << seedCells.size());
    
  // Construt shot by merging neighbour cells in phi direction 
  while (!seedCells.empty()) {
    // Find the neighbour in phi direction, and choose the one with highest pt
    const CaloCell* cell = seedCells.front(); 
    const CaloCell* phiNeigCell = getPhiNeighbour(*cell, seedCells);
    
    // Construct shot PFO candidate
    xAOD::PFO* shot = new xAOD::PFO();
    shotPFOContainer.push_back(shot);

    // -- Construct the shot cluster 
    xAOD::CaloCluster* shotCluster = createShotCluster(cell, phiNeigCell, *cellContainer, &shotClusterContainer);
   
    ElementLink<xAOD::CaloClusterContainer> clusElementLink;
    clusElementLink.toContainedElement( shotClusterContainer, shotCluster );
    shot->setClusterLink( clusElementLink );
   
    // -- Calculate the four momentum
    // TODO: simplify the calculation 
    if (phiNeigCell) {
      // interpolate position
      double dPhi = TVector2::Phi_mpi_pi( phiNeigCell->phi() - cell->phi());
      double ratio = phiNeigCell->pt()*m_caloWeightTool->wtCell(phiNeigCell)/(cell->pt()*m_caloWeightTool->wtCell(cell) + phiNeigCell->pt()*m_caloWeightTool->wtCell(phiNeigCell));
      float phi = cell->phi()+dPhi*ratio;
      float pt = cell->pt()*m_caloWeightTool->wtCell(cell)+phiNeigCell->pt()*m_caloWeightTool->wtCell(phiNeigCell);

      shot->setP4( (float) pt, (float) cell->eta(), (float) phi, (float) cell->m());
    }
    else {
      shot->setP4( (float) cell->pt()*m_caloWeightTool->wtCell(cell), (float) cell->eta(), (float) cell->phi(), (float) cell->m()); 
    }

    // -- Set the Attribute 
    shot->setBDTPi0Score(-9999.);
    shot->setCharge(0);
    shot->setCenterMag(0.0);
    
    shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nCellsInEta, m_nCellsInEta);
    
    const IdentifierHash seedHash = cell->caloDDE()->calo_hash();
    shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash);

    std::vector<std::vector<const CaloCell*>> cellBlock = TauShotVariableHelpers::getCellBlock(*shot, m_calo_id);

    float pt1 = TauShotVariableHelpers::ptWindow(cellBlock, 1, m_caloWeightTool);
    shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt1, pt1);
    
    float pt3 = TauShotVariableHelpers::ptWindow(cellBlock, 3, m_caloWeightTool);
    shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt3, pt3);
    
    float pt5 = TauShotVariableHelpers::ptWindow(cellBlock, 5, m_caloWeightTool);
    shot->setAttribute<float>(xAOD::PFODetails::PFOAttributes::tauShots_pt5, pt5);
    
    int nPhotons = getNPhotons(cell->eta(), pt1);
    shot->setAttribute<int>(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons, nPhotons);

    // Add Element link to the shot PFO container
    ElementLink<xAOD::PFOContainer> PFOElementLink;
    PFOElementLink.toContainedElement(shotPFOContainer, shot);
    tau.addShotPFOLink(PFOElementLink);
    
    // Remove used cells from list
    auto cellIndex = std::find(seedCells.begin(), seedCells.end(), cell);
    seedCells.erase(cellIndex);
    if (phiNeigCell) {
      cellIndex = std::find(seedCells.begin(), seedCells.end(), phiNeigCell);
      seedCells.erase(cellIndex);
    }
  } // Loop over seed cells
  
  return StatusCode::SUCCESS;
}



int TauShotFinder::getEtaBin(float eta) const {
  float absEta=std::abs(eta);
  
  if (absEta < 0.80) {
    return 0; // Central Barrel
  }
  if (absEta<1.39) {
    return 1; // Outer Barrel
  }
  if (absEta<1.51) {
    return 2; // Crack region
  }
  if (absEta<1.80) {
    return 3; // Endcap, fine granularity
  }
  return 4; // Endcap, coarse granularity
}



int TauShotFinder::getNPhotons(float eta, float energy) const {
  int etaBin = getEtaBin(eta);
  
  // No photons in crack region
  if(etaBin==2) return 0;

  const std::vector<float>& minPtCut = m_minPtCut.value();
  const std::vector<float>& doubleShotCut = m_doubleShotCut.value();
  ATH_MSG_DEBUG("etaBin = " << etaBin  << ", energy = " << energy);
  ATH_MSG_DEBUG("MinPtCut: " << minPtCut.at(etaBin) << "DoubleShotCut: " << doubleShotCut.at(etaBin));

  if (energy < minPtCut.at(etaBin)) return 0;
  if (energy > doubleShotCut.at(etaBin)) return 2;
  return 1;
}



std::vector<const CaloCell*> TauShotFinder::selectCells(const xAOD::TauJet& tau,
                                                        const CaloCellContainer& cellContainer,
                                                        const CaloDetDescrManager* detMgr) const {
  // Get only cells within dR < 0.4
  // -- TODO: change the hardcoded 0.4
  std::vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
  emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
  boost::scoped_ptr<CaloCellList> cellList(new CaloCellList(detMgr, &cellContainer,emSubCaloBlocks)); 
  // -- FIXME: tau p4 is corrected to point at tau vertex, but the cells are not 
  cellList->select(tau.eta(), tau.phi(), 0.4); 

  std::vector<const CaloCell*> cells;
  for (const CaloCell* cell : *cellList) {
    // Require cells above 100 MeV
    // FIXME: cells are not corrected to point at tau vertex
    if (cell->pt() * m_caloWeightTool->wtCell(cell) < 100.) continue;
    
    // Require cells in EM1 
    int sampling = cell->caloDDE()->getSampling();
    if( !( sampling == CaloCell_ID::EMB1 || sampling == CaloCell_ID::EME1 ) ) continue;
    
    cells.push_back(cell);
  }

  return cells;
} 



std::vector<const CaloCell*> TauShotFinder::selectSeedCells(const xAOD::TauJet& tau,
                                                            const CaloCellContainer& cellContainer,
                                                            const CaloDetDescrManager* detMgr) const {

  // Apply pre-selection of the cells
  std::vector<const CaloCell*> cells = selectCells(tau, cellContainer,detMgr);
  std::sort(cells.begin(),cells.end(),ptSort(*this));

  std::vector<const CaloCell*> seedCells;  
  std::set<IdentifierHash> seedCellHashes;

  // Loop the pt sorted cells, and select the seed cells
  for (const CaloCell* cell: cells) {
    const IdentifierHash cellHash = cell->caloDDE()->calo_hash();

    std::vector<IdentifierHash> nextEtaHashes;
    m_calo_id->get_neighbours(cellHash, LArNeighbours::nextInEta, nextEtaHashes);
    std::vector<IdentifierHash> prevEtaHashes;
    m_calo_id->get_neighbours(cellHash, LArNeighbours::prevInEta, prevEtaHashes);
   
    std::vector<IdentifierHash> neighHashes = nextEtaHashes; 
    neighHashes.insert(neighHashes.end(),prevEtaHashes.begin(),prevEtaHashes.end()); 
   
    // Check whether it is a seed cell
    bool status = true;
    for (const IdentifierHash& neighHash : neighHashes) {
      // Seed cells must not have seed cells as neighbours
      // TODO: maybe this requirement can be removed
      if (seedCellHashes.find(neighHash) != seedCellHashes.end()) {
        status = false;
        break;
      }
      
      // Pt of seed cells must be larger than neighbours'
      const CaloCell* neighCell = cellContainer.findCell(neighHash);
      if (!neighCell) continue;
      if (neighCell->pt() * m_caloWeightTool->wtCell(neighCell) >= cell->pt() * m_caloWeightTool->wtCell(cell)) {
        status = false;
        break;
      }
    } // End of the loop of neighbour cells
    
    if (!status) continue; 
    
    seedCells.push_back(cell); 
    seedCellHashes.insert(cellHash);
  } // End of the loop of cells

  return seedCells; 
} 



bool TauShotFinder::isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash) const {
  std::vector<IdentifierHash> neigHashes;
 
  // Next cell in phi direction 
  m_calo_id->get_neighbours(cell1Hash,LArNeighbours::nextInPhi,neigHashes);
  if (neigHashes.size() > 1) {
    ATH_MSG_DEBUG(cell1Hash << " has " << neigHashes.size()  <<  " neighbours in the next phi direction !"); 
  }
  if (std::find(neigHashes.begin(), neigHashes.end(), cell2Hash) != neigHashes.end()) {
    return true;
  }
  
  // Previous cell in phi direction
  m_calo_id->get_neighbours(cell1Hash,LArNeighbours::prevInPhi,neigHashes);
  if (neigHashes.size() > 1) {
    ATH_MSG_DEBUG(cell1Hash << " has " << neigHashes.size()  <<  " neighbours in the previous phi direction !"); 
  }
  return std::find(neigHashes.begin(), neigHashes.end(), cell2Hash) != neigHashes.end();
}



const CaloCell* TauShotFinder::getPhiNeighbour(const CaloCell& seedCell, 
                                               const std::vector<const CaloCell*>& seedCells) const {

  const IdentifierHash seedHash = seedCell.caloDDE()->calo_hash();
 
  // Obtain the neighbour cells in the phi direction 
  std::vector<const CaloCell*> neighCells;
  for (const CaloCell* neighCell : seedCells) {
    if (neighCell == &seedCell) continue;
    
    IdentifierHash neighHash = neighCell->caloDDE()->calo_hash();
    if (this->isPhiNeighbour(seedHash, neighHash)) {
      neighCells.push_back(neighCell);
    }
  }
  std::sort(neighCells.begin(),neighCells.end(),ptSort(*this)); 

  // Select the one with largest pt
  const CaloCell* phiNeigCell = nullptr;
  if (!neighCells.empty()) {
    phiNeigCell = neighCells[0];
  } 

  return phiNeigCell;
}



std::vector<const CaloCell*> TauShotFinder::getEtaNeighbours(const CaloCell& cell,
                                                             const CaloCellContainer& cellContainer, 
							                                 int maxDepth) const {
    std::vector<const CaloCell*> cells;
    
    // Add neighbours in next eta direction
    this->addEtaNeighbours(cell, cellContainer, cells, 0, maxDepth, true);
    // Add neighbours in previous eta direction
    this->addEtaNeighbours(cell, cellContainer, cells, 0, maxDepth, false);

    return cells; 
}



void TauShotFinder::addEtaNeighbours(const CaloCell& cell,
                                     const CaloCellContainer& cellContainer,
                                     std::vector<const CaloCell*>& cells,
                                     int depth,
                                     int maxDepth,
                                     bool next) const {
  ++depth; 
  
  if (depth > maxDepth) return;

  const IdentifierHash cellHash = cell.caloDDE()->calo_hash();
  
  std::vector<IdentifierHash> neigHashes;
  if (next) {
    m_calo_id->get_neighbours(cellHash,LArNeighbours::nextInEta,neigHashes);
  }
  else {
    m_calo_id->get_neighbours(cellHash,LArNeighbours::prevInEta,neigHashes);
  }

  for (const IdentifierHash& hash : neigHashes) {
    const CaloCell* newCell = cellContainer.findCell(hash);
    
    if (!newCell) continue;
    
    cells.push_back(newCell);
    this->addEtaNeighbours(*newCell, cellContainer, cells, depth, maxDepth, next);
  
    if (neigHashes.size() > 1) {
      ATH_MSG_DEBUG(cellHash << " has " << neigHashes.size()  <<  " neighbours in the eta direction !"); 
      break; 
    }
  } 
}



xAOD::CaloCluster* TauShotFinder::createShotCluster(const CaloCell* cell, 
						    const CaloCell* phiNeigCell, 
						    const CaloCellContainer& cellContainer,
						    xAOD::CaloClusterContainer* clusterContainer) const {
    
  xAOD::CaloCluster* shotCluster = CaloClusterStoreHelper::makeCluster(clusterContainer,&cellContainer);
  
  int maxDepth = (m_nCellsInEta - 1) / 2;

  std::vector<const CaloCell*> windowNeighbours = this->getEtaNeighbours(*cell, cellContainer, maxDepth);
  if (phiNeigCell) {
    std::vector<const CaloCell*> mergeCells = this->getEtaNeighbours(*phiNeigCell, cellContainer, maxDepth);
    windowNeighbours.push_back(phiNeigCell);
    windowNeighbours.insert(windowNeighbours.end(), mergeCells.begin(), mergeCells.end());
  }

  shotCluster->getOwnCellLinks()->reserve(windowNeighbours.size()+1);
  const IdentifierHash seedHash = cell->caloDDE()->calo_hash();
  shotCluster->addCell(cellContainer.findIndex(seedHash), 1.);
  
  for (const CaloCell* cell : windowNeighbours) {
    shotCluster->addCell(cellContainer.findIndex(cell->caloDDE()->calo_hash()), 1.0);
  }

  CaloClusterKineHelper::calculateKine(shotCluster,true,true);

  return shotCluster;
} 



TauShotFinder::ptSort::ptSort( const TauShotFinder& info ) : m_info(info) { } 
bool TauShotFinder::ptSort::operator()( const CaloCell* cell1, const CaloCell* cell2 ){
  double pt1 = cell1->pt()*m_info.m_caloWeightTool->wtCell(cell1);
  double pt2 = cell2->pt()*m_info.m_caloWeightTool->wtCell(cell2);
  return pt1 > pt2;  
}

#endif
