/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include <vector>
#include "TMath.h"
#include "TrigL0GepPerf/CaloCellsHandler.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"


// Only update info that changes between events
void CaloCellsHandler::updateCellsInformation(const CaloCellContainer* cells){

  int caloCell_id  = -999;
  float caloCell_e = -999;

  for(auto cell: *cells){
    caloCell_id = (cell->ID().get_identifier32()).get_compact();
    caloCell_e = cell->energy();

    m_caloCells[caloCell_id].e          = caloCell_e;
    m_caloCells[caloCell_id].et         = caloCell_e * 1.0/TMath::CosH(cell->eta());
    m_caloCells[caloCell_id].time       = cell->time();
    m_caloCells[caloCell_id].quality    = cell->quality();
    m_caloCells[caloCell_id].provenance = cell->provenance();
    m_caloCells[caloCell_id].sigma      = caloCell_e / m_caloCells[caloCell_id].totalNoise;
  }
}

// Get calo cells map
void CaloCellsHandler::fillCellsInformation(const CaloCellContainer* cells){

  m_caloCells.clear();  

  for(auto cell: *cells){
    Gep::CustomCaloCell caloCell; 
    
    caloCell.e          = cell->energy();
    caloCell.et         = cell->energy() * 1.0/TMath::CosH(cell->eta());
    caloCell.time       = cell->time();
    caloCell.quality    = cell->quality();
    caloCell.provenance = cell->provenance();    
          
    float totalNoise = noiseTool_cells->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
    float electronicNoise = noiseTool_cells->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
    caloCell.totalNoise = totalNoise;
    caloCell.electronicNoise = electronicNoise;
    caloCell.sigma = caloCell.e / totalNoise;

    caloCell.isBad = cell->badcell();    
    caloCell.eta    = cell->eta();
    caloCell.phi    = cell->phi();
    caloCell.sinTh  = cell->sinTh();
    caloCell.cosTh  = cell->cosTh();
    caloCell.sinPhi = cell->sinPhi();
    caloCell.cosPhi = cell->cosPhi();
    caloCell.cotTh  = cell->cotTh();
    caloCell.x = cell->x();
    caloCell.y = cell->y();
    caloCell.z = cell->z();
    
    unsigned int samplingEnum = m_ccIdHelper->calo_sample(cell->ID());
    
    bool IsEM = m_ccIdHelper->is_em(cell->ID());
    bool IsEM_Barrel=false;
    bool IsEM_EndCap=false;
    bool IsEM_BarrelPos=false;
    bool IsEM_BarrelNeg=false;
    if(IsEM){
      IsEM_Barrel=m_ccIdHelper->is_em_barrel(cell->ID());
      if(IsEM_Barrel){
	if(m_ccIdHelper->pos_neg(cell->ID())>0) IsEM_BarrelPos=true;
      }
      IsEM_EndCap=m_ccIdHelper->is_em_endcap(cell->ID());
    }
    
    caloCell.isEM           = IsEM;
    caloCell.isEM_barrel    = IsEM_Barrel;
    caloCell.isEM_endCap    = IsEM_EndCap;
    caloCell.isEM_barrelPos = IsEM_BarrelPos;
    caloCell.isEM_barrelNeg = IsEM_BarrelNeg;  //always false?
    caloCell.isFCAL = m_ccIdHelper->is_fcal(cell->ID());
    caloCell.isHEC  = m_ccIdHelper->is_hec(cell->ID());
    caloCell.isTile = m_ccIdHelper->is_tile(cell->ID());
    
    caloCell.sampling = samplingEnum;
    caloCell.detName = CaloSampling::getSamplingName(samplingEnum);
    
    caloCell.neighbours = getNeighbours(cells, cell);
    caloCell.id = (cell->ID().get_identifier32()).get_compact(); 
    
    const CaloDetDescriptor *elt = cell->caloDDE()->descriptor();
    caloCell.layer = cell->caloDDE()->getLayer();
    
    float deta = elt->deta();
    float dphi = elt->dphi();
    
    float etamin = caloCell.eta - (0.5*deta);
    float etamax = caloCell.eta + (0.5*deta);
    
    float phimin = caloCell.phi - (0.5*dphi);
    float phimax = caloCell.phi + (0.5*dphi);
    
    caloCell.etaMin = etamin;
    caloCell.etaMax = etamax;
    caloCell.phiMin = phimin;
    caloCell.phiMax = phimax;
    caloCell.etaGranularity = deta;
    caloCell.phiGranularity = dphi;
    
    // store cells map    
    m_caloCells.insert(std::pair<unsigned int, Gep::CustomCaloCell>(caloCell.id, caloCell));
    
  }
}



// Get neighbours of a given calo cell
std::vector<unsigned int> CaloCellsHandler::getNeighbours(const CaloCellContainer* allcells, const CaloCell* acell){

  const CaloDetDescrManager* m_calo_dd_man  = CaloDetDescrManager::instance(); 
  const CaloCell_ID* m_calo_id = m_calo_dd_man->getCaloCell_ID(); 

  // get all neighboring cells
  std::vector<IdentifierHash> cellNeighbours;
  IdentifierHash cellHashID = m_calo_id->calo_cell_hash(acell->ID());
  m_calo_id->get_neighbours(cellHashID,LArNeighbours::super3D,cellNeighbours);

  std::vector<unsigned int> neighbour_ids;
  for (unsigned int iNeighbour = 0; iNeighbour < cellNeighbours.size(); ++iNeighbour) {
    const CaloCell* neighbour = allcells->findCell(cellNeighbours[iNeighbour]);
    if (neighbour) neighbour_ids.push_back((neighbour->ID().get_identifier32()).get_compact());
    else std::cout << "Couldn't access neighbour #" << iNeighbour << " for cell ID " << (acell->ID().get_identifier32()).get_compact() << std::endl;
  }

  return neighbour_ids;
}



