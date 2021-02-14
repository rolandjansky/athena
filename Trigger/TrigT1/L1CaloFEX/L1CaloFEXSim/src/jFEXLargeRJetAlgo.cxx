/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXSmallRJetAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#include <iostream>
#include <vector>
#include "L1CaloFEXSim/jFEXLargeRJetAlgo.h"
#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

LVL1::jFEXLargeRJetAlgo::jFEXLargeRJetAlgo(const std::string& type, const std::string& name, const IInterface* parent):
   AthAlgTool(type, name, parent)   
  {
  declareInterface<IjFEXLargeRJetAlgo>(this);
  }

/** Destructor */
LVL1::jFEXLargeRJetAlgo::~jFEXLargeRJetAlgo()
{
}
StatusCode LVL1::jFEXLargeRJetAlgo::initialize()
{
   ATH_CHECK(m_jFEXLargeRJetAlgo_jTowerContainerKey.initialize());

   return StatusCode::SUCCESS;

}
StatusCode LVL1::jFEXLargeRJetAlgo::safetyTest(){
  SG::ReadHandle<jTowerContainer> jk_jFEXLargeRJetAlgo_jTowerContainer(m_jFEXLargeRJetAlgo_jTowerContainerKey);
  if(! jk_jFEXLargeRJetAlgo_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve  jk_jFEXLargeRJetAlgo_jTowerContainer " << m_jFEXLargeRJetAlgo_jTowerContainerKey.key());
    return StatusCode::FAILURE;
  }  
  return StatusCode::SUCCESS;

}

void LVL1::jFEXLargeRJetAlgo::setupCluster(int inputTable[15][15]){

  std::copy(&inputTable[0][0], &inputTable[0][0] + 225 , &m_largeRJetEtRing_IDs[0][0]);

}

unsigned int LVL1::jFEXLargeRJetAlgo::getRingET(){
  SG::ReadHandle<jTowerContainer> jk_jFEXLargeRJetAlgo_jTowerContainer(m_jFEXLargeRJetAlgo_jTowerContainerKey/*,ctx*/);
  int RingET =0;
  for(int n =0; n <15; n++){
    for(int m =0; m <15; m++){
      if((m_largeRJetEtRing_IDs[n][m]) != 0){
        const LVL1::jTower * tmpTower = jk_jFEXLargeRJetAlgo_jTowerContainer->findTower(m_largeRJetEtRing_IDs[n][m]);
        int et = tmpTower->getTotalET();
        RingET +=et;
      }
    }
  }
  return RingET;
}

unsigned int LVL1::jFEXLargeRJetAlgo::getLargeClusterET(unsigned int smallClusterET, unsigned int largeRingET){
  int largeClusterET = smallClusterET + largeRingET;
  return largeClusterET; 
}

std::unique_ptr<jFEXLargeRJetTOB> LVL1::jFEXLargeRJetAlgo::getLargeRJetTOBs(){
                                                           
  std::unique_ptr<jFEXLargeRJetTOB> tob = std::make_unique<jFEXLargeRJetTOB>(); 
  unsigned int et = getLargeClusterET(m_jFEXSmallRJetAlgoTool->getSmallClusterET(),getRingET());
  tob->setET(et);
  tob->setPhi(m_jFEXSmallRJetAlgoTool->getRealPhi()/10.); 
  tob->setEta(m_jFEXSmallRJetAlgoTool->getRealEta()/10.); 
  tob->setSub(0); 
  tob->setSat(0); 

  return tob;
}



}// end of namespace LVL1



