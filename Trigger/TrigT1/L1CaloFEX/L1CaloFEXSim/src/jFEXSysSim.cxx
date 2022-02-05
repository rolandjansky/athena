/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXSysSim  -  description
//                              -------------------
//     begin                : 19 10 2020
//     email                : jacob.julian.kempster@cern.ch alison.elliot@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/jFEXSysSim.h"
#include "L1CaloFEXSim/jFEXSim.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h" 
#include "xAODTrigger/jFexSRJetRoIAuxContainer.h"

#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIAuxContainer.h"

#include "xAODTrigger/jFexTauRoI.h"
#include "xAODTrigger/jFexTauRoIContainer.h" 
#include "xAODTrigger/jFexTauRoIAuxContainer.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"

#include <ctime>

namespace LVL1 {
  
  
  // default constructor for persistency

  jFEXSysSim::jFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
  {
    declareInterface<IjFEXSysSim>(this);

  }

    
  /** Destructor */
  //jFEXSysSim::~jFEXSysSim()
  //{
  //}

  //================ Initialisation =================================================

  StatusCode jFEXSysSim::initialize()
  {
    
    ATH_CHECK(m_jTowerContainerSGKey.initialize());

    ATH_CHECK(m_jFEXSimTool.retrieve() );
 
    ATH_CHECK(m_jFexSRJetOutKey.initialize());
    ATH_CHECK(m_jFexLRJetOutKey.initialize());
    ATH_CHECK(m_jFexTauOutKey.initialize());
    ATH_CHECK(m_jFexSumETOutKey.initialize());
    ATH_CHECK(m_jFexMETOutKey.initialize());

    return StatusCode::SUCCESS;
  }

  //================ Finalisation =================================================

  StatusCode jFEXSysSim::finalize()
  {
    return StatusCode::SUCCESS;
  }

  
  void jFEXSysSim::init()  {

  }

  void jFEXSysSim::cleanup()  {

    m_jFEXCollection.clear();
    m_jTowersColl.clear();

  }


  int jFEXSysSim::calcTowerID(int eta, int phi, int mod)  {

    return ((64*eta) + phi + mod);

  }

  StatusCode jFEXSysSim::execute(jFEXOutputCollection* inputOutputCollection)  {    

    SG::ReadHandle<LVL1::jTowerContainer> this_jTowerContainer(m_jTowerContainerSGKey/*,ctx*/);
    if(!this_jTowerContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jTowerContainerSGKey.key());
      return StatusCode::FAILURE;
    }

    m_allSmallRJetTobs.clear();
    m_allLargeRJetTobs.clear();
    m_alltauTobs.clear();
    m_allMetTobs.clear();
    m_allsumEtTobs.clear();
    // We need to split the towers into 6 blocks in eta and 4 blocks in phi.

    // boundaries in eta: -2.5, -1.6, -0.8, 0.0, 0.8, 1.6, 2.5
    // Written explicitly:
    // -2.5 -> -0.8  (in reality this will be -4.9 to -0.8 , but we're ignoring the forward region for the time being...) [core is -4.9 to -1.6] 
    // -2.4 -> -0.0 [core is -1.6 to -0.8]
    // -1.6 -> 0.8 [core is -0.8 to -0.0]
    // -0.8 -> 1.6 [core is [0.0 to 0.8]
    // 0.0 -> 2.4 [core is 0.8 to 1.6]
    // 0.8 -> 2.5 (in reality this will be 0.8 to 4.9 , but we're ignoring the forward region for the time being...) [core is 1.6 to 4.9]

    //----------------------------------------------WRONG! THE FPGAs SPLIT IN PHI, NOT THE FEXs------------------------------------------------
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // boundaries in phi: 0.0, 1.6, 3.2, 4.8, 6.4
    // Written explicitly:
    // 5.6 -> 2.4 [core is 0.0 to 1.6]
    // 0.8 -> 4.0 [core is 1.6 to 3.2]
    // 2.4 -> 5.6 [core is 3.2 to 4.8]
    // 4.0 -> 0.8 [core is 4.8 to 6.4]
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    //----------------------------------------------WRONG! THE FPGAs SPLIT IN PHI, NOT THE FEXs------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // C-SIDE NEGATIVE JFEX
    // LEFT-MOST
    // -4.9 to -0.8 [core is -4.9 to -1.6]
    // DO THE LEFT-MOST (NEGATIVE ETA) JFEX FIRST
    //id_modifier + phi + (64 * eta)
    int fcal2Eta = 3; int fcal2Phi = 0; int fcal2Mod = 1100000;
    int initialFCAL2 = calcTowerID(fcal2Eta,fcal2Phi,fcal2Mod); //1100192
    int fcal1Eta = 7; int fcal1Phi = 0; int fcal1Mod = 900000;
    int initialFCAL1 = calcTowerID(fcal1Eta,fcal1Phi,fcal1Mod); //900448
    int fcal0Eta = 11; int fcal0Phi = 0; int fcal0Mod = 700000;
    int initialFCAL0 = calcTowerID(fcal0Eta,fcal0Phi,fcal0Mod); //700704
    int emecEta = 28; int emecPhi = 0; int emecMod = 500000;
    int initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //501792
    int transEta = 14; int transPhi = 0; int transMod = 300000;
    int initialTRANS = calcTowerID(transEta,transPhi,transMod); //300896;
    int embEta = 13; int embPhi = 0; int embMod = 100000;
    int initialEMB = calcTowerID(embEta,embPhi,embMod); //100832

    uint8_t thisJFEX = 0;
    // jFEX 0
    thisJFEX = 0;
    

    // let's work fully out to in (sort of)
    // Let's go with FCAL2 first
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL;
    tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.reserve(1600);

    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width];

    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++){
      for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width; j++){
	tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[i][j] = 0;
      }
    }

    int rows = sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL / sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[0];
    int cols = sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[0] / sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[0][0];

    // set the FCAL2 part
    for(int thisCol=0; thisCol<4; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL2 - (thisCol * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }
    //---
    // Let's go with FCAL1
    // set the FCAL1 part
    for(int thisCol=4; thisCol<12; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL1 - ((thisCol-4) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }
    //---
    // Let's go with FCAL0
    // set the FCAL0 part
    for(int thisCol=12; thisCol<24; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL0 - ((thisCol-12) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }
    //---
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    // set the next EMEC part
    for(int thisCol=24; thisCol<28; thisCol++){
      for(int thisRow=0; thisRow<rows/2; thisRow++){

        int towerid = initialEMEC - ((thisCol-24) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }
    // set the EMEC part
    for(int thisCol=28; thisCol<38; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC - ((thisCol-24) * 64) + thisRow; //note special case -24 rather than -28, this *is* deliberate

	tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){

      int towerid = initialTRANS + thisRow;

      tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][38] = towerid;
      tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }
    // set the EMB part
    for(int thisCol = 39; thisCol < 45; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){

        int towerid = initialEMB - ( (thisCol-39) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL[thisRow][thisCol];
            if(tmptowerid == 0 ) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteForwardASide(tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // C-SIDE NEGATIVE JFEX
    // INNER-LEFT
    // -2.4 -> -0.0 [core is -1.6 to -0.8]
    // DO THE INNER-LEFT (NEGATIVE ETA) JFEX SECOND
    //id_modifier + phi + (64 * eta)
    emecEta = 23; emecPhi = 0; emecMod = 500000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //500472;
    transEta = 14; transPhi = 0; transMod = 300000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //300896;
    embEta = 13; embPhi = 0; embMod = 100000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //100832
    
    // jFEX 1
    thisJFEX = 1;
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_1;
    
    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_1 [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width];
    
    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++){
      for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width; j++){
        tmp_jTowersIDs_subset_1[i][j] = 0;
      }
    }

    rows = sizeof tmp_jTowersIDs_subset_1 / sizeof tmp_jTowersIDs_subset_1[0];
    cols = sizeof tmp_jTowersIDs_subset_1[0] / sizeof tmp_jTowersIDs_subset_1[0][0];
    
    // set the EMEC part
    for(int thisCol = 0; thisCol < 9; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC - (thisCol * 64) + thisRow;
	
	tmp_jTowersIDs_subset_1[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_1.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }
    
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++) {

        int towerid = initialTRANS + thisRow;

        tmp_jTowersIDs_subset_1[thisRow][9] = towerid;
        tmp_jTowersColl_subset_1.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    // set the EMB part
    for(int thisCol = 10; thisCol < cols; thisCol++) {
        for(int thisRow=0; thisRow<rows; thisRow++) {

            int towerid = initialEMB - ( (thisCol-10) * 64) + thisRow ;

            tmp_jTowersIDs_subset_1[thisRow][thisCol] = towerid;
            tmp_jTowersColl_subset_1.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

        }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_1[thisRow][thisCol];
            if(tmptowerid == 0) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteBarrel(tmp_jTowersIDs_subset_1, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------    
    // C-SIDE NEGATIVE JFEXs
    // CENTRAL-LEFT
    // -1.6 -> 0.8 [core is -0.8 to -0.0]
    // DO THE CENTRAL-LEFT JFEXs (NEGATIVE ETA) THIRD
    //id_modifier + phi + (64 * eta)
    emecEta = 15; emecPhi = 0; emecMod = 500000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //500960;
    transEta = 14; transPhi = 0; transMod = 300000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //300896;
    embEta = 13; embPhi = 0; embMod = 100000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //100832

    // jFEX 2
    thisJFEX = 2;
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_2;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_2 [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width];
    
    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++) {
        for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width; j++) {
            tmp_jTowersIDs_subset_2[i][j] = 0;
        }
    }


    rows = sizeof tmp_jTowersIDs_subset_2 / sizeof tmp_jTowersIDs_subset_2[0];
    cols = sizeof tmp_jTowersIDs_subset_2[0] / sizeof tmp_jTowersIDs_subset_2[0][0];

    // set the EMEC part
    for(int thisRow=0; thisRow<rows; thisRow++) {

        int towerid = initialEMEC /*- (thisCol * 64)*/  + thisRow;

        tmp_jTowersIDs_subset_2[thisRow][0] = towerid;
        tmp_jTowersColl_subset_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++) {

        int towerid = initialTRANS + thisRow;

        tmp_jTowersIDs_subset_2[thisRow][1] = towerid;
        tmp_jTowersColl_subset_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    // set the negative EMB part
    for(int thisCol = 2; thisCol < cols-8; thisCol++) {
        for(int thisRow=0; thisRow<rows; thisRow++) {
            int towerid = -1;

            int tmp_initEMB = initialEMB;

            towerid = tmp_initEMB - ( (thisCol-2) * 64) + thisRow;
            tmp_jTowersIDs_subset_2[thisRow][thisCol] = towerid;
            
            tmp_jTowersColl_subset_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

        }
    }
      
    embEta = 0; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200000
    
    // set the positive EMB part
    for(int thisCol = 16; thisCol < cols; thisCol++) {
        for(int thisRow=0; thisRow<rows; thisRow++) {
            int towerid = -1;

            int tmp_initEMB = initialEMB;

            towerid = tmp_initEMB + ( (thisCol-16) * 64) + thisRow;
            tmp_jTowersIDs_subset_2[thisRow][thisCol] = towerid;

            tmp_jTowersColl_subset_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

        }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_2[thisRow][thisCol];
            if(tmptowerid == 0) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }

    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteBarrel(tmp_jTowersIDs_subset_2, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) )); 
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // CENTRAL-RIGHT JFEXs
    // -0.8 -> 1.6 [core is [0.0 to 0.8]
    // DO THE CENTRAL-RIGHT JFEXs (POSITIVE ETA) FOURTH
    //id_modifier + phi + (64 * eta)
    emecEta = 15; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600960;
    transEta = 14; transPhi = 0; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400896;
    embEta = 7; embPhi = 0; embMod = 100000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //100448

    // jFEX 3
    thisJFEX = 3;
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_3;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_3 [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width];
    
    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++){
      for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width; j++){
        tmp_jTowersIDs_subset_3[i][j] = 0;
      }
    }


    rows = sizeof tmp_jTowersIDs_subset_3 / sizeof tmp_jTowersIDs_subset_3[0];
    cols = sizeof tmp_jTowersIDs_subset_3[0] / sizeof tmp_jTowersIDs_subset_3[0][0];
    
    // set the negative EMB part
    for(int thisCol = 0; thisCol < 8; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
        int towerid = -1;

        int tmp_initEMB = initialEMB;

        towerid = tmp_initEMB - ( (thisCol) * 64) + thisRow;

        tmp_jTowersIDs_subset_3[thisRow][thisCol] = towerid;

        tmp_jTowersColl_subset_3.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    embEta = 0; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200000
    // set the positive EMB part
    for(int thisCol = 8; thisCol < 22; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = -1;
	
	int tmp_initEMB = initialEMB;
	
	towerid = tmp_initEMB + ( (thisCol-8) * 64) + thisRow;
	
	tmp_jTowersIDs_subset_3[thisRow][thisCol] = towerid;
	
	tmp_jTowersColl_subset_3.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }

    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;

      tmp_jTowersIDs_subset_3[thisRow][22] = towerid;
      tmp_jTowersColl_subset_3.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    // set the EMEC part
    for(int thisRow=0; thisRow<rows; thisRow++){
      int towerid = initialEMEC + /*( (thisCol-8) * 64)*/ + thisRow;

      tmp_jTowersIDs_subset_3[thisRow][23] = towerid;
      tmp_jTowersColl_subset_3.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_3[thisRow][thisCol];
            if(tmptowerid == 0) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }
    
    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteBarrel(tmp_jTowersIDs_subset_3, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // INNER-RIGHT JFEXs
    // 0.0 -> 2.4 [core is 0.8 to 1.6]
    // DO THE INNER-RIGHT JFEXs (POSITIVE ETA) FIFTH
    emecEta = 15; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600960;
    transEta = 14; transPhi = 0; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400896;
    embEta = 0; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200000;

    // jFEX 4
    thisJFEX = 4;
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_4;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_4 [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width];
    
    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++){
      for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width; j++){
        tmp_jTowersIDs_subset_4[i][j] = 0;
      }
    }

    rows = sizeof tmp_jTowersIDs_subset_4 / sizeof tmp_jTowersIDs_subset_4[0];
    cols = sizeof tmp_jTowersIDs_subset_4[0] / sizeof tmp_jTowersIDs_subset_4[0][0];
    
    // set the EMB part
    for(int thisCol = 0; thisCol < 14; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = initialEMB + ( (thisCol) * 64) + thisRow;
	
	tmp_jTowersIDs_subset_4[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_4.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;
      
      tmp_jTowersIDs_subset_4[thisRow][14] = towerid;
      tmp_jTowersColl_subset_4.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
      
    }
    // set the EMEC part
    for(int thisCol = 15; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = initialEMEC + ( (thisCol-15) * 64) + thisRow;
	
	tmp_jTowersIDs_subset_4[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_4.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_4[thisRow][thisCol];
            if(tmptowerid == 0) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }
    
    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteBarrel(tmp_jTowersIDs_subset_4, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // RIGHT-MOST
    // 0.8 -> 2.5 (in reality this will be 0.8 to 4.9 , but we're ignoring the forward region for the time being...) [core is 1.6 to 4.9]
    // DO THE RIGHT-MOST (POSITIVE ETA) JFEXs SIXTH
    //id_modifier + phi + (64 * eta)
    fcal2Eta = 0; fcal2Phi = 0; fcal2Mod = 1200000;
    initialFCAL2 = calcTowerID(fcal2Eta,fcal2Phi,fcal2Mod); //1200000
    fcal1Eta = 0; fcal1Phi = 0; fcal1Mod = 1000000;
    initialFCAL1 = calcTowerID(fcal1Eta,fcal1Phi,fcal1Mod); //1000000
    fcal0Eta = 0; fcal0Phi = 0; fcal0Mod = 800000;
    initialFCAL0 = calcTowerID(fcal0Eta,fcal0Phi,fcal0Mod); //800000
    emecEta = 15; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600960;
    transEta = 14; transPhi = 0; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400896;
    embEta = 8; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200512;

    // jFEX 5
    thisJFEX = 5;
    
    
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::unordered_map<int,jTower> tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2;

    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2 [2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width];

    // zero the matrix out
    for (int i = 0; i<2*FEXAlgoSpaceDefs::jFEX_algoSpace_height; i++){
      for (int j = 0; j<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width; j++){
        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[i][j] = 0;
      }
    }
    
    rows = sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2 / sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[0];
    cols = sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[0] / sizeof tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[0][0];

    // set the EMB part
    for(int thisCol = 0; thisCol < 6; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
        int towerid = initialEMB + ( (thisCol) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;

      tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][6] = towerid;
      tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

    }

    // set the EMEC part
    for(int thisCol=7; thisCol<17; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC + ((thisCol-7) * 64) + thisRow;
	
	tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
      }
    }

    // set the next EMEC part
    for(int thisCol=17; thisCol<21; thisCol++){
      for(int thisRow=0; thisRow<rows/2; thisRow++){

        int towerid = initialEMEC + ((thisCol-7) * 64) + thisRow; //note special case -7 rather than -17, this *is* deliberate

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    //-----
    // Let's go with FCAL0
    // set the FCAL0 part
    for(int thisCol=21; thisCol<33; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL0 + ((thisCol-21) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    //---
    // Let's go with FCAL1
    // set the FCAL1 part
    for(int thisCol=33; thisCol<41; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL1 + ((thisCol-33) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }

    //---
    // Let's go with FCAL2
    // set the FCAL2 part
    for(int thisCol=41; thisCol<45; thisCol++){
      for(int thisRow=0; thisRow<rows/4; thisRow++){

        int towerid = initialFCAL2 + ((thisCol-41) * 64) + thisRow;

        tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_ENDCAP_AND_EMB_AND_FCAL_2.insert( std::unordered_map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      }
    }
    //---

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--) {
        for (int thisCol=0; thisCol<cols; thisCol++) {
            int tmptowerid = tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2[thisRow][thisCol];
            if(tmptowerid == 0) continue;
            const LVL1::jTower* tmptower = this_jTowerContainer->findTower(tmptowerid);
            const float tmptowereta = tmptower->eta();
            const float tmptowerphi = tmptower->phi();
            if(thisCol != cols-1) {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  ");
            }
            else {
                ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |");
            }
        }
      }
    }

    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->ExecuteForwardCSide(tmp_jTowersIDs_subset_ENDCAP_AND_EMB_AND_FCAL_2, inputOutputCollection));
    m_allSmallRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_allLargeRJetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getLargeRJetTOBs() ) ));
    m_alltauTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> >::value_type(thisJFEX,(m_jFEXSimTool->getTauTOBs() ) ));
    m_allsumEtTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getSumEtTOBs() ) ));
    m_allMetTobs.insert(std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> >::value_type(thisJFEX,(m_jFEXSimTool->getMetTOBs() ) ));
    m_jFEXSimTool->reset();

    
    //-----------------------------------------------------FILLING EDMs--------------------------------------------------------------------------------
    //---SRJet EDM
    auto jSRJetContainer = std::make_unique<xAOD::jFexSRJetRoIContainer> ();
    std::unique_ptr< xAOD::jFexSRJetRoIAuxContainer > jSRJetAuxContainer = std::make_unique<xAOD::jFexSRJetRoIAuxContainer> ();
    jSRJetContainer->setStore(jSRJetAuxContainer.get());

    // iterate over all SRJEt Tobs and fill EDM with them   
    for( auto const& [jfex, MODULE_tobs] : m_allSmallRJetTobs ) {
        uint8_t fpgaNum =0;
        for(auto &FPGA_tob : MODULE_tobs) {
            for(auto &tob: FPGA_tob) {
                float_t eta = -99;
                float_t phi = -99;
                if(tob.at(1) != 0){
                    eta = (this_jTowerContainer->findTower(tob.at(1)))->centreEta();
                    phi = (this_jTowerContainer->findTower(tob.at(1)))->centrephi_toPI();                   
                }
                
                ATH_CHECK(fillSRJetEDM(jfex, fpgaNum,tob.at(0), eta, phi, jSRJetContainer));
            }
            fpgaNum++;
        }
        
    }
    
    SG::WriteHandle<xAOD::jFexSRJetRoIContainer_v1> outputjFexSRJetHandle(m_jFexSRJetOutKey/*, ctx*/);
    ATH_MSG_DEBUG("  write: " << outputjFexSRJetHandle.key() << " = " << "..." );
    ATH_CHECK(outputjFexSRJetHandle.record(std::move(jSRJetContainer),std::move(jSRJetAuxContainer)));

    //---LRJet EDM
    auto jLRJetContainer = std::make_unique<xAOD::jFexLRJetRoIContainer> ();
    std::unique_ptr< xAOD::jFexLRJetRoIAuxContainer > jLRJetAuxContainer = std::make_unique<xAOD::jFexLRJetRoIAuxContainer> ();
    jLRJetContainer->setStore(jLRJetAuxContainer.get());

    // iterate over all LRJEt Tobs and fill EDM with them
    for(auto const& [jfex, MODULE_tobs] : m_allLargeRJetTobs ) {
        uint8_t fpgaNum =0;
        for(auto &FPGA_tob : MODULE_tobs) {
            for(auto&tob: FPGA_tob) {
                float_t eta = -99;
                float_t phi = -99;
                if(tob.at(1) != 0){
                    eta = (this_jTowerContainer->findTower(tob.at(1)))->centreEta();
                    phi = (this_jTowerContainer->findTower(tob.at(1)))->centrephi_toPI();                   
                }

                ATH_CHECK(fillLRJetEDM(jfex,fpgaNum, tob.at(0), eta, phi, jLRJetContainer));
            }        
            fpgaNum++;
        }

    }

    SG::WriteHandle<xAOD::jFexLRJetRoIContainer_v1> outputjFexLRJetHandle(m_jFexLRJetOutKey/*, ctx*/);
    ATH_MSG_DEBUG("  write: " << outputjFexLRJetHandle.key() << " = " << "..." );
    ATH_CHECK(outputjFexLRJetHandle.record(std::move(jLRJetContainer),std::move(jLRJetAuxContainer)));
    
    //---Tau EDM
    auto jTauContainer = std::make_unique<xAOD::jFexTauRoIContainer> ();
    std::unique_ptr< xAOD::jFexTauRoIAuxContainer > jTauAuxContainer = std::make_unique<xAOD::jFexTauRoIAuxContainer> ();
    jTauContainer->setStore(jTauAuxContainer.get());
    //iterate over all Tau Tobs and fill EDM with 
    for( auto const& [jfex, MODULE_tobs] : m_alltauTobs ) {
        uint8_t fpgaNum =0;
        for(auto &FPGA_tob : MODULE_tobs) {
            for(auto &tob : FPGA_tob) {
                float_t eta = -99;
                float_t phi = -99;
                if(tob.at(1) != 0){
                    eta = (this_jTowerContainer->findTower(tob.at(1)))->centreEta();
                    phi = (this_jTowerContainer->findTower(tob.at(1)))->centrephi_toPI();                   
                }
                
                ATH_CHECK(fillTauEDM(jfex,fpgaNum, tob.at(0), eta, phi, jTauContainer));
            }
            fpgaNum++;
        }
        
    }

    SG::WriteHandle<xAOD::jFexTauRoIContainer_v1> outputjFexTauHandle(m_jFexTauOutKey/*, ctx*/);
    ATH_MSG_DEBUG("  write: " << outputjFexTauHandle.key() << " = " << "..." );
    ATH_CHECK(outputjFexTauHandle.record(std::move(jTauContainer),std::move(jTauAuxContainer)));
    
    
    //---SumET EDM
    auto jSumETContainer = std::make_unique<xAOD::jFexSumETRoIContainer> ();
    std::unique_ptr< xAOD::jFexSumETRoIAuxContainer > jSumETAuxContainer = std::make_unique<xAOD::jFexSumETRoIAuxContainer> ();
    jSumETContainer->setStore(jSumETAuxContainer.get());    
    
    for( auto const& [jfex, MODULE_tobs] : m_allsumEtTobs ) {
        uint8_t fpgaNum =0;
        for(auto &FPGA_tob : MODULE_tobs) {
            for(auto &tob : FPGA_tob) {
                ATH_CHECK(fillSumEtEDM(jfex,fpgaNum, tob, jSumETContainer));
            }
            fpgaNum+=2;
        }
        
    }   

    SG::WriteHandle<xAOD::jFexSumETRoIContainer_v1> outputjFexSumETHandle(m_jFexSumETOutKey/*, ctx*/);
    ATH_MSG_DEBUG("  write: " << outputjFexSumETHandle.key() << " = " << "..." );
    ATH_CHECK(outputjFexSumETHandle.record(std::move(jSumETContainer),std::move(jSumETAuxContainer)));    
    
    //---MET EDM 
    auto jMETContainer = std::make_unique<xAOD::jFexMETRoIContainer> ();
    std::unique_ptr< xAOD::jFexMETRoIAuxContainer > jMETAuxContainer = std::make_unique<xAOD::jFexMETRoIAuxContainer> ();
    jMETContainer->setStore(jMETAuxContainer.get());    

    for( auto const& [jfex, MODULE_tobs] : m_allMetTobs ) {
        uint8_t fpgaNum =0;
        for(auto &FPGA_tob : MODULE_tobs) {
            for(auto &tob : FPGA_tob) {
                ATH_CHECK(fillMetEDM(jfex,fpgaNum, tob, jMETContainer));
            }
            fpgaNum+=2;
        }
        
    }     

    SG::WriteHandle<xAOD::jFexMETRoIContainer_v1> outputjFexMETHandle(m_jFexMETOutKey/*, ctx*/);
    ATH_MSG_DEBUG("  write: " << outputjFexMETHandle.key() << " = " << "..." );
    ATH_CHECK(outputjFexMETHandle.record(std::move(jMETContainer),std::move(jMETAuxContainer)));   



    //Send TOBs to bytestream?
    // ToDo
    // To implement
    // {--Implement--}

    return StatusCode::SUCCESS;

  }


    StatusCode jFEXSysSim::fillSRJetEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexSRJetRoIContainer > &jContainer) {

        xAOD::jFexSRJetRoI* my_EDM = new xAOD::jFexSRJetRoI();
        jContainer->push_back( my_EDM );

        my_EDM->initialize(jFexNum, fpgaNumber, tobWord, eta, phi);

        ATH_MSG_DEBUG(" setting SRJet jFEX Number:  " << +my_EDM->jFexNumber() << " et: " << my_EDM->et() << " eta: " << my_EDM->eta() <<" / "<< eta <<  " phi: " << my_EDM->phi()<<" / "<< phi  );

        return StatusCode::SUCCESS;

    }
      
      
    StatusCode jFEXSysSim::fillTauEDM(uint8_t jFexNum,uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexTauRoIContainer > &jContainer) {

        xAOD::jFexTauRoI* my_EDM = new xAOD::jFexTauRoI();
        jContainer->push_back( my_EDM );

        my_EDM->initialize(jFexNum, fpgaNumber, tobWord, eta, phi);

        ATH_MSG_DEBUG(" setting tau jFEX Number:  " << +my_EDM->jFexNumber() << " et: " << my_EDM->et() << " eta: " << my_EDM->eta() <<" / "<< eta <<  " phi: " << my_EDM->phi()<<" / "<< phi  );

        return StatusCode::SUCCESS;

    }

    StatusCode jFEXSysSim::fillLRJetEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexLRJetRoIContainer > &jContainer) {

        xAOD::jFexLRJetRoI* my_EDM = new xAOD::jFexLRJetRoI();
        jContainer->push_back( my_EDM );

        my_EDM->initialize(jFexNum, fpgaNumber, tobWord, eta, phi);

        ATH_MSG_DEBUG(" setting LRJet jFEX Number:  " << +my_EDM->jFexNumber() << " et: " << my_EDM->et() << " eta: " << my_EDM->eta() <<" / "<< eta <<  " phi: " << my_EDM->phi()<<" / "<< phi  );

        return StatusCode::SUCCESS;

    }

    StatusCode jFEXSysSim::fillSumEtEDM(uint8_t jFexNum,uint8_t fpgaNumber, uint32_t tobWord, std::unique_ptr< xAOD::jFexSumETRoIContainer > &jContainer) {

        xAOD::jFexSumETRoI* my_EDM = new xAOD::jFexSumETRoI();
        jContainer->push_back( my_EDM );
        
        my_EDM->initialize(jFexNum, fpgaNumber, tobWord);
        
        ATH_MSG_DEBUG(" setting SumET jFEX Number:  " << +my_EDM->jFexNumber() << " Et_up: " << my_EDM->tobEt_upper() << " Et_down: " << my_EDM->tobEt_lower() <<  " sat_up: " << my_EDM->tobSat_upper()<<  " sat_low: " << my_EDM->tobSat_lower());
        
        return StatusCode::SUCCESS;

    }   

    StatusCode jFEXSysSim::fillMetEDM(uint8_t jFexNum,uint8_t fpgaNumber, uint32_t tobWord, std::unique_ptr< xAOD::jFexMETRoIContainer > &jContainer) {

        xAOD::jFexMETRoI* my_EDM = new xAOD::jFexMETRoI();
        jContainer->push_back( my_EDM );

        my_EDM->initialize(jFexNum, fpgaNumber, tobWord);

        ATH_MSG_DEBUG(" setting MET jFEX Number:  " << +my_EDM->jFexNumber() << " Et_x: " << my_EDM->tobEx() << " Et_y: " << my_EDM->tobEy() <<  " sat: " << my_EDM->tobSat()<<  " res: " << my_EDM->tobRes() );
        
        return StatusCode::SUCCESS;

    }


  
} // end of namespace bracket


