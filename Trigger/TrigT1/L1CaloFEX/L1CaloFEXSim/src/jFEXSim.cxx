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

StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[16*4][9]){

  const int nrows = 16;
  const int ncols = 9;

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];

  ATH_CHECK( m_jFEXFPGATool.retrieve() );

  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<nrows; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(0, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows; myrow<nrows*2; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-nrows][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(1, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*2; myrow<nrows*3; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*2)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(2, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*3; myrow<nrows*4; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*3)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(3, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 3---------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}

StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[16*4][8]){

  //std::copy(&tmp_jTowersIDs_subset[0][0], &tmp_jTowersIDs_subset[0][0]+(10*18),&m_jTowersIDs[0][0]);

  const int nrows = 16;
  const int ncols = 8;

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];

  ATH_CHECK( m_jFEXFPGATool.retrieve() );
  
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = 0; myrow<nrows; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(0, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 0----------------------------------------------------------------------------------------------------------------------------------------------
  
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows; myrow<nrows*2; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-nrows][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(1, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 1----------------------------------------------------------------------------------------------------------------------------------------------


  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*2; myrow<nrows*3; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*2)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(2, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 2----------------------------------------------------------------------------------------------------------------------------------------------

  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------
  memset(tmp_jTowersIDs_subset_FPGA, 0, sizeof tmp_jTowersIDs_subset_FPGA);
  for (int myrow = nrows*3; myrow<nrows*4; myrow++){
    for (int mycol = 0; mycol<ncols; mycol++){
      tmp_jTowersIDs_subset_FPGA[myrow-(nrows*3)][mycol] = tmp_jTowersIDs_subset[myrow][mycol];
    }
  }
  ATH_CHECK(m_jFEXFPGATool->init(3, m_id));
  m_jFEXFPGATool->SetTowersAndCells_SG(tmp_jTowersIDs_subset_FPGA);
  ATH_CHECK(m_jFEXFPGATool->execute());
  m_jFEXFPGATool->reset();
  //FPGA 3----------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}

void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[16][8]){ // METHOD USING ONLY IDS

  const int nrows = 16;
  const int ncols = 8;

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

  void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[16][9]){ // METHOD USING ONLY IDS

    const int nrows = 16;
    const int ncols = 9;

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
  

} // end of namespace bracket

