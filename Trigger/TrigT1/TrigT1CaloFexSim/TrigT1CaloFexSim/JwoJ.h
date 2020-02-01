/**

 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

 */

#ifndef TRIGT1CALOFEXSIM_JWOJ_H
#define TRIGT1CALOFEXSIM_JWOJ_H
/*
 * Class   : JwoJ
 *
 * Author  : Myers, Ava (amyers@cern.ch)
 *
 * Date    : Oct 2018 - Created class
 *
 * Implements the Jets without Jets algorithm for MET reconstruction. Optimized for resolution (Rebecca Linck): Uses total reconstructed MET as the "soft term". Includes all helper methods for MET calculation using JwoJ
 */
#include <algorithm>
#include "TMath.h"
#include "Objects.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "JGTowerReader.h"

/**
 *@brief Separates Et into hard and soft terms depending on threshold pt. Stores hard and soft MET terms in a vector to be passed.
 *@return @c std::vector<float>
 */
std::vector<float> Run_JwoJ(const xAOD::JGTowerContainer* towers, const std::vector<TowerObject::Block> blocks, float rho, float pTcone_cut, bool useEtaBins, bool useNegTowers){
  /*
    By this definition, if we set useNegTowers to true, then we will use all the towers. If we set it to false, then we veto them after pileup subtraction. 
   */
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
    
    float block_etx = 0;
    float block_ety = 0;
    
    if(useEtaBins){
      seed_Et -= rho;
      if(!useNegTowers){
	//If we set useNegTowers = False, then we would set all their ET = 0; This is set after pileup subtraction, which is a choice that could be studied, but seems reasonable. 
	  if(seed_Et<0)seed_Et=0; 
	}
      block_etx = seed_Et*TMath::Cos(block_phi);
      block_ety = seed_Et*TMath::Sin(block_phi);

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
      if(TMath::Abs(blocks[b].Eta()) < 2.4){
	seed_Et -= rho;
	if(!useNegTowers){ 
	  //If we set useNegTowers = False, then we would set all their ET = 0;
	    if(seed_Et<0)seed_Et=0;
	  }
	block_etx = seed_Et*TMath::Cos(block_phi);
	block_ety = seed_Et*TMath::Sin(block_phi);

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
	seed_Et -= 4*rho;
	block_etx = seed_Et*TMath::Cos(block_phi);
	block_ety = seed_Et*TMath::Sin(block_phi);

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
    }
    scalar_Et += seed_Et;
    Etx_tot += block_etx;
    Ety_tot += block_ety;
  }
  float MHT = TMath::Sqrt(Htx*Htx + Hty*Hty);
  float MET = TMath::Sqrt(Etx*Etx + Ety*Ety);
  Et_tot = TMath::Sqrt(Etx_tot*Etx_tot + Ety_tot*Ety_tot);

  //convert to GeV for fitting process  
  Et_vals.push_back(scalar_Et);
  Et_vals.push_back(Htx);
  Et_vals.push_back(Hty);
  Et_vals.push_back(Etx);
  Et_vals.push_back(Ety);
  Et_vals.push_back(MHT);  
  Et_vals.push_back(MET);
  Et_vals.push_back(Et_tot);

  return Et_vals;
}

#endif

