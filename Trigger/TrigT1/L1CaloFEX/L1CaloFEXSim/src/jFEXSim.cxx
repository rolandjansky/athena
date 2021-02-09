/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
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


  StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[2*FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]){
  m_smallRJet_tobWords.clear();
  m_largeRJet_tobWords.clear();

  const int nrows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int ncols = FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;;

  // So the way this works now is a little awkward due to the forward regions, which have less cells in phi and also overlap a lot.
  // Interesting that I put nrows = 16*2 here, the documentation (l1caloreqs) looks like it should infact be just 16 (based on Fig 27)
  // But actually the overlap is I suppose much greater, probably covering a much larger region, in this case 16/2
  // Yes, this is answered by the phew lines just below which specify the exact ranges
  // Back to the forward regions, which have less cells in phi.  We have 2 decisions, we can either fill half the cells in phi, or fill every other cell in phi
  // For now I will fill every other cell in phi, but realistically the algorithm in the forward region needs to be written in a very different way
  // and the re-writing on that algorithm will likely impact on how it is written in the central regions too
  // The new writing of the algorithm will need to use the information from the central point of each tower in the array to check if it falls within a certain radius
  // it cannot rely on the structure of the array itself at all.  As such this algorithm is likely to end up being much less efficient that the eFEX equivalent.

  int tmp_jTowersIDs_subset_FPGA[nrows][ncols];
  
  // FPGA boundaries in phi: 0.0, 1.6, 3.2, 4.8, 6.4
  // Written explicitly:
  // 5.6 -> 2.4 [core is 0.0 to 1.6] // FPGA 0
  // 0.8 -> 4.0 [core is 1.6 to 3.2] // FPGA 1
  // 2.4 -> 5.6 [core is 3.2 to 4.8] // FPGA 2
  // 4.0 -> 0.8 [core is 4.8 to 6.4] // FPGA 3

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
  m_smallRJet_tobWords.push_back(m_jFEXFPGATool->getSmallRJetTOBs());
  m_largeRJet_tobWords.push_back(m_jFEXFPGATool->getLargeRJetTOBs());
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
  m_smallRJet_tobWords.push_back(m_jFEXFPGATool->getSmallRJetTOBs());
  m_largeRJet_tobWords.push_back(m_jFEXFPGATool->getLargeRJetTOBs()); 
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
  m_smallRJet_tobWords.push_back(m_jFEXFPGATool->getSmallRJetTOBs());
  m_largeRJet_tobWords.push_back(m_jFEXFPGATool->getLargeRJetTOBs());
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
  m_smallRJet_tobWords.push_back(m_jFEXFPGATool->getSmallRJetTOBs());
  m_largeRJet_tobWords.push_back(m_jFEXFPGATool->getLargeRJetTOBs());
  m_jFEXFPGATool->reset();
  //FPGA 3---------------------------------------------------------------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;

}

StatusCode jFEXSim::NewExecute(int tmp_jTowersIDs_subset[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]){


  const int nrows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int ncols = FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;

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

void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]){ // METHOD USING ONLY IDS

  const int nrows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int ncols = FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;; 
  // We may need a function along the lines of "SetForwardTowersAndCells_SG"(int tmp_jTowerIDs_subset[16][28]

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
  
  for(auto &j : m_smallRJet_tobWords){
    if (first) tobsSort = j;
    else tobsSort.insert(tobsSort.end(),j.begin(), j.end());
    first = false;
  } 
  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in jFEX: " << m_id);
 //sort the tobs from the fpgas by their et
  std::sort (tobsSort.begin(), tobsSort.end(), smallRJet_ET_Sort);
  tobsSort.resize(7);
  return tobsSort;
}

std::vector<uint32_t> jFEXSim::getLargeRJetTOBs()
{
  std::vector<uint32_t> tobsSort;
  tobsSort.clear();
  bool first = true;

  for(auto &j : m_largeRJet_tobWords){
    if (first) tobsSort = j;
    else tobsSort.insert(tobsSort.end(),j.begin(), j.end());
    first = false;
  }
  ATH_MSG_DEBUG("number of tobs: " <<tobsSort.size() << " in jFEX: " << m_id);
//sort the tobs from the fpgas by their et
  std::sort (tobsSort.begin(), tobsSort.end(), largeRJet_ET_Sort);
  tobsSort.resize(1);
  return tobsSort;
}




void jFEXSim::SetTowersAndCells_SG(int tmp_jTowersIDs_subset[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]){ // METHOD USING ONLY IDS

  const int nrows = FEXAlgoSpaceDefs::jFEX_algoSpace_height;
  const int ncols = FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;

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

