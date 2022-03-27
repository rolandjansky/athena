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
   ATH_CHECK(m_jTowerContainerKey.initialize());

   return StatusCode::SUCCESS;

}
StatusCode LVL1::jFEXLargeRJetAlgo::safetyTest(){
  SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey);
  if(! jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve container " << m_jTowerContainerKey.key());
    return StatusCode::FAILURE;
  }  
  return StatusCode::SUCCESS;

}

void LVL1::jFEXLargeRJetAlgo::setupCluster(int inputTable[15][15]){

  std::copy(&inputTable[0][0], &inputTable[0][0] + 225 , &m_largeRJetEtRing_IDs[0][0]);

}

unsigned int LVL1::jFEXLargeRJetAlgo::getRingET(){
  int RingET =0;
  for(int n =0; n <15; n++){
    for(int m =0; m <15; m++){
      if((m_largeRJetEtRing_IDs[n][m]) != 0){
        int et = getTTowerET(m_largeRJetEtRing_IDs[n][m]);
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

std::unique_ptr<jFEXLargeRJetTOB> LVL1::jFEXLargeRJetAlgo::getLargeRJetTOBs(int smallClusterET,int TTID){
                                                           
  std::unique_ptr<jFEXLargeRJetTOB> tob = std::make_unique<jFEXLargeRJetTOB>(); 
  unsigned int et = getLargeClusterET(smallClusterET,getRingET());
  tob->setET(et);
  tob->setPhi(m_jFEXSmallRJetAlgoTool->getRealPhi(TTID)/10.); 
  tob->setEta(m_jFEXSmallRJetAlgoTool->getRealEta(TTID)/10.); 
  tob->setRes(0); 
  tob->setSat(0); 

  return tob;
}

//Gets the ET for the TT. This ET is EM + HAD
int LVL1::jFEXLargeRJetAlgo::getTTowerET(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    } 
    
    if(m_map_Etvalues.find(TTID) != m_map_Etvalues.end()) {
        return m_map_Etvalues[TTID][0];
    }
    
    //we shouldn't arrive here
    return 0;
    
}


void LVL1::jFEXLargeRJetAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map){
    m_map_Etvalues=et_map;
}

}// end of namespace LVL1



