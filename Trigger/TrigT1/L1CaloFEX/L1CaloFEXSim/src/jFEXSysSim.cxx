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
    // -2.5 -> -1.6
    // -1.6 -> -0.8
    // -0.8 -> 0.0
    // 0.0 -> 0.8
    // 0.8 -> 1.6
    // 1.6 -> 2.5

    // boundaries in phi: 0.0, 1.6, 3.2, 4.8, 6.4
    // Written explicitly:
    // 0.0 -> 1.6
    // 1.6 -> 3.2
    // 3.2 -> 4.8
    // 4.8 -> 6.4

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // C-SIDE NEGATIVE JFEX
    // LEFT-MOST
    // DO THE LEFT-MOST (NEGATIVE ETA) JFEX FIRST
    int fpgacounter = 0;
    //id_modifier + phi + (64 * eta)
    int emecEta = 24; int emecPhi = 0; int emecMod = 500000;
    int initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //501536;


    int thisJFEX = 0;
    if(true){ // JFEX 0
      thisJFEX = 0;
      
      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;
      
      // let's try doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][9];
      
      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMEC part
      for(int thisCol=0; thisCol<9; thisCol++){
	for(int thisRow=0; thisRow<rows; thisRow++){

	  int towerid = initialEMEC - (thisCol * 64) + thisRow;

	  //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap
	  
	  tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
	  tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

	}
      }

      if(false){
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
      }


      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      //fgpacounter++;

    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // C-SIDE NEGATIVE JFEX
    // INNER-LEFT
    // DO THE INNER-LEFT (NEGATIVE ETA) JFEX SECOND
    fpgacounter = 0;
    //id_modifier + phi + (64 * eta)
    emecEta = 15; emecPhi = 0; emecMod = 500000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //500960;
    int transEta = 14; int transPhi = 0; int transMod = 300000;
    int initialTRANS = calcTowerID(transEta,transPhi,transMod); //300896;
    int embEta = 13; int embPhi = 0; int embMod = 100000;
    int initialEMB = calcTowerID(embEta,embPhi,embMod); //100832

    if(true){ // JFEX 1
      thisJFEX = 1;

      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;

      // let's try doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][8];

      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMEC part
      for(int thisRow=0; thisRow<rows; thisRow++){
	
	int towerid = initialEMEC /*- (thisCol * 64)*/ + thisRow;
	
	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap
	
	tmp_jTowersIDs_subset[thisRow][0/*thisCol*/] = towerid;
	tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }

      // set the TRANS part
      for(int thisRow = 0; thisRow < rows; thisRow++){

	int towerid = initialTRANS + thisRow;

	//if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

	tmp_jTowersIDs_subset[thisRow][1] = towerid;
	tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

      }

      // set the EMB part
      for(int thisCol = 2; thisCol < cols; thisCol++){
	for(int thisRow=0; thisRow<rows; thisRow++){

	  int towerid = initialEMB - ( (thisCol-2) * 64) + thisRow;

	  //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

	  tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
	  tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

	}
      }
      

      if(false){
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
      }


      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      fpgacounter++;

    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------    
    // C-SIDE NEGATIVE JFEXs
    // CENTRAL-LEFT
    // DO THE CENTRAL-LEFT JFEXs (NEGATIVE ETA) THIRD
    fpgacounter = 0;
    //id_modifier + phi + (64 * eta)
    embEta = 7; embPhi = 0; embMod = 100000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //100448

    if(true){ // jFEX 2
      thisJFEX = 2;
      
      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;
      
      // doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][8];

      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMB part
      for(int thisCol = 0; thisCol < cols; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
	  int towerid = -1;

	  int tmp_initEMB = initialEMB;

	  towerid = tmp_initEMB - ( (thisCol) * 64) + thisRow;

	  //if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

          tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;

          tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

        }
      }
      

      if(false){
	ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
        for (int thisRow=rows-1; thisRow>=0; thisRow--){
          for (int thisCol=0; thisCol<cols; thisCol++){
            int tmptowerid = tmp_jTowersIDs_subset[thisRow][thisCol];
            const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
            const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
            if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
            else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
	  }
        }
      }

      //tool use instead
      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      fpgacounter++;

    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // CENTRAL-RIGHT JFEXs
    // DO THE CENTRAL-RIGHT JFEXs (POSITIVE ETA) FOURTH
    fpgacounter = 0;
    //id_modifier + phi + (64 * eta)
    embEta = 0; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200000

    if(true){ // jFEX 3
      thisJFEX = 3;
      if(fpgacounter > 0){  initialEMB += 16; }

      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;

      // doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][8];

      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMB part
      for(int thisCol = 0; thisCol < cols; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
          int towerid = -1;

          int tmp_initEMB = initialEMB;

          towerid = tmp_initEMB + ( (thisCol) * 64) + thisRow;

          //if( (thisJFEX == 22) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

          tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;

          tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

        }
      }

      if(false){
        ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
        for (int thisRow=rows-1; thisRow>=0; thisRow--){
          for (int thisCol=0; thisCol<cols; thisCol++){
            int tmptowerid = tmp_jTowersIDs_subset[thisRow][thisCol];
            const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
            const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
            if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
            else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
          }
        }
      }

      //tool use instead
      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      fpgacounter++;

    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // INNER-RIGHT JFEXs
    // DO THE INNER-RIGHT JFEXs (POSITIVE ETA) FIFTH
    fpgacounter = 0;
    emecEta = 15; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600960;
    transEta = 14; transPhi = 0; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400896;
    embEta = 8; embPhi = 0; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200512;

    if(true){ // jFEX 4
      thisJFEX = 4;

      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;

      // doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][8];

      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMB part
      for(int thisCol = 0; thisCol < 6; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
          int towerid = initialEMB + ( (thisCol) * 64) + thisRow;

          //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

          tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
          tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

        }
      }
      // set the TRANS part
      for(int thisRow = 0; thisRow < rows; thisRow++){
        int towerid = initialTRANS + thisRow;

        //if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

        tmp_jTowersIDs_subset[thisRow][6] = towerid;
        tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

      }
      // set the EMEC part
      for(int thisRow=0; thisRow<rows; thisRow++){
	int towerid = initialEMEC + /*( (thisCol-8) * 64)*/ + thisRow;

	//if( (thisJFEX == 23) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap
	
	tmp_jTowersIDs_subset[thisRow][7/*thisCol*/] = towerid;
	tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));
	
	std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();
	
      }


      if(false){
	ATH_MSG_DEBUG("CONTENTS OF jFEX " << thisJFEX << " :");
	for (int thisRow=rows-1; thisRow>=0; thisRow--){
	  for (int thisCol=0; thisCol<cols; thisCol++){
	    int tmptowerid = tmp_jTowersIDs_subset[thisRow][thisCol];
	    const float tmptowereta = this_jTowerContainer->findTower(tmptowerid)->eta();
	    const float tmptowerphi = this_jTowerContainer->findTower(tmptowerid)->phi();
	    if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
	    else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
	  }
	}
      }

      //tool use instead
      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      fpgacounter++;

    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // A-SIDE POSITIVE JFEXs
    // RIGHT-MOST
    // DO THE RIGHT-MOST (POSITIVE ETA) JFEXs SIXTH
    fpgacounter = 0;
    //id_modifier + phi + (64 * eta)
    emecEta = 16; emecPhi = 0; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //601024;

    if(true){ // jFEX 5
      thisJFEX = 5;

      // decide which subset of towers (and therefore supercells) should go to the jFEX
      std::map<int,jTower> tmp_jTowersColl_subset;

      // let's try doing this with an array initially just containing tower IDs.
      int tmp_jTowersIDs_subset [16*4][9];

      int rows = sizeof tmp_jTowersIDs_subset / sizeof tmp_jTowersIDs_subset[0];
      int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];

      // set the EMEC part
      for(int thisCol=0; thisCol<9; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){

          int towerid = initialEMEC + (thisCol * 64) + thisRow;

          //if( (thisJFEX == 21) && (thisRow >= 7)){ towerid -= 64; }; // we'll bring this back in if need to apply FPGA or jFEX overlap

          tmp_jTowersIDs_subset[thisRow][thisCol] = towerid;
          tmp_jTowersColl_subset.insert( std::map<int, jTower>::value_type(towerid,  *(this_jTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_jTowerContainer->findTower(towerid)->getEMSCIDs();

        }
      }

      if(false){
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
      }

      m_jFEXSimTool->init(thisJFEX);
      ATH_CHECK(m_jFEXSimTool->NewExecute(tmp_jTowersIDs_subset));
      m_jFEXSimTool->reset();

      fpgacounter++;

    }
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

