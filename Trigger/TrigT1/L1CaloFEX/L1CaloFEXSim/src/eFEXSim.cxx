/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXSim  -  description
//                              -------------------
//     begin                : 12 07 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eFEXFPGA.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace LVL1 {

  eFEXSim::eFEXSim(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
  {
    declareInterface<IeFEXSim>(this);
  }


  //---------------- Initialisation -------------------------------------------------

  StatusCode eFEXSim::initialize()
  {
    return StatusCode::SUCCESS;
  }

  //---------------- Finalisation -------------------------------------------------

  StatusCode eFEXSim::finalize()
  {
    return StatusCode::SUCCESS;
  }


  void eFEXSim::reset()
  {

    int rows = sizeof m_eTowersIDs / sizeof m_eTowersIDs[0];
    int cols = sizeof m_eTowersIDs[0] / sizeof m_eTowersIDs[0][0];

    m_id = -1;
    m_eFEXFPGACollection.clear();
    for (int i=0; i<rows; i++){
      for (int j=0; j<cols; j++){
	  m_eTowersIDs[i][j] = 0;
	}
    }
    
  }

  void eFEXSim::init(int id)
  {
    m_id = id;
  }

  /** Destructor */
  eFEXSim::~eFEXSim()
  {
  }
  
 void eFEXSim::execute(){

 }

StatusCode eFEXSim::NewExecute(int tmp_eTowersIDs_subset[10][18]){
  m_tobWords.clear();

  std::copy(&tmp_eTowersIDs_subset[0][0], &tmp_eTowersIDs_subset[0][0]+(10*18),&m_eTowersIDs[0][0]);

  int tmp_eTowersIDs_subset_FPGA[10][6];

  ATH_CHECK( m_eFEXFPGATool.retrieve() );
  
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 0; mycol<6; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_eFEXFPGATool->init(0, m_id));
  m_eFEXFPGATool->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  ATH_CHECK(m_eFEXFPGATool->execute());
  m_tobWords.push_back(m_eFEXFPGATool->getEmTOBs());
  m_eFEXFPGATool->reset();
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 4; mycol<10; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-4] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_eFEXFPGATool->init(1, m_id));
  m_eFEXFPGATool->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  ATH_CHECK(m_eFEXFPGATool->execute());
  m_tobWords.push_back(m_eFEXFPGATool->getEmTOBs());
  m_eFEXFPGATool->reset();
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 8; mycol<14; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-8] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_eFEXFPGATool->init(2, m_id));
  m_eFEXFPGATool->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  ATH_CHECK(m_eFEXFPGATool->execute());
  m_tobWords.push_back(m_eFEXFPGATool->getEmTOBs());
  m_eFEXFPGATool->reset();
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 12; mycol<18; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-12] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_eFEXFPGATool->init(3, m_id));
  m_eFEXFPGATool->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  ATH_CHECK(m_eFEXFPGATool->execute());
  m_tobWords.push_back(m_eFEXFPGATool->getEmTOBs());
  m_eFEXFPGATool->reset();
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}


std::vector<uint32_t> eFEXSim::getEmTOBs()
{

  std::vector<uint32_t> tobsSort;
  tobsSort.clear();
  bool first = true;

  // concatonate tobs from the fpgas
  for(auto &j : m_tobWords){
    if (first) tobsSort = j;
    else tobsSort.insert(tobsSort.end(),j.begin(),j.end());
    first = false;
  }

  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in eFEX: " << m_id);

  // sort the tobs from the fpgas by their et (last 12 bits of 32 bit word)
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);

  /*
  for(auto &j : tobsSort){
    std::cout << "values: post sort " << std::bitset<32>(j) << std::endl;
  }
  */

  // return the tob 6 highest ET TOBs from the efex
  tobsSort.resize(6);
  return tobsSort;
}

void eFEXSim::SetTowersAndCells_SG(int tmp_eTowersIDs_subset[10][18]){ // METHOD USING ONLY IDS

  std::copy(&tmp_eTowersIDs_subset[0][0], &tmp_eTowersIDs_subset[0][0]+(10*18),&m_eTowersIDs[0][0]);
  
  int tmp_eTowersIDs_subset_FPGA[10][6];
  
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 0; mycol<6; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }  
  m_eFEXFPGACollection.at(0)->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 4; mycol<10; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-4] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  m_eFEXFPGACollection.at(1)->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  

  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 8; mycol<14; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-8] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  m_eFEXFPGACollection.at(2)->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_eTowersIDs_subset_FPGA, 0, sizeof tmp_eTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 12; mycol<18; mycol++){
      tmp_eTowersIDs_subset_FPGA[myrow][mycol-12] = tmp_eTowersIDs_subset[myrow][mycol];
    }
  }
  m_eFEXFPGACollection.at(3)->SetTowersAndCells_SG(tmp_eTowersIDs_subset_FPGA);
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  
}
  

} // end of namespace bracket

