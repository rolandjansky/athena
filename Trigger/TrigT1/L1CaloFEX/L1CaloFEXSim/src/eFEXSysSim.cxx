/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXSysSim  -  description
//                              -------------------
//     begin                : 12 07 2019
//     email                : jacob.julian.kempster@cern.ch alison.elliot@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/eFEXSysSim.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ServiceHandle.h"


#include <ctime>

namespace LVL1 {
  
  
  // default constructor for persistency

  eFEXSysSim::eFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
  {
    declareInterface<IeFEXSysSim>(this);

  }

    
  /** Destructor */
  //eFEXSysSim::~eFEXSysSim()
  //{
  //}

  //================ Initialisation =================================================

  StatusCode eFEXSysSim::initialize()
  {
    
    ATH_CHECK(m_eTowerContainerSGKey.initialize());

    ATH_CHECK( m_eFEXSimTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  //================ Finalisation =================================================

  StatusCode eFEXSysSim::finalize()
  {
    return StatusCode::SUCCESS;
  }

  
  void eFEXSysSim::init()  {

  }

  void eFEXSysSim::cleanup()  {

    m_eFEXCollection.clear();
    m_eTowersColl.clear();

  }


  int eFEXSysSim::calcTowerID(int eta, int phi, int mod)  {

    return ((64*eta) + phi + mod);

  }

  StatusCode eFEXSysSim::execute()  {    

    SG::ReadHandle<LVL1::eTowerContainer> this_eTowerContainer(m_eTowerContainerSGKey/*,ctx*/);
    if(!this_eTowerContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve eTowerContainer " << m_eTowerContainerSGKey.key());
      return StatusCode::FAILURE;
    }

    // We need to split the towers into 3 blocks in eta and 8 blocks in phi.

    // boundaries in eta: -2.5, -0.8, 0.8, 2.5
    // REAL boundaries in eta (overlaps must occur for sliding window algorithms!): -2.5, -0.7, -0.9, 0.9, 0.7, 2.5
    // Written explicitly:
    // -2.5 -> -0.7
    // -0.9 -> 0.9
    // 0.7 -> 2.5

    // boundaries in phi: 0.2, 1.0, 1.8, 2.6, 3.4, 4.2, 5.0, 5.8
    // Written explicityl with REAL boundaries in eta (overlaps must occur for sliding window algorithms!)
    // 0.1 -> 1.1
    // 0.9 -> 1.9
    // 1.7 -> 2.7
    // 2.5 -> 3.5
    // 3.3 -> 4.3
    // 4.1 -> 5.1
    // 4.9 -> 5.9
    // 5.7 -> 0.3

    // C-SIDE NEGATIVE EFEXs
    // DO THE LEFT-MOST (NEGATIVE ETA) EFEXs FIRST
    int fexcounter = 0;
    // Example values for eFEX 0
    //id_modifier + phi + (64 * eta)
    int emecEta = 24; int emecPhi = 1; int emecMod = 500000;
    int initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //501537;
    int transEta = 14; int transPhi = 1; int transMod = 300000;
    int initialTRANS = calcTowerID(transEta,transPhi,transMod); //300897;
    int embEta = 13; int embPhi = 1; int embMod = 100000;
    int initialEMB = calcTowerID(embEta,embPhi,embMod); //100833;

    for (int thisEFEX=0; thisEFEX<=21; thisEFEX+=3){
      
      if(fexcounter > 0){ initialEMEC += 8; initialTRANS += 8; initialEMB += 8; } // TODO // SOMEHOW REMOVE HARD-CODING?

      // decide which subset of towers (and therefore supercells) should go to the eFEX
      std::map<int,eTower> tmp_eTowersColl_subset;
      
      // let's try doing this with an array initially just containing tower IDs.
      int tmp_eTowersIDs_subset [10][18];
      
      int rows = sizeof tmp_eTowersIDs_subset / sizeof tmp_eTowersIDs_subset[0];
      int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];

      // set the EMEC part
      for(int thisCol=0; thisCol<10; thisCol++){
	for(int thisRow=0; thisRow<rows; thisRow++){

	  int towerid = initialEMEC - (thisCol * 64) + thisRow;

	  if( (thisEFEX == 21) && (thisRow >= 7)){ towerid -= 64; };
	  
	  tmp_eTowersIDs_subset[thisRow][thisCol] = towerid;
	  tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

	}
      }

      // set the TRANS part
      for(int thisRow = 0; thisRow < rows; thisRow++){

	int towerid = initialTRANS + thisRow;

	if( (thisEFEX == 21) && (thisRow >= 7)){ towerid -= 64; };

	tmp_eTowersIDs_subset[thisRow][10] = towerid;
	tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

      }

      // set the EMB part
      for(int thisCol = 11; thisCol < cols; thisCol++){
	for(int thisRow=0; thisRow<rows; thisRow++){

	  int towerid = initialEMB - ( (thisCol-11) * 64) + thisRow;

	  if( (thisEFEX == 21) && (thisRow >= 7)){ towerid -= 64; };

	  tmp_eTowersIDs_subset[thisRow][thisCol] = towerid;
	  tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

	}
      }
      

      if(false){
	ATH_MSG_DEBUG("CONTENTS OF eFEX " << thisEFEX << " :");
        for (int thisRow=rows-1; thisRow>=0; thisRow--){
          for (int thisCol=0; thisCol<cols; thisCol++){
            int tmptowerid = tmp_eTowersIDs_subset[thisRow][thisCol];
            const float tmptowereta = this_eTowerContainer->findTower(tmptowerid)->eta();
            const float tmptowerphi = this_eTowerContainer->findTower(tmptowerid)->phi();
            if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])  "); }
            else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
          }
        }
      }


      m_eFEXSimTool->init(thisEFEX);
      ATH_CHECK(m_eFEXSimTool->NewExecute(tmp_eTowersIDs_subset));
      m_eFEXSimTool->reset();

      fexcounter++;

    }

    // CENTRAL EFEXs
    // DO THE CENTRAL EFEXs SECOND
    fexcounter = 0;
    int embnegEta = 8; int embnegPhi = 1; int embnegMod = 100000;
    int initialEMB_neg = calcTowerID(embnegEta,embnegPhi,embnegMod); //100513;
    int embposEta = 0; int embposPhi = 1; int embposMod = 200000;
    int initialEMB_pos = calcTowerID(embposEta,embposPhi,embposMod); //200001;

    for (int thisEFEX=1; thisEFEX<=22; thisEFEX+=3){

      if(fexcounter > 0){  initialEMB_neg += 8; initialEMB_pos += 8; }
      
      // decide which subset of towers (and therefore supercells) should go to the eFEX
      std::map<int,eTower> tmp_eTowersColl_subset;
      
      // doing this with an array initially just containing tower IDs.
      int tmp_eTowersIDs_subset [10][18];

      int rows = sizeof tmp_eTowersIDs_subset / sizeof tmp_eTowersIDs_subset[0];
      int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];

      // set the EMB part
      for(int thisCol = 0; thisCol < cols; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
	  int towerid = -1;

	  int tmp_initEMB = initialEMB_neg;

	  if(thisCol < 9){
	    towerid = tmp_initEMB - ( (thisCol) * 64) + thisRow;
	  }
	  else{
	    tmp_initEMB = initialEMB_pos;
	    towerid = tmp_initEMB + ( (thisCol-9) * 64) + thisRow;
	  }

	  if( (thisEFEX == 22) && (thisRow >= 7)){ towerid -= 64; };

          tmp_eTowersIDs_subset[thisRow][thisCol] = towerid;

          tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

        }
      }
      

      if(false){
	ATH_MSG_DEBUG("CONTENTS OF eFEX " << thisEFEX << " :");
        for (int thisRow=rows-1; thisRow>=0; thisRow--){
          for (int thisCol=0; thisCol<cols; thisCol++){
            int tmptowerid = tmp_eTowersIDs_subset[thisRow][thisCol];
            const float tmptowereta = this_eTowerContainer->findTower(tmptowerid)->eta();
            const float tmptowerphi = this_eTowerContainer->findTower(tmptowerid)->phi();
            if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
            else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
          }
        }
      }

      //tool use instead
      m_eFEXSimTool->init(thisEFEX);
      ATH_CHECK(m_eFEXSimTool->NewExecute(tmp_eTowersIDs_subset));
      m_eFEXSimTool->reset();

      fexcounter++;

    }

    // POSITIVE EFEXs
    // LET'S DO THE RIGHT-MOST (POSTITIVE ETA) EFEXs THIRD
    fexcounter = 0;
    // Example values for eFEX 0
    emecEta = 15; emecPhi = 1; emecMod = 600000;
    initialEMEC = calcTowerID(emecEta,emecPhi,emecMod); //600961;
    transEta = 14; transPhi = 1; transMod = 400000;
    initialTRANS = calcTowerID(transEta,transPhi,transMod); //400897;
    embEta = 7; embPhi = 1; embMod = 200000;
    initialEMB = calcTowerID(embEta,embPhi,embMod); //200449;

    for (int thisEFEX=2; thisEFEX<=23; thisEFEX+=3){

      if(fexcounter > 0){ initialEMEC += 8; initialTRANS += 8; initialEMB += 8; }

      // decide which subset of towers (and therefore supercells) should go to the eFEX
      std::map<int,eTower> tmp_eTowersColl_subset;

      // doing this with an array initially just containing tower IDs.
      int tmp_eTowersIDs_subset [10][18];

      int rows = sizeof tmp_eTowersIDs_subset / sizeof tmp_eTowersIDs_subset[0];
      int cols = sizeof tmp_eTowersIDs_subset[0] / sizeof tmp_eTowersIDs_subset[0][0];

      // set the EMB part
      for(int thisCol = 0; thisCol < 7; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
          int towerid = initialEMB + ( (thisCol) * 64) + thisRow;

          if( (thisEFEX == 23) && (thisRow >= 7)){ towerid -= 64; };

          tmp_eTowersIDs_subset[thisRow][thisCol] = towerid;
          tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

        }
      }
      // set the TRANS part
      for(int thisRow = 0; thisRow < rows; thisRow++){
        int towerid = initialTRANS + thisRow;

        if( (thisEFEX == 23) && (thisRow >= 7)){ towerid -= 64; };

        tmp_eTowersIDs_subset[thisRow][7] = towerid;
        tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

      }
      // set the EMEC part
      for(int thisCol=8; thisCol<cols; thisCol++){
        for(int thisRow=0; thisRow<rows; thisRow++){
          int towerid = initialEMEC + ( (thisCol-8) * 64) + thisRow;

          if( (thisEFEX == 23) && (thisRow >= 7)){ towerid -= 64; };

          tmp_eTowersIDs_subset[thisRow][thisCol] = towerid;
          tmp_eTowersColl_subset.insert( std::map<int, eTower>::value_type(towerid,  *(this_eTowerContainer->findTower(towerid))));

	  std::vector<Identifier> supercellIDs = this_eTowerContainer->findTower(towerid)->getSCIDs();

        }
      }


      if(false){
	ATH_MSG_DEBUG("CONTENTS OF eFEX " << thisEFEX << " :");
	for (int thisRow=rows-1; thisRow>=0; thisRow--){
	  for (int thisCol=0; thisCol<cols; thisCol++){
	    int tmptowerid = tmp_eTowersIDs_subset[thisRow][thisCol];
	    const float tmptowereta = this_eTowerContainer->findTower(tmptowerid)->eta();
	    const float tmptowerphi = this_eTowerContainer->findTower(tmptowerid)->phi();
	    if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  "); }
	    else { ATH_MSG_DEBUG("|  " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])  |"); }
	  }
	}
      }

      //tool use instead
      m_eFEXSimTool->init(thisEFEX);
      ATH_CHECK(m_eFEXSimTool->NewExecute(tmp_eTowersIDs_subset));
      m_eFEXSimTool->reset();

      fexcounter++;

    }
    
    //Collate TOBS returned from eFEXSims. Should that be here?
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

