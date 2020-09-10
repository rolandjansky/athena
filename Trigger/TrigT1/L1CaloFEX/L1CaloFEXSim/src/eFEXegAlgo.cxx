/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXegAlgo  -  description
//                              -------------------
//     begin                : 24 02 2020
//     email                : antonio.jacques.costa@cern.ch ulla.blumenschein@cern.ch tong.qiu@cern.ch
//  ***************************************************************************/
#include <iostream>
#include <vector>

#include "L1CaloFEXSim/eFEXegAlgo.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eTower.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {

  // default constructor for persistency
eFEXegAlgo::eFEXegAlgo(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent) 
  {
    declareInterface<IeFEXegAlgo>(this);
  }

  /** Destructor */
eFEXegAlgo::~eFEXegAlgo()
{
}

StatusCode eFEXegAlgo::initialize(){

  ATH_CHECK(m_eFEXegAlgo_eTowerContainerKey.initialize());

  return StatusCode::SUCCESS;

}


StatusCode eFEXegAlgo::safetyTest(){

  // This is to test that it will also work in the other functions, as we call this object from SG in every function but don't want them all to have to return StatusCodes, and i'm not sure how to make this a private member instead
  //ATH_CHECK(m_eFEXegAlgo_eTowerContainerKey.initialize());
  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);
  if(!jk_eFEXegAlgo_eTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_eFEXegAlgo_eTowerContainer " << m_eFEXegAlgo_eTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}

void eFEXegAlgo::setup(int inputTable[3][3]) {
  
  std::copy(&inputTable[0][0], &inputTable[0][0] + 9, &m_eFEXegAlgoTowerID[0][0]);
  
  setSeed();

}

void LVL1::eFEXegAlgo::getCoreEMTowerET(unsigned int & et) { 

  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);
  
  const LVL1::eTower * tmpTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[1][1]);
  et = tmpTower->getLayerTotalET(0) + tmpTower->getLayerTotalET(1) + tmpTower->getLayerTotalET(2) + tmpTower->getLayerTotalET(3);

}

void LVL1::eFEXegAlgo::getCoreHADTowerET(unsigned int & et) { 

  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);

  const LVL1::eTower * tmpTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[1][1]);
  et = tmpTower->getLayerTotalET(4);

}

void LVL1::eFEXegAlgo::getRealPhi(float & phi) {

  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);
  phi = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[1][1])->phi();
  
}

void LVL1::eFEXegAlgo::getRealEta(float & eta) {
  
  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);

  eta = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[1][1])->eta() * jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[1][1])->getPosNeg();

}

std::vector<unsigned int> eFEXegAlgo::getReta() {

  std::vector<unsigned int> retavec;
  unsigned int coresum  = 0;   // 3x2 L2 sum : core
  unsigned int totalsum = 0;   // 7x3 L2 sum : total
  unsigned int envsum   = 0;   // total - core : env

  // window limits
  int iTotalStart = m_seedID-3;
  int iTotalEnd   = m_seedID+3;
  int iCoreStart  = m_seedID-1;
  int iCoreEnd    = m_seedID+1;
  int phiStart    = -999; 
  int phiEnd      = -99;
  if (m_seed_UnD) {
    phiStart = 0;
    phiEnd = 1;
  } else {
    phiStart = 1;
    phiEnd = 2;
  }

  // 3x2 and 7x3 L2 sum
  for (int i=iTotalStart; i<=iTotalEnd; ++i) { // eta
    for(int j=0; j<=2; ++j) { // phi
      if (i>=iCoreStart && i <= iCoreEnd && j>=phiStart && j<=phiEnd) {
	unsigned int tmp_et; getWindowET(2,j,i,tmp_et);
	coresum += tmp_et;
      }

      unsigned int tmptot_et; getWindowET(2,j,i,tmptot_et);
      totalsum += tmptot_et;
    }
  }

  // get environment
  envsum = totalsum - coresum;

  retavec.push_back(coresum);
  retavec.push_back(envsum);

  return retavec;
}

void eFEXegAlgo::getRhad(std::vector<unsigned int> & rhadvec) {

  //std::vector<unsigned int> rhadvec;
  unsigned int hadsum = 0; // 3x3 Towers Had 
  unsigned int emsum = 0;  // (1x3 + 3x3 + 3x3 + 1x3) SCs EM
  
  int iCoreStart  = m_seedID-1;
  int iCoreEnd    = m_seedID+1;

  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);
  
  // 3x3 Towers Had ; 1x3 L0 + 1x3 L3 EM
  for (int i=0; i<3; ++i) { // eta
    for (int j=0; j<=2; ++j) { // phi
      const eTower * tTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[i][j]);
      hadsum += tTower->getLayerTotalET(4);
      if (i==1) {
	emsum += ( tTower->getLayerTotalET(0) + tTower->getLayerTotalET(3) );
      }
    }
  }
  
  // 3x3 SCs L1 and L2 sum
  for (int i=iCoreStart; i<=iCoreEnd; ++i) { // eta
    for(int j=0; j<=2; ++j) { // phi
      unsigned int tmp_et_a, tmp_et_b;
      getWindowET(1,j,i,tmp_et_a);
      getWindowET(2,j,i,tmp_et_b);
      emsum += ( tmp_et_a + tmp_et_b );
    }
  }   
  
  rhadvec.push_back(emsum);
  rhadvec.push_back(hadsum);

}

void LVL1::eFEXegAlgo::getWstot(std::vector<unsigned int> & output){
  unsigned int numer = 0;
  unsigned int den = 0;
  int iStart = m_seedID - 2;
  int iEnd = m_seedID + 2;

  for (int i = iStart; i <= iEnd; ++i) { // eta
    int diff = i - m_seedID;
    unsigned int weight = diff*diff;
    for (int j = 0; j <= 2; ++j) { // phi
      unsigned int eT; getWindowET(1, j, i, eT);
      numer += eT*weight;
      den += eT;
    }
  }
  output.push_back(numer);
  output.push_back(den);

}

unsigned int LVL1::eFEXegAlgo::getET() {
  int phiUpDownID = 1 + m_seed_UnD - !m_seed_UnD;
  
  unsigned int PS_ET_1, PS_ET_2;
  getWindowET(0, 1, 0, PS_ET_1);
  getWindowET(0, phiUpDownID, 0, PS_ET_2);
  unsigned int L1_ET_1, L1_ET_2, L1_ET_3, L1_ET_4, L1_ET_5, L1_ET_6;
  getWindowET(1, 1, m_seedID, L1_ET_1); getWindowET(1, 1, m_seedID - 1, L1_ET_2); getWindowET(1, 1, m_seedID + 1, L1_ET_3);
  getWindowET(1, phiUpDownID, m_seedID, L1_ET_4); getWindowET(1, phiUpDownID, m_seedID - 1, L1_ET_5); getWindowET(1, phiUpDownID, m_seedID + 1, L1_ET_6);
  unsigned int L2_ET_1, L2_ET_2, L2_ET_3, L2_ET_4, L2_ET_5, L2_ET_6;
  getWindowET(2, 1, m_seedID, L2_ET_1); getWindowET(2, 1, m_seedID - 1, L2_ET_2); getWindowET(2, 1, m_seedID + 1, L2_ET_3);
  getWindowET(2, phiUpDownID, m_seedID, L2_ET_4); getWindowET(2, phiUpDownID, m_seedID - 1, L2_ET_5); getWindowET(2, phiUpDownID, m_seedID + 1, L2_ET_6);
  unsigned int L3_ET_1, L3_ET_2;
  getWindowET(3, 1, 0, L3_ET_1); getWindowET(3, phiUpDownID, 0, L3_ET_2);

  unsigned int totET = PS_ET_1 + PS_ET_2;
  totET += L1_ET_1 + L1_ET_2 + L1_ET_3 + L1_ET_4 + L1_ET_5 + L1_ET_6;
  totET += L2_ET_1 + L2_ET_2 + L2_ET_3 + L2_ET_4 + L2_ET_5 + L2_ET_6;
  totET += L3_ET_1 + L3_ET_2;

  return totET;

}

std::unique_ptr<eFEXegTOB> LVL1::eFEXegAlgo::geteFEXegTOB() {
  std::unique_ptr<eFEXegTOB> out = std::make_unique<eFEXegTOB>();
;// = new eFEXegTOB();
  out->setET(getET());

  std::vector<unsigned int> temvector;
  getWstot(temvector);

  out->setWstotNum(temvector[0]);
  out->setWstotDen(temvector[1]);
  getRhad(temvector);
  out->setRhadNum(temvector[1]);
  out->setRhadDen(temvector[0] + temvector[1]);
  temvector = getReta();
  out->setRetaNum(temvector[0]);
  out->setRetaDen(temvector[0] + temvector[1]);
  out->setSeedUnD(m_seed_UnD);
  out->setSeed(m_seedID);
  return out;
}

void LVL1::eFEXegAlgo::getWindowET(int layer, int jPhi, int SCID, unsigned int & outET) {

  SG::ReadHandle<eTowerContainer> jk_eFEXegAlgo_eTowerContainer(m_eFEXegAlgo_eTowerContainerKey/*,ctx*/);

  if (SCID<0) { // left towers in eta
    int etaID = 4+SCID;
    const eTower * tmpTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[jPhi][0]);
    if (layer==1 || layer==2) {
      outET = tmpTower->getET(layer,etaID);
    } else if (layer==0 || layer==3 || layer==4) {
      outET = tmpTower->getLayerTotalET(layer);
    }
  } else if (SCID>=0 && SCID<4) { // central towers in eta
    const eTower * tmpTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[jPhi][1]);
    if (layer==1 || layer==2) { 
      outET = tmpTower->getET(layer,SCID);
    } else if (layer==0 || layer==3 || layer==4) {
      outET = tmpTower->getLayerTotalET(layer);
    }
  } else if (SCID>=4){ // right towers in eta
    int etaID = SCID-4;
    const eTower * tmpTower = jk_eFEXegAlgo_eTowerContainer->findTower(m_eFEXegAlgoTowerID[jPhi][2]);
    if (layer==1 || layer==2) {  
      outET = tmpTower->getET(layer,etaID);
    } else if (layer==0 || layer==3 || layer==4) {
      outET = tmpTower->getLayerTotalET(layer);
    }
  }

}
  
  
void eFEXegAlgo::setSeed() {

  m_haveseed = false;
  unsigned int tmpID = 999;
  unsigned int maxET = 0;
  
  for (int i=0; i<4 ; ++i) {
    int iSeedL = i-1;
    int iSeedR = i+1;

    // eta ID of candidate seed
    unsigned int cETUp;
    getWindowET(2,0,i,cETUp);
    unsigned int iSeedET;
    getWindowET(2,1,i, iSeedET);
    unsigned int cETDown;
    getWindowET(2,2,i, cETDown);
    
    // left of candidate seed
    unsigned int lETUp;
    getWindowET(2,0,iSeedL,lETUp);
    unsigned int lET;
    getWindowET(2,1,iSeedL,lET);
    unsigned int lETDown;
    getWindowET(2,2,iSeedL,lETDown);
    
    // right of candidate seed
    unsigned int rETUp;
    getWindowET(2,0,iSeedR,rETUp);
    unsigned int rET;
    getWindowET(2,1,iSeedR,rET);
    unsigned int rETDown;
    getWindowET(2,2,iSeedR,rETDown);
    
    // greater for left and up cells, greater or equal for right and down ones
    if (iSeedET>lET && iSeedET>=rET 
        && iSeedET>lETUp    && iSeedET>cETUp    && iSeedET>rETUp 
        && iSeedET>=lETDown && iSeedET>=cETDown && iSeedET>=rETDown) {
      if (iSeedET>maxET) { // if two maxima exist, keep the one to the left
        maxET = iSeedET;
        tmpID = i;
      }
    }
  }
  
  if(tmpID!=999) {
    m_seedID = tmpID;
    m_haveseed = true;
    unsigned int tmp_et_1, tmp_et_2;
    getWindowET(2,0,m_seedID,tmp_et_1);
    getWindowET(2,2,m_seedID,tmp_et_2);
    if (tmp_et_1 >= tmp_et_2) {
      m_seed_UnD = true; // go up if energy greater or equal to bottom
    } 
  }
}

} // namespace LVL1
