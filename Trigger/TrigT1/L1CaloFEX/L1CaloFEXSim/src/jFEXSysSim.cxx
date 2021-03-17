/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ServiceHandle.h"


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

    ATH_CHECK( m_jFEXSimTool.retrieve() );

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

  StatusCode jFEXSysSim::execute()  {    

    SG::ReadHandle<LVL1::jTowerContainer> this_jTowerContainer(m_jTowerContainerSGKey/*,ctx*/);
    if(!this_jTowerContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jTowerContainerSGKey.key());
      return StatusCode::FAILURE;
    }

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
    // -2.5 -> -0.8  (in reality this will be -4.9 to -0.8 , but we're ignoring the forward region for the time being...) [core is -4.9 to -1.6]
    // DO THE LEFT-MOST (NEGATIVE ETA) JFEX FIRST
    //id_modifier + phi + (64 * eta)
    int emecEta = 24; int emecPhi = 0; int emecMod = 500000;
    int initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //501536;
    int transEta = 14; int transPhi = 0; int transMod = 300000;
    int initialTRANS = calcTowerID(transEta,transPhi,transMod); //300896;
    int embEta = 13; int embPhi = 0; int embMod = 100000;
    int initialEMB = calcTowerID(embEta,embPhi,embMod); //100832

    int thisJFEX = 0;
    // jFEX 0
    thisJFEX = 0;
    
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::map<int,jTower> tmp_jTowersColl_subset;
    
    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset [16*4][17];
    
    int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
    int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
    
    // set the EMEC part
    for(int thisCol=0; thisCol<10; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC - (thisCol * 64) + thisRow;
	
	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){

      int towerid = initialTRANS + thisRow;

      //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

      tmp_jTowersIDs_subset[thisRow][10] = towerid;
      tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

    }

    // set the EMB part
    for(int thisCol = 11; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){

        int towerid = initialEMB - ( (thisCol-11) * 64) + thisRow;

        //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

        tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

      }
    }

    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }
    
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
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
    std::map<int,jTower> tmp_jTowersColl_subset_1;
    
    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_1 [16*4][24];
    
    rows = sizeof tmp_jTowersIDs_subset_1 / sizeof tmp_jTowersIDs_subset_1[0];
    cols = sizeof tmp_jTowersIDs_subset_1[0] / sizeof tmp_jTowersIDs_subset_1[0][0];
    
    // set the EMEC part
    for(int thisCol = 0; thisCol < 9/*cols*/; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC - (thisCol * 64) + thisRow;
	
	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_1[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_1.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      
      int towerid = initialTRANS + thisRow;
      
      //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
      
      tmp_jTowersIDs_subset_1[thisRow][9] = towerid;
      tmp_jTowersColl_subset_1.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
      
      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
      
    }
    
    // set the EMB part
    for(int thisCol = 10; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMB - ( (thisCol-10) * 64) + thisRow ;
	
	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_1[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_1.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    
    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset_1[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }
      
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset_1));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
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
    std::map<int,jTower> tmp_jTowersColl_subset_2;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_2 [16*4][24];
    
    rows = sizeof tmp_jTowersIDs_subset_2 / sizeof tmp_jTowersIDs_subset_2[0];
    cols = sizeof tmp_jTowersIDs_subset_2[0] / sizeof tmp_jTowersIDs_subset_2[0][0];
    
    // set the EMEC part
    for(int thisRow=0; thisRow<rows; thisRow++){
      
      int towerid = initialEMEC /*- (thisCol * 64)*/  + thisRow;
      
      //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
      
      tmp_jTowersIDs_subset_2[thisRow][0] = towerid;
      tmp_jTowersColl_subset_2.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
      
      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
      
    }

    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){

      int towerid = initialTRANS + thisRow;

      //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

      tmp_jTowersIDs_subset_2[thisRow][1] = towerid;
      tmp_jTowersColl_subset_2.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

    }


    // set the negative EMB part
    for(int thisCol = 2; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = -1;
	
	int tmp_initEMB = initialEMB;
	
	towerid = tmp_initEMB - ( (thisCol-2) * 64) + thisRow;
	
	//if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_2[thisRow][thisCol] = towerid;
	
	tmp_jTowersColl_subset_2.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    embEta = 0; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200000
    // set the positive EMB part
    for(int thisCol = 16; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
        int towerid = -1;

        int tmp_initEMB = initialEMB;

        towerid = tmp_initEMB + ( (thisCol-16) * 64) + thisRow;

        //if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

        tmp_jTowersIDs_subset_2[thisRow][thisCol] = towerid;

        tmp_jTowersColl_subset_2.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

      }
    }

    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset_2[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }

    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset_2));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) )); 
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
    std::map<int,jTower> tmp_jTowersColl_subset_3;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_3 [16*4][24];
    
    rows = sizeof tmp_jTowersIDs_subset_3 / sizeof tmp_jTowersIDs_subset_3[0];
    cols = sizeof tmp_jTowersIDs_subset_3[0] / sizeof tmp_jTowersIDs_subset_3[0][0];
    
    // set the negative EMB part
    for(int thisCol = 0; thisCol < 8; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
        int towerid = -1;

        int tmp_initEMB = initialEMB;

        towerid = tmp_initEMB - ( (thisCol) * 64) + thisRow;

        //if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

        tmp_jTowersIDs_subset_3[thisRow][thisCol] = towerid;

        tmp_jTowersColl_subset_3.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

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
	
	//if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_3[thisRow][thisCol] = towerid;
	
	tmp_jTowersColl_subset_3.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }

    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;

      //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

      tmp_jTowersIDs_subset_3[thisRow][22] = towerid;
      tmp_jTowersColl_subset_3.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

    }

    // set the EMEC part
    for(int thisRow=0; thisRow<rows; thisRow++){
      int towerid = initialEMEC + /*( (thisCol-8) * 64)*/ + thisRow;

      //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

      tmp_jTowersIDs_subset_3[thisRow][23] = towerid;
      tmp_jTowersColl_subset_3.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

    }

    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset_3[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }
    
    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset_3));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
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
    std::map<int,jTower> tmp_jTowersColl_subset_4;
    
    // doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_4 [16*4][24];
    
    rows = sizeof tmp_jTowersIDs_subset_4 / sizeof tmp_jTowersIDs_subset_4[0];
    cols = sizeof tmp_jTowersIDs_subset_4[0] / sizeof tmp_jTowersIDs_subset_4[0][0];
    
    // set the EMB part
    for(int thisCol = 0; thisCol < 14; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = initialEMB + ( (thisCol) * 64) + thisRow;

	
	//if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_4[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_4.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;
      
      //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
      
      tmp_jTowersIDs_subset_4[thisRow][14] = towerid;
      tmp_jTowersColl_subset_4.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
      
      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
      
    }
    // set the EMEC part
    for(int thisCol = 15; thisCol < cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = initialEMEC + ( (thisCol-15) * 64) + thisRow;
	
	//if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_4[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_4.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset_4[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }
    
    //tool use instead
    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset_4));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // RIGHT-MOST
    // 0.8 -> 2.5 (in reality this will be 0.8 to 4.9 , but we're ignoring the forward region for the time being...) [core is 1.6 to 4.9]
    // DO THE RIGHT-MOST (POSITIVE ETA) JFEXs SIXTH
    //id_modifier + phi + (64 * eta)
    emecEta = 15; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600960;
    transEta = 14; transPhi = 0; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400896;
    embEta = 8; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200512;

    // jFEX 5
    thisJFEX = 5;
    
    // decide which subset of towers (and therefore supercells) should go to the jFEX
    std::map<int,jTower> tmp_jTowersColl_subset_5;
    
    // let's try doing this with an array initially just containing tower IDs.
    int tmp_jTowersIDs_subset_5 [16*4][17];
    
    rows = sizeof tmp_jTowersIDs_subset_5 / sizeof tmp_jTowersIDs_subset_5[0];
    cols = sizeof tmp_jTowersIDs_subset_5[0] / sizeof tmp_jTowersIDs_subset_5[0][0];

    // set the EMB part
    for(int thisCol = 0; thisCol < 6; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
        int towerid = initialEMB + ( (thisCol) * 64) + thisRow;


        //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

        tmp_jTowersIDs_subset_5[thisRow][thisCol] = towerid;
        tmp_jTowersColl_subset_5.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

      }
    }
    // set the TRANS part
    for(int thisRow = 0; thisRow < rows; thisRow++){
      int towerid = initialTRANS + thisRow;

      //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi

      tmp_jTowersIDs_subset_5[thisRow][6] = towerid;
      tmp_jTowersColl_subset_5.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

      std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

    }
    
    // set the EMEC part
    for(int thisCol=7; thisCol<cols; thisCol++){
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC + ((thisCol-7) * 64) + thisRow;
	
	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply jFEX overlap in phi
	
	tmp_jTowersIDs_subset_5[thisRow][thisCol] = towerid;
	tmp_jTowersColl_subset_5.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }
    }
    
    ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	int tmptowerid = tmp_jTowersIDs_subset_5[thisRow][thisCol];
	const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  "); }
	else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
      }
    }

    m_jFEXSimTool->init(thisJFEX);
    ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset_5));
    m_alljTobs.insert(std::map<int, std::vector<uint32_t> >::value_type(thisJFEX,(m_jFEXSimTool->getSmallRJetTOBs() ) ));
    m_jFEXSimTool->reset();
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------


    //Collate TOBS returned from jFEXSims. Should that be here?
    // ToDo
    // To implement
    // {--Implement--}


    //Send TOBs to bytestream?
    // ToDo
    // To implement
    // {--Implement--}

    return StatusCode::SUCCESS;

  }

  
} // end of namespace bracket

