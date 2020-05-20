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
#include "AthenaBaseComps/AthCheckMacros.h"
/**
 *@brief Separates Et into hard and soft terms depending on threshold pt. Stores hard and soft MET terms in a vector to be passed.
 *@return @c std::vector<float>
 */
std::vector<float> Run_JwoJ(const xAOD::JGTowerContainer* towers, const std::vector<TowerObject::Block> blocks, float pTcone_cut, bool useRho, float RhoA, float RhoB, float RhoC, bool useNegTowers){
  /*
    By this definition, if we set useNegTowers to true, then we will use all the towers. If we set it to false, then we veto them after pileup subtraction. 
   */

  pTcone_cut*=Gaudi::Units::GeV;
  std::vector<float> Et_vals;
  float Htx = 0; float Hty = 0;
  float Etx = 0; float Ety = 0;
  float Et_tot = 0; float Etx_tot = 0; float Ety_tot = 0;
  
  float scalar_Et = 0;
  
  for(unsigned int b = 0; b < blocks.size(); b++){

    float pt_block = blocks[b].Pt();//pT of the gBlock 
    float seed_Et = (towers->at(blocks[b].seedIndex()))->et();
    float seed_phi = (towers->at(blocks[b].seedIndex()))->phi();
    float seed_eta = (towers->at(blocks[b].seedIndex()))->eta();
    float Area = GTowerArea(seed_eta); 
    std::string FPGA = GFEX_pFPGA(seed_eta);
    float rho = 0; 
    if(FPGA=="A") rho = RhoA; 
    else if(FPGA=="B") rho = RhoB;
    else if(FPGA=="C") rho = RhoC; 
    else ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,"JwoJ") << "NO FPGA FOUND"; 

    if(!useNegTowers && seed_Et < 0)continue; 
    
    //First compute global variables 
    scalar_Et+=seed_Et; 
    Etx_tot += seed_Et*TMath::Cos(seed_phi); 
    Ety_tot += seed_Et*TMath::Sin(seed_phi); 

    if(pt_block > pTcone_cut){
      if(useRho) {
	//Only apply rho correction for hard term 
	seed_Et -= rho*Area; 
	if(seed_Et < 0 ) seed_Et = 0; 
      }
      Htx += seed_Et*TMath::Cos(seed_phi);;
      Hty += seed_Et*TMath::Sin(seed_phi);;
    }
    else{
      Etx += seed_Et*TMath::Cos(seed_phi);;
      Ety += seed_Et*TMath::Sin(seed_phi);;
    }

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

