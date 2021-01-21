/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXSim  -  description
//                              -------------------
//     begin                : 19 10 2020
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#include "L1CaloFEXSim/jFEXSim.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jFEXFPGA.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace LVL1 {

  jFEXSim::jFEXSim(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
  {
    declareInterface<IjFEXSim>(this);
  }


  //================ Initialisation =================================================

  StatusCode jFEXSim::initialize()
  {
    return StatusCode::SUCCESS;
  }

  //================ Finalisation =================================================

  StatusCode jFEXSim::finalize()
  {
    return StatusCode::SUCCESS;
  }


  void jFEXSim::reset()
  {

    m_id = -1;
    m_jFEXFPGACollection.clear();

    int rows = sizeof m_jTowersIDs_Thin / sizeof m_jTowersIDs_Thin[0];
    int cols = sizeof m_jTowersIDs_Thin[0] / sizeof m_jTowersIDs_Thin[0][0];
    for (int i=0; i<rows; i++){
      for (int j=0; j<cols; j++){
	  m_jTowersIDs_Thin[i][j] = 0;
	}
    }


    rows = sizeof m_jTowersIDs_Wide / sizeof m_jTowersIDs_Wide[0];
    cols = sizeof m_jTowersIDs_Wide[0] / sizeof m_jTowersIDs_Wide[0][0];
    for (int i=0; i<rows; i++){
      for (int j=0; j<cols; j++){
	m_jTowersIDs_Wide[i][j] = 0;
      }
    }
    
  }

  void jFEXSim::init(int id)
  {
    m_id = id;
  }

  /** Destructor */
  jFEXSim::~jFEXSim()
  {
  }
  
 void jFEXSim::execute(){

 }

StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[16*4][17]){
  m_tobWords.clear();
  const int nrows = 16*2;
  const int ncols = 17;

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];
  
  // FPGA boundaries in phi: 0.0, 1.6, 3.2, 4.8, 6.4
  // Written explicitly:
  // 5.6 -> 2.4 [core is 0.0 to 1.6]
  // 0.8 -> 4.0 [core is 1.6 to 3.2]
  // 2.4 -> 5.6 [core is 3.2 to 4.8]
  // 4.0 -> 0.8 [core is 4.8 to 6.4]

  ATH_CHECK( m_jFEXFPGATool.retrieve() );

  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 5.6 -> 2.4 [core is 0.0 to 1.6]
  for (int myrow = 0; myrow<8; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[56+myrow][mycol]; // fills target rows 0-7 with source rows 56-63
    }
  }
  for (int myrow = 8; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow-8][mycol]; // fills target rows 8-31 with source rows 0-23
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(0, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_tobWords.push_back(m_jFEXFPGATool->getjTOBs());
  m_jFEXFPGATool->reset();
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 0.8 -> 4.0 [core is 1.6 to 3.2]
  for (int myrow = 0; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[8+myrow][mycol]; // fills target rows 0-31 with source rows 8-39
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(1, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_tobWords.push_back(m_jFEXFPGATool->getjTOBs());
  m_jFEXFPGATool->reset();
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 2.4 -> 5.6 [core is 3.2 to 4.8]
  for (int myrow = 0; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[24+myrow][mycol]; // fills target rows 0-31 with source rows 24-55 
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(2, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_tobWords.push_back(m_jFEXFPGATool->getjTOBs());
  m_jFEXFPGATool->reset();
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 4.0 -> 0.8 [core is 4.8 to 6.4]
  for (int myrow = 0; myrow<24; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[40+myrow][mycol]; // fills target rows 0-23 with source rows 40-63
    }
  }
  for (int myrow = 24; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow-24][mycol]; // fills target rows 24-31 with source rows 0-8
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(3, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_tobWords.push_back(m_jFEXFPGATool->getjTOBs());
  m_jFEXFPGATool->reset();
  //FPGA 3---------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}

StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[16*4][24]){

  //std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(10*18),&m_jTowersIDs[0][0]);

  const int nrows = 16*2;
  const int ncols = 24;

  // FPGA boundaries in phi: 0.0, 1.6, 3.2, 4.8, 6.4
  // Written explicitly:
  // 5.6 -> 2.4 [core is 0.0 to 1.6]
  // 0.8 -> 4.0 [core is 1.6 to 3.2]
  // 2.4 -> 5.6 [core is 3.2 to 4.8]
  // 4.0 -> 0.8 [core is 4.8 to 6.4]

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];

  ATH_CHECK( m_jFEXFPGATool.retrieve() );
  
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 5.6 -> 2.4 [core is 0.0 to 1.6]
  for (int myrow = 0; myrow<8; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[56+myrow][mycol]; // fills target rows 0-7 with source rows 56-63
    }
  }
  for (int myrow = 8; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow-8][mycol]; // fills target rows 8-31 with source rows 0-23
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(0, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 0.8 -> 4.0 [core is 1.6 to 3.2]
  for (int myrow = 0; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[8+myrow][mycol]; // fills target rows 0-31 with source rows 8-39
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(1, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 2.4 -> 5.6 [core is 3.2 to 4.8]
  for (int myrow = 0; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[24+myrow][mycol]; // fills target rows 0-31 with source rows 24-55
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(2, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  // 4.0 -> 0.8 [core is 4.8 to 6.4]
  for (int myrow = 0; myrow<24; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[40+myrow][mycol]; // fills target rows 0-23 with source rows 40-63
    }
  }
  for (int myrow = 24; myrow<32; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow-24][mycol]; // fills target rows 24-31 with source rows 0-8
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(3, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}

void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[16*2][17]){ // METHOD USING ONLY IDS

  const int nrows = 16*2;
  const int ncols = 17;

  std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(nrows*ncols),&m_jTowersIDs_Wide[0][0]);

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];
  
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<nrows; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }  
  m_jFEXFPGACollection.at(0)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows; myrow<nrows*2; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-nrows][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  m_jFEXFPGACollection.at(1)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  

  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*2; myrow<nrows*3; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*2)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  m_jFEXFPGACollection.at(2)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*3; myrow<nrows*4; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*3)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  m_jFEXFPGACollection.at(3)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  
}

std::vector<uint32_t> jFEXSim::getSmallRJetTOBs()
{
  std::vector<uint32_t> tobsSort;
  tobsSort.clear();
  bool first = true;
  
  for(auto &j : m_tobWords){
    if (first) tobsSort = j;
    else tobsSort.insert(tobsSort.end(),j.begin(), j.end());
    first = false;
  } 
  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in jFEX: " << m_id);
  std::sort (tobsSort.begin(), tobsSort.end(), etSort);
  tobsSort.resize(7);
  return tobsSort;
}





  void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[16*2][24]){ // METHOD USING ONLY IDS

    const int nrows = 16*2;
    const int ncols = 24;

    std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(nrows*ncols),&m_jTowersIDs_Thin[0][0]);

    int tmp_jTowersIDs_subset_FPGA[nrows][ncols];

    //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
    memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
    for (int myrow = 0; myrow<nrows; myrow++){
      for (int mycol = 0; mycol<ncols; mycol++){
	tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
      }
    }
    m_jFEXFPGACollection.at(0)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
    //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------

    //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
    memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
    for (int myrow = nrows; myrow<nrows*2; myrow++){
      for (int mycol = 0; mycol<ncols; mycol++){
	tmp_jTowersIDs_subset_FPGA[myrow-nrows][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
      }
    }
    m_jFEXFPGACollection.at(1)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
    //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


    //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
    memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
    for (int myrow = nrows*2; myrow<nrows*3; myrow++){
      for (int mycol = 0; mycol<ncols; mycol++){
	tmp_jTowersIDs_subset_FPGA[myrow-(nrows*2)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
      }
    }
    m_jFEXFPGACollection.at(2)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
    //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

    //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
    memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
    for (int myrow = nrows*3; myrow<nrows*4; myrow++){
      for (int mycol = 0; mycol<ncols; mycol++){
	tmp_jTowersIDs_subset_FPGA[myrow-(nrows*3)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
      }
    }
    m_jFEXFPGACollection.at(3)->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
    //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------

  }
  

} // end of namespace bracket

