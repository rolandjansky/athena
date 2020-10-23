/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXFPGA  -  description
//                              -------------------
//     begin                : 19 10 2020
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/
#include "L1CaloFEXSim/jFEXFPGA.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
//#include "L1CaloFEXSim/jFEXegAlgo.h" //for the future
//#include "L1CaloFEXSim/jFEXegTOB.h" //for the future
//#include "L1CaloFEXSim/jFEXOutputCollection.h" //for the future
//#include "L1CaloFEXSim/jFEXtauAlgo.h" //for the future
//#include "L1CaloFEXSim/jFEXtauTOB.h" //for the future
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include <vector>
#include "TH1F.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "SGTools/TestStore.h"


namespace LVL1 {

  // default constructor for persistency

jFEXFPGA::jFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  declareInterface<IjFEXFPGA>(this);
}
  
    
  /** Destructor */
  jFEXFPGA::~jFEXFPGA()
  {
  }

//================ Initialisation =================================================
  
StatusCode jFEXFPGA::initialize()
{

  ATH_CHECK(m_jFEXFPGA_jTowerContainerKey.initialize());
  //ATH_CHECK(m_jFEXFPGA_jFEXOutputCollectionKey.initialize());
  return StatusCode::SUCCESS;
}
  

StatusCode jFEXFPGA::init(int id, int jfexid)
{
  m_id = id;
  m_jfexid = jfexid;

  return StatusCode::SUCCESS;

}

void jFEXFPGA::reset(){

  m_id = -1;
  m_jfexid = -1;

}

StatusCode jFEXFPGA::execute(){

  SG::ReadHandle<jTowerContainer> jk_jFEXFPGA_jTowerContainer(m_jFEXFPGA_jTowerContainerKey/*,ctx*/);
  if(!jk_jFEXFPGA_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_jFEXFPGA_jTowerContainer " << m_jFEXFPGA_jTowerContainerKey.key() );
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;

}

void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][9]){
    
  const int rows = 16;
  const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
  
  std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Wide[0][0]);
  
  if(true){ //this prints out the jTower IDs that each FPGA is responsible for
    ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") IS RESPONSIBLE FOR eTOWERS :");
    for (int thisRow=rows-1; thisRow>=0; thisRow--){
      for (int thisCol=0; thisCol<cols; thisCol++){
	if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  "); }
	else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Wide[thisRow][thisCol] << "  |"); }
      }
    }
  }
  
}

  void jFEXFPGA::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[][8]){

    const int rows = 16;
    const int cols = sizeof tmp_jTowersIDs_subset[0] / sizeof tmp_jTowersIDs_subset[0][0];
    
    std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(rows*cols),&m_jTowersIDs_Thin[0][0]);

    if(false){ //this prints out the jTower IDs that each FPGA is responsible for
      ATH_MSG_DEBUG("\n==== jFEXFPGA ========= FPGA (" << m_id << ") IS RESPONSIBLE FOR eTOWERS :");
      for (int thisRow=rows-1; thisRow>=0; thisRow--){
	for (int thisCol=0; thisCol<cols; thisCol++){
	  if(thisCol != cols-1){ ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  "); }
	  else { ATH_MSG_DEBUG("|  " << m_jTowersIDs_Thin[thisRow][thisCol] << "  |"); }
	}
      }
    }

  }
  
} // end of namespace bracket

