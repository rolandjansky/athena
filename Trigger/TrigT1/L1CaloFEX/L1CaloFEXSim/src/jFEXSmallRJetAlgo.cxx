/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXSmallRJetAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************  
#include <iostream>
#include <vector>
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXSmallRJetAlgo::jFEXSmallRJetAlgo(const std::string& type, const std::string& name, const IInterface* parent):
   AthAlgTool(type, name, parent)   
  {
  declareInterface<IjFEXSmallRJetAlgo>(this);
  }

/** Destructor */
LVL1::jFEXSmallRJetAlgo::~jFEXSmallRJetAlgo()
{
}
StatusCode LVL1::jFEXSmallRJetAlgo::initialize()
{
   ATH_CHECK(m_jFEXSmallRJetAlgo_jTowerContainerKey.initialize());

   return StatusCode::SUCCESS;

}

//calls container for TT
StatusCode LVL1::jFEXSmallRJetAlgo::safetyTest(){

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey);

  if(! jk_jFEXSmallRJetAlgo_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve  jk_jFEXSmallRJetAlgo_jTowerContainer " << m_jFEXSmallRJetAlgo_jTowerContainerKey.key());

    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

void LVL1::jFEXSmallRJetAlgo::setup(int inputTable[5][5]) {

  std::copy(&inputTable[0][0], &inputTable[0][0] + 25, &m_jFEXalgoTowerID[0][0]);

}
     
//TOB for Small R Jet Algo
/*LVL1::jFEXSmallRJetAlgoTOB * LVL1::jFEXSmallRJetAlgo::getSmallRJetTOB()
{
  jFEXSmallRJetAlgoTOB *tob = new jFEXSmallRJetTOB();
//  unsigned int et = getTTowerET();
//  unsigned int phi = getTTowerET;
//  unsigned int eta = getRealPhi();
//  tob->setEta();
//  tob->setPhi();
//  tob->setET(et);
 // tob->setRes();
 // tob->setSat();
  return tob;
}

*/


//Gets the ET for the TT. This ET is EM + HAD
unsigned int LVL1::jFEXSmallRJetAlgo::getTTowerET(){
 SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

 const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[2][2]);
 unsigned int et = tmpTower->getTotalET();  
 return et;
}
//Gets Phi of the TT
unsigned int LVL1::jFEXSmallRJetAlgo::getRealPhi() {  

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);
  
  unsigned int phi = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[2][2])->phi();
  return phi;
}
//Gets Eta of the TT
unsigned int LVL1::jFEXSmallRJetAlgo::getRealEta() { 

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

  unsigned int eta = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[2][2])->eta();
  return eta;
}
//this function calculates seed for a given TT
void LVL1::jFEXSmallRJetAlgo::buildSeeds()
{
  ATH_MSG_DEBUG("--------jFEXSmallRJetAlgo::buildsSeeds ----------------");
  m_seedSet = false;
  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey);
//const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[2][2]);

  for(int mphi = 0; mphi < 5; mphi++){
    for(int meta = 0; meta<5; meta++){
      int et_tmp = 0;
      int seedTotalET = 0;
  
      for(int ieta = -1; ieta < 2; ieta++){
        for(int iphi = -1; iphi < 2; iphi++){
          const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[meta + ieta][mphi + iphi]);
          //for that TT, build the seed
          //ATH_MSG_DEBUG("check my phi eta tower index, iphi, ieta: " <<iphi<< " "<< ieta<<" " <<m_jFEXalgoTowerID[ieta][iphi]);
	  //here we sum TT ET to calculate seed	    	
	  et_tmp = tmpTower->getTotalET();  
          seedTotalET += et_tmp;
       	  }
    	}
   	m_jFEXalgoSearchWindowSeedET[meta][mphi] = seedTotalET;
        ATH_MSG_DEBUG("check seed m_eta, m_phi, seed_ET: "<<meta<<" , "<< mphi<<" , "<<seedTotalET);
      	}
     }
    //ATH_MSG_DEBUG("check ET value of seed: "<<seedTotalET);    
    m_seedSet = true;
}


//check if central TT is a local maxima
bool LVL1::jFEXSmallRJetAlgo::isSeedLocalMaxima()
{
  if(m_seedSet == false){
    ATH_MSG_DEBUG("Local Maxima not checked due to seed not calculated.");
  }
  if(m_seedSet == true){
    ATH_MSG_DEBUG("Local Maxima checking begins.");
    //here put the 24 conditions to determine if the [2][2] TT seed is a local maxima.
    int central_seed = m_jFEXalgoSearchWindowSeedET[2][2];
    for (int ieta = 0; ieta < 5; ieta++){
      for (int iphi = 0; iphi < 5; iphi++){

        //avoid comparing central seed to itself  
 	if ((ieta == 2) && (iphi == 2)){
          continue;
      	}
        //strictly less than central
	if( (iphi >= ieta) && !(ieta == 3 && iphi == 3) && !(ieta == 4 && iphi == 4) ){
	  if(central_seed<m_jFEXalgoSearchWindowSeedET[ieta][iphi]){
	    return false;
	  }  
	}	
	//less than or equal to central 
	if((ieta > iphi) || (ieta == 3 && iphi == 3) || (ieta == 4 && iphi == 4)){
          if(central_seed<= m_jFEXalgoSearchWindowSeedET[ieta][iphi]){
	    return false;
	    }	
	}
       }
     }
  }  
  ATH_MSG_DEBUG("Local Maxima found.");
  return true;
}

void LVL1::jFEXSmallRJetAlgo::setupCluster(int inputTable[4][5]) {

  std::copy(&inputTable[0][0], &inputTable[0][0] + 20, &m_smallRJetClusterIDs[0][0]);

}



//in this clustering func, the central TT in jet is the parameters
unsigned int LVL1::jFEXSmallRJetAlgo::getSmallClusterET(){

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

  //first summing search window (25 TTs)
  unsigned int searchWindowET = 0;
  for(int neta = 0; neta< 5; neta++){ 
    for(int nphi = 0; nphi< 5; nphi++){
      const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[neta][nphi]);
      searchWindowET += tmpTower->getTotalET();
    }
  } 

  //secondly summing over energy ring ET (20 TTs)
  unsigned int clusterBoarderET = 0;
  for(int n = 0; n< 4; n++){
    for(int m = 0; m< 5; m++){

      const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_smallRJetClusterIDs[n][m]);
      clusterBoarderET += tmpTower->getTotalET();
    }
  }

 int smallRclusterET = searchWindowET + clusterBoarderET;
 return smallRclusterET;
}
         
std::unique_ptr<jFEXSmallRJetTOB> LVL1::jFEXSmallRJetAlgo::getSmallRJetTOBs(){

  std::unique_ptr<jFEXSmallRJetTOB> tob = std::make_unique<jFEXSmallRJetTOB>();

  unsigned int et = getSmallClusterET();
 // unsigned int phi = getRealPhi(phi);
 // unsigned int eta = getRealEta(eta);
  
  tob->setET(et);
  tob->setPhi(getRealPhi());
  tob->setEta(getRealEta());
  tob->setRes(0);
  tob->setSat(0);
  return tob;
}


}// end of namespace LVL1
