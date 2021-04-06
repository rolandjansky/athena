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
    ATH_CHECK(m_jFEXtauAlgo_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXtauAlgo::safetyTest() {

    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    if(! jk_jFEXtauAlgo_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve  jk_jFEXtauAlgo_jTowerContainer " << m_jFEXtauAlgo_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void LVL1::jFEXtauAlgo::setup(int TTwindow[5][5], int seed[3][3]) {

    ATH_MSG_DEBUG(m_color.BLUE<<"---------------- jFEXtauAlgo::setup ----------------"<<m_color.END);
    std::copy(&TTwindow[0][0], &TTwindow[0][0] + 25, &m_TTwindow[0][0]);
    std::copy(&seed[0][0], &seed[0][0] + 9 , &m_SeedIDs[0][0]);

}
     
//TOB for Tau Algo
/*LVL1::jFEXtauAlgoTOB * LVL1::jFEXtauAlgo::getTauTOB()
{
  jFEXtauAlgoTOB *tob = new jFEXtauTOB();
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




//this function calculates seed for a given TT
void LVL1::jFEXtauAlgo::buildSeeds()
{
    ATH_MSG_DEBUG("---------------- jFEXtauAlgo::buildsSeeds ----------------");
    m_seedSet = false;
    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    
    for(int mphi = 0; mphi<3; mphi++) {
        for(int meta = 0; meta<3; meta++) {

            int et_tmp = 0;
            int seedTotalET = 0;

            for(int iphi = -1; iphi < 2; iphi++) {
                for(int ieta = -1; ieta < 2; ieta++) {

                    const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(m_TTwindow[(mphi+1) + iphi][(meta+1) + ieta]);
                    et_tmp = tmpTower->getTotalET();
                    seedTotalET += et_tmp;
                }
            }
            m_SeedCluster_ET[mphi][meta] = seedTotalET;

            const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(m_TTwindow[(mphi+1)][(meta+1)]);
            et_tmp = tmpTower->getTotalET();
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


  tob->setET(m_ClusterEt);
  tob->setPhi(mphi-8); // coord within the FPGA core area
  tob->setEta(meta-8); // coord within the FPGA core area
  tob->setIso(m_TauIsolation);
  tob->setSat(0);
  return tob;
}




void LVL1::jFEXtauAlgo::Represent(int var_,int dim_) {
    int rows_ = dim_;
    int colums_ = dim_;
    int row_center= (int) rows_/2;
    int col_center= (int) colums_/2;


    for(int i = 0; i< rows_; i++ ) {
        //if(i == 0) printf("%3s %s%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %s\n -7 ","","",-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,"");
        //else printf("%3d ",i);
        for(int j = 0; j< colums_; j++) {
            std::string col_= "";
            if(sqrt(pow(i-row_center,2)+pow(j-col_center,2))<2) {
                col_= m_color.B_RED; //RED
                if((i-row_center)==0 and (j-col_center)==0) col_= m_color.B_PURPLE;  //PURPLE
            }
            else if(sqrt(pow(i-row_center,2)+pow(j-col_center,2))<4) col_= m_color.B_ORANGE; //ORANGE
            else if(sqrt(pow(i-row_center,2)+pow(j-col_center,2))<8) col_= m_color.B_BLUE; //BLUE
            else col_= m_color.B_GRAY; //GREY

            if(var_==0) printf("%s%4.1f %s",col_.c_str(),sqrt(pow(i,2)+pow(j,2)),"\033[0m");
            if(var_==1) printf("%s%d-%d %6d%s ",col_.c_str(),i,j,m_SeedIDs[i][j],"\033[0m");  //Trigger Tower ID
            if(var_==2) printf("%s%6d%s ",col_.c_str(),getTTowerET(i,j),"\033[0m");  //Trigger Tower ID
            if(var_==3) printf("%s%6d%s ",col_.c_str(),getRealEta(i,j),"\033[0m");  //Trigger Tower ID
            if(var_==4) printf("%s%6d%s ",col_.c_str(),getRealPhi(i,j),"\033[0m");  //Trigger Tower ID
            if(var_==5) printf("%s%2d-%2d%s ",col_.c_str(),i,j,"\033[0m");
            if(var_==6){
              if(m_SeedConditions_ET[i][j]<m_SeedConditions_ET[1][1]) col_= m_color.B_GREEN;
              printf("%s%6d<%6d%6d%s ",col_.c_str(),m_SeedConditions_ET[i][j],m_SeedConditions_ET[1][1],(int)(m_SeedConditions_ET[i][j]<m_SeedConditions_ET[1][1]),"\033[0m");  //Trigger Tower ID
            } 
            if(var_==7) printf("%s%6d%s ",col_.c_str(),m_TTwindow[i][j],"\033[0m");
            if(var_==8) printf("%s%6d%s ",col_.c_str(),getTTowerET(i,j),"\033[0m");
        }
        printf("\n");
    }
    printf("\n");

}





//Gets the ET for the TT. This ET is EM + HAD
int LVL1::jFEXtauAlgo::getTTowerET(unsigned int row_,unsigned int col_) {
    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(m_SeedIDs[row_][col_]);
    return tmpTower->getTotalET();
}
//Gets Phi of the TT
int LVL1::jFEXtauAlgo::getRealPhi(unsigned int row_,unsigned int col_) {

    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(m_SeedIDs[row_][col_]);
    
    return tmpTower->phi();
}
//Gets Eta of the TT
int LVL1::jFEXtauAlgo::getRealEta(unsigned int row_,unsigned int col_) {

    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(m_SeedIDs[row_][col_]);
    return realValue(m_SeedIDs[row_][col_],tmpTower->eta()); //return positive ETA for even TTs ID 2XX.XXX.. etc and negative ETA for odd TTs ID 1XX.XXX.. and so on
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
    SG::ReadHandle<jTowerContainer> jk_jFEXtauAlgo_jTowerContainer(m_jFEXtauAlgo_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXtauAlgo_jTowerContainer->findTower(First_ETring[i]);
    m_TauIsolation += tmpTower->getTotalET();
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


}// end of namespace LVL1
