/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXForwardJetsAlgo - Algorithm for forward Jets in jFEX
//                              -------------------
//     begin                : 07 06 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************  
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXForwardJetsAlgo::jFEXForwardJetsAlgo(const std::string& type, const std::string& name, const IInterface* parent):
   AthAlgTool(type, name, parent)   
  {
  declareInterface<IjFEXForwardJetsAlgo>(this);
  }

/** Destructor */
LVL1::jFEXForwardJetsAlgo::~jFEXForwardJetsAlgo()
{
}
StatusCode LVL1::jFEXForwardJetsAlgo::initialize()
{
   ATH_CHECK(m_jFEXForwardJetsAlgo_jTowerContainerKey.initialize());
   return StatusCode::SUCCESS;

}

//calls container for TT
StatusCode LVL1::jFEXForwardJetsAlgo::safetyTest(){

  SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey);

  if(! jk_jFEXForwardJetsAlgo_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve  jk_jFEXForwardJetsAlgo_jTowerContainer " << m_jFEXForwardJetsAlgo_jTowerContainerKey.key());

    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

void LVL1::jFEXForwardJetsAlgo::setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) {
  std::copy(&inputTable[0][0], &inputTable[0][0] + (FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width), &m_jFEXalgoTowerID[0][0]);
}

//Gets geometric global centre Phi coord of the TT
float LVL1::jFEXForwardJetsAlgo::globalPhi(int nphi, int neta) {

  SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
  float phi = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[nphi][neta])->centrePhi();
  return phi;
}

//Gets geometric global centre Eta coord of the TT
float LVL1::jFEXForwardJetsAlgo::globalEta(int nphi, int neta) {

  SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
  float eta = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[nphi][neta])->centreEta();
  return eta;

}

//Gets Phi of the TT
unsigned int LVL1::jFEXForwardJetsAlgo::localPhi(int nphi, int neta) {

 SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
 unsigned int phi = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[nphi][neta])->phi();
 return phi;
}
//Gets Eta of the TT
unsigned int LVL1::jFEXForwardJetsAlgo::localEta(int nphi, int neta) {

 SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
 unsigned int eta  = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[nphi][neta])->eta();
 return eta;
}

//Return ET of TT. Should be FCAL 0 + 1 + 2 //maybe check this 
unsigned int LVL1::jFEXForwardJetsAlgo::getTTowerET(int nphi, int neta){

 SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
 const LVL1::jTower * tmpTower = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[nphi][neta]);
 unsigned int et = tmpTower->getTotalET();
 return et;
}

std::map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::FcalJetsTowerIDLists(){
 
  std::map<int, jFEXForwardJetsInfo> FCALJetTowerIDLists; 
  static constexpr float TT_Size = M_PI/32;

  //This flag determines if the TT ID which are in the first and second energy rings are stored 
  //in the jFEXForwardJetsInfo class. It is set to false to reduce quantity data stored in class
  m_storeEnergyRingTTIDs = false;

  std::vector<int> lower_centre_neta;  
  std::vector<int> upper_centre_neta;  
  int lowerEM_eta = 0;
  int upperEM_eta = 0;

  //STEP 1: check if we are in module 0 or 5 and assign corrrect eta FEXAlgoSpace parameters
  if(m_jFEXalgoTowerID[31][0] == 0){
    //Module 0
    lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_start_eta});
    upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_end_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta,FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta });

    lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerEM_eta;
    upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperEM_eta;
  }

  else{
    //Module 5 
    lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMB_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta});
    upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_end_eta});

    lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerEM_eta;
    upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperEM_eta;
  } 
  
  //STEP 2: define phi FEXAlgoSpace parameters
  std::vector<int> lower_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_start_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_start_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_start_phi}; 
  std::vector<int> upper_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_end_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_end_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_end_phi};


  //STEP 3: loop over different EM/FCAL0 eta phi core fpga regions. These are potential central trigger towers for jets
  for(int myCounter = 0; myCounter<3; myCounter++){
    for(int centre_nphi = lower_centre_nphi[myCounter]; centre_nphi < upper_centre_nphi[myCounter]; centre_nphi++) {
      for(int centre_neta = lower_centre_neta[myCounter]; centre_neta < upper_centre_neta[myCounter]; centre_neta++) {

        //STEP 4: define TTID which will be the key for class in map
        int myTTIDKey = m_jFEXalgoTowerID[centre_nphi][centre_neta];

        //STEP 5: ignore when tower ID is zero. 
        if(myTTIDKey == 0){continue;}

        //STEP 6: define class
        jFEXForwardJetsInfo TriggerTowerInformation;

        //STEP 7: fill local eta phi coords
        TriggerTowerInformation.setCentreLocalTTPhi(centre_nphi);
        TriggerTowerInformation.setCentreLocalTTEta(centre_neta);
        
        float centreTT_phi = globalPhi(centre_nphi, centre_neta);
        float centreTT_eta = globalEta(centre_nphi, centre_neta);
        TriggerTowerInformation.setCentreTTPhi(centreTT_phi);
        TriggerTowerInformation.setCentreTTEta(centreTT_eta);

        //STEP 8: loop over all FCAL0 TTIDs and fill TT IDs for seed and calculate seed energy     
        for(int nphi = 0; nphi < 32; nphi++) {   
          for(int neta = lowerEM_eta; neta < upperEM_eta; neta++) {
            if(((nphi == centre_nphi) && (neta == centre_neta)) ||(m_jFEXalgoTowerID[nphi][neta])==0 ){continue;}
            float TT_phi = globalPhi(nphi, neta);
            float TT_eta = globalEta(nphi, neta);
            float R = sqrt(pow((centreTT_eta - TT_eta),2) + pow((centreTT_phi - TT_phi),2));
            if((R > 0) && (R <( 2 * TT_Size))){
	      //STEP 9: fill TTID in seed
              TriggerTowerInformation.includeTTinSeed(m_jFEXalgoTowerID[nphi][neta]);           
              //STEP 10: add ET value to seed 
	      TriggerTowerInformation.addToSeedET(getTTowerET(nphi, neta));   
            }                        
          }
        }
        //STEP 11: Store filled class into map, with central TT ID as Key     
        FCALJetTowerIDLists[myTTIDKey] = TriggerTowerInformation;     
     }//end of centre_nphi loop 
   }//end of centre_mphi loop
 }//end of myCounter loop
 return FCALJetTowerIDLists;      
}



std::map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::isSeedLocalMaxima(){
  std::vector<int> localMaximas;
  std::map<int, jFEXForwardJetsInfo> localMaximaLists = FcalJetsTowerIDLists();
  size_t isLocalMaxima = 0;
  int totalLM = 0;
  for(std::map<int, jFEXForwardJetsInfo>::iterator it = localMaximaLists.begin(); it != (localMaximaLists.end()); ++it)
  {
    isLocalMaxima = 0;
    int myTTKey = it->first;
    jFEXForwardJetsInfo myFCALJetInfoClass = it ->second;
    //Local maxima check takes place here
    SG::ReadHandle<jTowerContainer> jk_jFEXForwardJetsAlgo_jTowerContainer(m_jFEXForwardJetsAlgo_jTowerContainerKey/*,ctx*/);
    float centre_phi = myFCALJetInfoClass.getCentreTTPhi();
    float centre_eta = myFCALJetInfoClass.getCentreTTPhi();
    float centre_energy = myFCALJetInfoClass.getSeedET();
    std::vector<int> TTinSeed = myFCALJetInfoClass.getTTinSeed();
    for (std::size_t i = 0, max = TTinSeed.size(); i != max; ++i){
      float seed_phi = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(TTinSeed[i])->centrePhi();
      float seed_eta = jk_jFEXForwardJetsAlgo_jTowerContainer->findTower(TTinSeed[i])->centreEta();

      float delta_phi = seed_phi - centre_phi;
      float delta_eta = seed_eta - centre_eta;
      float seed_energy = localMaximaLists[TTinSeed[i]].getSeedET(); 
       
      if( ((delta_eta + delta_phi) < 0) || ((delta_eta + delta_phi == 0) && (delta_eta < 0)) )
      {
        if(centre_energy >= seed_energy){isLocalMaxima++;}
      }
      else
      {
        if(centre_energy > seed_energy){isLocalMaxima++;}
      }
     }     
    //if it is a local maxima, here we save the TT ID
    if((isLocalMaxima == TTinSeed.size()) &&(isLocalMaxima !=0)){localMaximas.push_back(myTTKey); totalLM++;}  
  }
  
  //now with a list of localmaxima centre TT IDs, we can remove all the other not LM keys and jfexforward jet classes from the map
  for(std::map<int, jFEXForwardJetsInfo>::iterator it = localMaximaLists.begin(); it !=localMaximaLists.end(); ++it)
  {
    std::vector<int>::iterator itr;
    itr = std::find(localMaximas.begin(), localMaximas.end(), it->first);
    if(itr !=localMaximas.end())
    {continue;}
    else{
      localMaximaLists.erase(it->first);
    }
  }

 return localMaximaLists;
}

std::map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::calculateJetETs(){
  std::map<int, jFEXForwardJetsInfo> localMaximas = isSeedLocalMaxima();
  static constexpr float TT_Size = M_PI/32;

  //here we loop over all trigger towers, fcal 0 1 2 and calculate energy of TT which lays in the 0.4
  //and 0.8 radius to be stored. 
  for(std::map<int, jFEXForwardJetsInfo>::iterator it = localMaximas.begin(); it !=localMaximas.end(); ++it){
    int key = it->first;
    float centreTT_phi = localMaximas[key].getCentreTTPhi();
    float centreTT_eta = localMaximas[key].getCentreTTEta();
    
    for(int nphi = 0; nphi < FEXAlgoSpaceDefs::jFEX_algoSpace_height; nphi++) {
      for(int neta = 0; neta < FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width; neta++) {

        if((m_jFEXalgoTowerID[nphi][neta] == (it->first))){continue;}
        float TT_phi = globalPhi(nphi, neta);
        float TT_eta = globalEta(nphi, neta);
        float R = sqrt(pow((centreTT_eta - TT_eta),2) + pow((centreTT_phi - TT_phi),2));

        if((R> (2 * TT_Size)) && (R< (4 * TT_Size))){ 
          localMaximas[it->first].addToFirstEnergyRingET(getTTowerET(nphi, neta));
          if(m_storeEnergyRingTTIDs){localMaximas[it->first].includeTTIDinFirstER(m_jFEXalgoTowerID[nphi][neta]);}
        }
 
        if((R> (4 * TT_Size)) && (R< (8 * TT_Size))){
          localMaximas[it->first].addToSecondEnergyRingET(getTTowerET(nphi, neta));
          if(m_storeEnergyRingTTIDs){localMaximas[it->first].includeTTIDinSecondER(m_jFEXalgoTowerID[nphi][neta]);}
        }
 
      }
    }
  }
  return localMaximas;
}

}// end of namespace LVL1

