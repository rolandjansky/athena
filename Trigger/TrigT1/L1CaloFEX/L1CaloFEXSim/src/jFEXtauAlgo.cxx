/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXtauAlgo - Algorithm for Tau Algorithm in jFEX
//                              -------------------
//     begin                : 18 02 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "L1CaloFEXSim/jFEXtauAlgo.h"
#include "L1CaloFEXSim/jFEXtauTOB.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXtauAlgo::jFEXtauAlgo(const std::string& type, const std::string& name, const IInterface* parent): AthAlgTool(type, name, parent) {
    declareInterface<IjFEXtauAlgo>(this);
}

/** Destructor */
LVL1::jFEXtauAlgo::~jFEXtauAlgo() {
}

StatusCode LVL1::jFEXtauAlgo::initialize() {
    ATH_CHECK(m_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXtauAlgo::safetyTest() {

    SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey);
    if(! jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve  jTowerContainer " << m_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void LVL1::jFEXtauAlgo::setup(int TTwindow[5][5], int seed[3][3]) {

    ATH_MSG_DEBUG(m_color.BLUE<<"---------------- jFEXtauAlgo::setup ----------------"<<m_color.END);
    std::copy(&TTwindow[0][0], &TTwindow[0][0] + 25, &m_TTwindow[0][0]);
    std::copy(&seed[0][0], &seed[0][0] + 9 , &m_SeedIDs[0][0]);

}
     

//this function calculates seed for a given TT
void LVL1::jFEXtauAlgo::buildSeeds()
{
    ATH_MSG_DEBUG("---------------- jFEXtauAlgo::buildsSeeds ----------------");
    m_seedSet = false;
    
    for(int mphi = 0; mphi<3; mphi++) {
        for(int meta = 0; meta<3; meta++) {

            int et_tmp = 0;
            int seedTotalET = 0;

            for(int iphi = -1; iphi < 2; iphi++) {
                for(int ieta = -1; ieta < 2; ieta++) {
                    et_tmp =  getTTowerET(m_TTwindow[(mphi+1) + iphi][(meta+1) + ieta]);
                    seedTotalET += et_tmp;
                }
            }
            m_SeedCluster_ET[mphi][meta] = seedTotalET;
            
            et_tmp = getTTowerET(m_TTwindow[(mphi+1)][(meta+1)]);
            m_SeedConditions_ET[mphi][meta] = et_tmp;
        }
    }

    m_seedSet = true;
    ATH_MSG_DEBUG("---------------- jFEXtauAlgo::buildsSeeds finished ----------------");


}


//check if central TT is a local maxima
bool LVL1::jFEXtauAlgo::isSeedLocalMaxima(){
    m_isLocalMaxima=false;
    m_ClusterEt = m_SeedCluster_ET[1][1];
    
    if(m_seedSet == false) {
        ATH_MSG_DEBUG("Local Maxima not checked due to seed not calculated.");
    }
    if(m_seedSet == true) {
        ATH_MSG_DEBUG("Local Maxima checking begins.");
        //here put the 8 conditions to determine if the [1][1] TT seed is a local maxima.
        int central_seed = m_SeedConditions_ET[1][1];
        for (int iphi = 0; iphi < 3; iphi++) {
            for (int ieta = 0; ieta < 3; ieta++) {

                //avoid comparing central seed to itself
                if ((ieta == 1) && (iphi == 1)) {
                    continue;
                }
                else if( (iphi > ieta) || (ieta==2 && iphi==2) ) { //strictly less than central
                    if(central_seed<=m_SeedConditions_ET[iphi][ieta]) {
                        return false;
                    }
                }
                else if((ieta > iphi) || (ieta == 0 && iphi == 0)) { //less than or equal to central
                    if(central_seed< m_SeedConditions_ET[iphi][ieta]) {
                        return false;
                    }
                }

            }
        }
    }
    m_isLocalMaxima=true;
    

    
    ATH_MSG_DEBUG("Local Maxima found. with ClusterET = "<<m_ClusterEt);
    return true;
}


         
std::unique_ptr<jFEXtauTOB> LVL1::jFEXtauAlgo::getTauTOBs(int mphi, int meta){

  std::unique_ptr<jFEXtauTOB> tob = std::make_unique<jFEXtauTOB>();
  
  int sat = 0; 
  unsigned int et = m_ClusterEt/200.;
  if (et > 0x7ff) { //0x7ff is 11 bits
    et = 0x7ff;
    sat=1;
  }
  unsigned int iso = m_TauIsolation/200.;
  if (iso > 0x7ff) { //0x7ff is 11 bits
    iso = 0x7ff;
  }

  tob->setET(et);
  tob->setPhi(mphi-8); // coord within the FPGA core area
  tob->setEta(meta-8); // coord within the FPGA core area
  tob->setIso(iso);
  tob->setSat(sat);
  return tob;
}


//Gets the ET for the TT. This ET is EM + HAD
int LVL1::jFEXtauAlgo::getTTowerET(unsigned int TTID ) {
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
int LVL1::jFEXtauAlgo::getRealPhi(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }
    
    SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jTowerContainer->findTower(TTID);
    return tmpTower->phi();
}
//Gets Eta of the TT
int LVL1::jFEXtauAlgo::getRealEta(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }
    
    SG::ReadHandle<jTowerContainer> jTowerContainer(m_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jTowerContainer->findTower(TTID);
    return realValue(TTID,tmpTower->eta()); //return positive ETA for even TTs ID 2XX.XXX.. etc and negative ETA for odd TTs ID 1XX.XXX.. and so on
    //return tmpTower->eta();
}

//Gets the seed total ET
int LVL1::jFEXtauAlgo::getClusterEt() {
    return m_ClusterEt;
}


//Gets the Isolation/FirstEtRing of jFEX Tau

void LVL1::jFEXtauAlgo::setFirstEtRing(int First_ETring[36]) {

  ATH_MSG_DEBUG("Calculating the jFEXTau ISO");
  m_TauIsolation=0;

  for(int i=0;i<36;i++){
    m_TauIsolation += getTTowerET(First_ETring[i]);
  }
  
}

int LVL1::jFEXtauAlgo::getFirstEtRing() {
  
  return m_TauIsolation;
}

//Gets the seed total ET
int LVL1::jFEXtauAlgo::getIsLocalMaxima() {

    return m_isLocalMaxima;
}

int LVL1::jFEXtauAlgo::realValue(int ID, int eta){

  return ((int)(ID/pow(10,5)) % 10) % 2==0 ?  eta : -eta ;
  
}

void LVL1::jFEXtauAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map){
    m_map_Etvalues=et_map;
}


}// end of namespace LVL1
