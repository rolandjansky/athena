/*

 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

JwoJ.h: Implementation of the Jets without Jets algorithm
Author: Ava Myers (amyers@cern.ch)
Optimized for resolution (Rebecca Linck) -> uses total reconstructed MET as "soft term"

Includes all helper methods for MET calculation using JwoJ

Methods: 

RunJwoJ: Separates Et into "hard" and "soft" terms depending on value
         Stores the hard and soft MET terms in global vector entries for MET calculation in METAlgs.cxx

BuildBlocksFromTowers: Takes JGTowers from a JGTowerContainer and combines them into gBlocks. Used for threshold on the "cone"(gBlock) pt

 */

#include <algorithm>
#include "TMath.h"
#include "Objects.h"
#include "GaudiKernel/SystemOfUnits.h"
//--------------------------------------------------------------------
void BuildBlocksFromTowers(std::vector<TowerObject::Block>& blocks, const xAOD::JGTowerContainer towers, const int blockRows, const int blockCols, bool useNegTowers){
  
  blocks.clear();
  TowerObject::TowerGrid grid = TowerObject::TowerGrid(towers);
  
  for(const xAOD::JGTower* seed: towers){
    int seedIndex = std::find(towers.begin(), towers.end(), seed) - towers.begin();
    
    std::vector<int> neighbors = grid.neighbors(*seed, blockRows, blockCols);
    float seed_Et = seed->et();
    if(!useNegTowers) seed_Et = TMath::Abs(seed_Et);
    double block_area(0.0);
    double block_pt(seed_Et);
    double neighbor_pt = 0;
    
    for(const int& neighborIndex: neighbors){
      const xAOD::JGTower* neighbor = towers.at(neighborIndex);
      block_area += neighbor->deta()*neighbor->dphi();
      neighbor_pt = neighbor->et();
      if(!useNegTowers) neighbor_pt = TMath::Abs(neighbor_pt);
      block_pt += neighbor_pt;
    }
    
    TowerObject::Block block(block_pt, seed->eta(), seed->phi(), 0.0);
    block.seedIndex(seedIndex);
    block.numEta(blockCols);
    block.numPhi(blockRows);
    block.area(block_area);
    block.numConstituents(neighbors.size());
    
    blocks.push_back(block);
  }
  
  std::sort(blocks.rbegin(), blocks.rend());
}
std::vector<float> Run_JwoJ(const xAOD::JGTowerContainer* towers, float pTcone_cut, bool useNegTowers){
  
  std::vector<TowerObject::Block> blocks;
  BuildBlocksFromTowers(blocks, *towers, 3, 3, useNegTowers);
  
  pTcone_cut*=Gaudi::Units::GeV;
  std::vector<float> Et_vals;
  float Ht = 0; float Htx = 0; float Hty = 0;
  float Et = 0; float Etx = 0; float Ety = 0;
  float Et_tot = 0; float Etx_tot = 0; float Ety_tot = 0;
  
  float scalar_Et = 0;
  
  for(unsigned int b = 0; b < blocks.size(); b++){
    float block_phi = blocks[b].Phi();
    float pt_cone = blocks[b].Pt();
    float seed_Et = (towers->at(blocks[b].seedIndex()))->et();
    
    float block_etx = seed_Et*cos(block_phi);
    float block_ety = seed_Et*sin(block_phi);
    
    if(TMath::Abs(blocks[b].Eta()) < 2.4){
      if(pt_cone > pTcone_cut){
	Ht += seed_Et;
	Htx += block_etx;
	Hty += block_ety;
      }else{
	Et += seed_Et;
	Etx += block_etx;
	Ety += block_ety;
      }      
    }else{
      if(pt_cone > 4*pTcone_cut){
	Ht += seed_Et;
	Htx += block_etx;
	Hty += block_ety;
      }
      else{
	Et += seed_Et;
	Etx += block_etx;
	Ety += block_ety;
      }
    }
      scalar_Et += seed_Et;
      Etx_tot += block_etx;
      Ety_tot += block_ety;
  }
  float MHT = TMath::Sqrt(Htx*Htx + Hty*Hty);
  float MET = TMath::Sqrt(Etx*Etx + Ety*Ety);
  Et_tot = TMath::Sqrt(Etx_tot*Etx_tot + Ety_tot*Ety_tot);

  //convert to GeV for fitting process  
  Et_vals.push_back(scalar_Et*0.001);
  Et_vals.push_back(MHT*0.001);
  Et_vals.push_back(MET*0.001);
  Et_vals.push_back(Et_tot*0.001);
  
  return Et_vals;
}



