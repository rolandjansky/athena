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

void LVL1::jFEXSmallRJetAlgo::setup(int inputTable[7][7]) {

  std::copy(&inputTable[0][0], &inputTable[0][0] + 49, &m_jFEXalgoTowerID[0][0]);
}


int LVL1::jFEXSmallRJetAlgo::realValue(int ID, int eta){

  return ((int)(ID/pow(10,5)) % 10) % 2==0 ?  eta : -eta ;
  
}


//Gets the ET for the TT. This ET is EM + HAD
unsigned int LVL1::jFEXSmallRJetAlgo::getTTowerET(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    } 
    
    if(m_map_Etvalues.find(TTID) != m_map_Etvalues.end()) {
        return m_map_Etvalues[TTID][0];
    }
    
    //we shouldn't arrive here
    return 0;
    
}

//Gets Phi of the TT
unsigned int LVL1::jFEXSmallRJetAlgo::getRealPhi(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }
    SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

    unsigned int phi = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(TTID)->phi();
    return phi;
}
//Gets Eta of the TT
int LVL1::jFEXSmallRJetAlgo::getRealEta(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }
    SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);
    const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(TTID);
    return realValue(TTID,tmpTower->eta());
}



//this function calculates seed for a given TT
void LVL1::jFEXSmallRJetAlgo::buildSeeds()
{

    m_seedSet = false;
    m_LMDisplaced = false;

    for(int mphi = 1; mphi < 6; mphi++) {
        for(int meta = 1; meta< 6; meta++) {
            int et_tmp = 0;
            int seedTotalET = 0;
            for(int ieta = -1; ieta < 2; ieta++) {
                for(int iphi = -1; iphi < 2; iphi++) {
                    //for that TT, build the seed
                    //here we sum TT ET to calculate seed
                    et_tmp = getTTowerET(m_jFEXalgoTowerID[meta + ieta][mphi + iphi]);
                    seedTotalET += et_tmp;
                }
            }
            m_jFEXalgoSearchWindowSeedET[meta -1][mphi -1] = seedTotalET;
        }

    }

    int centralTT_ET = getTTowerET(m_jFEXalgoTowerID[3][3]);
    if(centralTT_ET==m_jFEXalgoSearchWindowSeedET[3][3]) {
        m_LMDisplaced = true;
    }

    m_seedSet = true;
}


//check if central TT is a local maxima
bool LVL1::jFEXSmallRJetAlgo::isSeedLocalMaxima()
{
    if(m_seedSet == false) {
        ATH_MSG_DEBUG("Local Maxima not checked due to seed not calculated.");
    }
    if(m_seedSet == true) {

        //here put the 24 conditions to determine if the [2][2] TT seed is a local maxima.

        int central_seed = m_jFEXalgoSearchWindowSeedET[2][2];
        for (int ieta = 0; ieta < 5; ieta++) {
            for (int iphi = 0; iphi < 5; iphi++) {
                //avoid comparing central seed to itself
                if ((ieta == 2) && (iphi == 2)) { 
                    continue;
                }
                //strictly less than central
                if( (iphi >= ieta) && !(ieta == 3 && iphi == 3) && !(ieta == 4 && iphi == 4) ) {
                    if(central_seed<m_jFEXalgoSearchWindowSeedET[ieta][iphi]) {
                        return false;
                    }
                }
                //less than or equal to central
                if((ieta > iphi) || (ieta == 3 && iphi == 3) || (ieta == 4 && iphi == 4)) {
                    if(central_seed<= m_jFEXalgoSearchWindowSeedET[ieta][iphi]) {
                        return false;
                     }
              }
            }
        } 
    }
    
    return true;

}
bool LVL1::jFEXSmallRJetAlgo::checkDisplacedLM()
{
   return m_LMDisplaced;
}


//in this clustering func, the central TT in jet is the parameters
unsigned int LVL1::jFEXSmallRJetAlgo::getSmallClusterET(){


  //first summing search window (25 TTs)
  unsigned int searchWindowET = 0;
  for(int neta = 0; neta< 7; neta++){ 
    for(int nphi = 0; nphi< 7; nphi++){
      searchWindowET += getTTowerET(m_jFEXalgoTowerID[neta][nphi]);
    }
  } 

  //corners removed in 7x7 window to obtain SmallRJetCluser ET;
  int cornersET = 0;  

  cornersET += getTTowerET(m_jFEXalgoTowerID[0][0]);
  cornersET += getTTowerET(m_jFEXalgoTowerID[6][6]);
  cornersET += getTTowerET(m_jFEXalgoTowerID[0][6]);
  cornersET += getTTowerET(m_jFEXalgoTowerID[6][0]);

  //SR Jet Energy cluster
  int SRJetClusterET = searchWindowET + cornersET;

  return SRJetClusterET;
}

unsigned int LVL1::jFEXSmallRJetAlgo::getSmallETRing(){
  int SmallETRing = getSmallClusterET() - m_jFEXalgoSearchWindowSeedET[3][3];   
  return SmallETRing;
}
         
std::unique_ptr<jFEXSmallRJetTOB> LVL1::jFEXSmallRJetAlgo::getSmallRJetTOBs(){

  std::unique_ptr<jFEXSmallRJetTOB> tob = std::make_unique<jFEXSmallRJetTOB>();

  unsigned int et = getSmallClusterET();

  tob->setET(et); 
  tob->setPhi(getRealPhi(m_jFEXalgoTowerID[3][3]));
  tob->setEta(getRealEta(m_jFEXalgoTowerID[3][3]));
  tob->setRes(0);
  tob->setSat(0);
  return tob;
}


unsigned int LVL1::jFEXSmallRJetAlgo::getTTIDcentre(){
  return m_jFEXalgoTowerID[3][3];
}


void LVL1::jFEXSmallRJetAlgo::setFPGAEnergy(std::map<int,std::vector<int> > et_map){
    m_map_Etvalues=et_map;
}

}// end of namespace LVL1

