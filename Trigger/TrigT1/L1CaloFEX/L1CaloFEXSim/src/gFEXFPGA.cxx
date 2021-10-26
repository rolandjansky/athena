/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXFPGA - Defines FPGA tools
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gFEXFPGA.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include <vector>


namespace LVL1 {

   // default constructor for persistency

  gFEXFPGA::gFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent):
     AthAlgTool(type,name,parent)
  {
     declareInterface<IgFEXFPGA>(this);
  }

     /** Destructor */
     gFEXFPGA::~gFEXFPGA()
     {
     }

  //---------------- Initialisation -------------------------------------------------

  StatusCode gFEXFPGA::initialize()
  {

     ATH_CHECK(m_gFEXFPGA_gTowerContainerKey.initialize());

     return StatusCode::SUCCESS;
  }


  StatusCode gFEXFPGA::init(int id)
  {
     m_fpgaId = id;

     return StatusCode::SUCCESS;

  }

  void gFEXFPGA::reset(){

     m_fpgaId = -1;

  }


  void gFEXFPGA::SetTowersAndCells_SG(gTowersCentral tmp_gTowersIDs_subset){

     int rows = tmp_gTowersIDs_subset.size();
     int cols = tmp_gTowersIDs_subset[0].size();

     std::copy(&tmp_gTowersIDs_subset[0][0], &tmp_gTowersIDs_subset[0][0]+(rows*cols),&m_gTowersIDs_central[0][0]);


  }

  void gFEXFPGA::SetTowersAndCells_SG(gTowersForward tmp_gTowersIDs_subset){

     int rows = tmp_gTowersIDs_subset.size();
     int cols = tmp_gTowersIDs_subset[0].size();

     std::copy(&tmp_gTowersIDs_subset[0][0], &tmp_gTowersIDs_subset[0][0]+(rows*cols),&m_gTowersIDs_forward[0][0]);

   }


   void gFEXFPGA::GetEnergyMatrix(gTowersCentral & output_gTower_energies) const{

      int rows = output_gTower_energies.size();
      int cols = output_gTower_energies[0].size();

      SG::ReadHandle<gTowerContainer> jk_gFEXFPGA_gTowerContainer(m_gFEXFPGA_gTowerContainerKey/*,ctx*/);

      for (int myrow = 0; myrow<rows; myrow++){
         for (int mycol = 0; mycol<cols; mycol++){
            output_gTower_energies[myrow][mycol] = 0;
            if (m_gTowersIDs_central[myrow][mycol] == 0) continue;
            const LVL1::gTower * tmpTower = jk_gFEXFPGA_gTowerContainer->findTower(m_gTowersIDs_central[myrow][mycol]);
            if (tmpTower == nullptr) continue;
            output_gTower_energies[myrow][mycol] = tmpTower->getET();

         }
      }
   }

   void gFEXFPGA::GetEnergyMatrix(gTowersForward & output_gTower_energies) const{

      int rows = output_gTower_energies.size();
      int cols = output_gTower_energies[0].size();

      SG::ReadHandle<gTowerContainer> jk_gFEXFPGA_gTowerContainer(m_gFEXFPGA_gTowerContainerKey/*,ctx*/);

      for (int myrow = 0; myrow<rows; myrow++){
         for (int mycol = 0; mycol<cols; mycol++){
            output_gTower_energies[myrow][mycol] = 0;
            if (m_gTowersIDs_forward[myrow][mycol] == 0) continue;
            const LVL1::gTower * tmpTower = jk_gFEXFPGA_gTowerContainer->findTower(m_gTowersIDs_forward[myrow][mycol]);
            if (tmpTower == nullptr) continue;
            output_gTower_energies[myrow][mycol] = tmpTower->getET();

         }
      }
   }


} // end of namespace bracket