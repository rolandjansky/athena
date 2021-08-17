/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXSim - Simulation of the gFEX module
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gFEXSim.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gFEXFPGA.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace LVL1 {

   gFEXSim::gFEXSim(const std::string& type,const std::string& name,const IInterface* parent):
      AthAlgTool(type,name,parent)
   {
      declareInterface<IgFEXSim>(this);
   }


   void gFEXSim::reset()
   {
      int rows = m_gTowersIDs.size();
      int cols = m_gTowersIDs[0].size();

      for (int i=0; i<rows; i++){
         for (int j=0; j<cols; j++){
            m_gTowersIDs[i][j] = 0;
         }
      }

   }

   /** Destructor */
   gFEXSim::~gFEXSim(){
   }

 void gFEXSim::execute(){

 }

StatusCode gFEXSim::executegFEXSim(gTowersIDs tmp_gTowersIDs_subset){

   typedef  std::array<std::array<int, 12>, 32> gTowersCentral;
   typedef  std::array<std::array<int, 7>, 32> gTowersForward;

   int rows = tmp_gTowersIDs_subset.size();
   int cols = tmp_gTowersIDs_subset[0].size();

   std::copy(&tmp_gTowersIDs_subset[0][0], &tmp_gTowersIDs_subset[0][0]+(rows*cols),&m_gTowersIDs[0][0]);

   gTowersCentral Atwr = {{{0}}};
   gTowersCentral Btwr = {{{0}}};
   gTowersForward CPtwr = {{{0}}};
   gTowersForward CNtwr = {{{0}}};


   ATH_CHECK( m_gFEXFPGA_Tool.retrieve() );

   //FPGA A----------------------------------------------------------------------------------------------------------------------------------------------
   gTowersCentral tmp_gTowersIDs_subset_centralFPGA;
   memset(&tmp_gTowersIDs_subset_centralFPGA, 0, sizeof tmp_gTowersIDs_subset_centralFPGA);
   for (int myrow = 0; myrow<32; myrow++){
      for (int mycol = 0; mycol<12; mycol++){
         tmp_gTowersIDs_subset_centralFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+7];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(0));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_centralFPGA);
   m_gFEXFPGA_Tool->GetEnergyMatrix(Atwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA A----------------------------------------------------------------------------------------------------------------------------------------------

   //FPGA B----------------------------------------------------------------------------------------------------------------------------------------------
   gTowersCentral tmp_gTowersIDs_subset_centralFPGA_B;
   memset(&tmp_gTowersIDs_subset_centralFPGA_B, 0, sizeof tmp_gTowersIDs_subset_centralFPGA_B);
   for (int myrow = 0; myrow<32; myrow++){
      for (int mycol = 0; mycol<12; mycol++){
         tmp_gTowersIDs_subset_centralFPGA_B[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+19];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(1));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_centralFPGA_B);
   m_gFEXFPGA_Tool->GetEnergyMatrix(Btwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA B----------------------------------------------------------------------------------------------------------------------------------------------


   //FPGA C-P ----------------------------------------------------------------------------------------------------------------------------------------------
   //Use a matrix with 32 rows, even if FPGA-C (positive) mostly deals with regions of 16 bins in phi.
   //However, one small region in eta (2.5<|eta|<2.8) has 32 bins in phi. So we use a matrix 32x14
   //but we fill half of it in the region 2.8<|eta|<4.8.
   // int tmp_gTowersIDs_subset_forwardFPGA[32][7];
   gTowersForward tmp_gTowersIDs_subset_forwardFPGA;
   memset(&tmp_gTowersIDs_subset_forwardFPGA, 0, sizeof tmp_gTowersIDs_subset_forwardFPGA);
   for (int myrow = 0; myrow<32; myrow++){
      for (int mycol = 0; mycol<1; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+31];
      }
   }
   for (int myrow = 0; myrow<16; myrow++){
      for (int mycol = 1; mycol<7; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+31];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(2));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_forwardFPGA);
   m_gFEXFPGA_Tool->GetEnergyMatrix(CPtwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA C-P----------------------------------------------------------------------------------------------------------------------------------------------


   //FPGA C-N----------------------------------------------------------------------------------------------------------------------------------------------
   //Use a matrix with 32 rows, even if FPGA-C (negative) mostly deals with regions of 16 bins in phi.
   //However, one small region in eta (2.5<|eta|<2.8) has 32 bins in phi. So we use a matrix 32x14
   //but we fill half of it in the region 2.8<|eta|<4.8.
   // int tmp_gTowersIDs_subset_forwardFPGA[32][7];
   gTowersForward tmp_gTowersIDs_subset_forwardFPGA_N;
   memset(&tmp_gTowersIDs_subset_forwardFPGA_N, 0, sizeof tmp_gTowersIDs_subset_forwardFPGA_N);
   for (int myrow = 0; myrow<16; myrow++){
      for (int mycol = 0; mycol<6; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA_N[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol];
      }
   }
   for (int myrow = 0; myrow<32; myrow++){
      for (int mycol = 6; mycol<7; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA_N[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(3));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_forwardFPGA);
   m_gFEXFPGA_Tool->GetEnergyMatrix(CNtwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA C-N----------------------------------------------------------------------------------------------------------------------------------------------

   int pucA = 0;
   int pucB = 0;
   int seedThreshold = 0;
   int jetThreshold = 0;

   // The output TOBs, to be filled by the gFEXJetAlgoTool
   std::array<uint32_t, 7> ATOB1_dat = {0};
   std::array<uint32_t, 7> ATOB2_dat = {0};
   std::array<uint32_t, 7> BTOB1_dat = {0};
   std::array<uint32_t, 7> BTOB2_dat = {0};

   // Retrieve the gFEXJetAlgoTool
   ATH_CHECK( m_gFEXJetAlgoTool.retrieve() );

   // Pass the energy matrices to the algo tool, and run the algorithms
   auto tobs_v = m_gFEXJetAlgoTool->largeRfinder(Atwr, Btwr, CNtwr, CPtwr,
                                                 pucA, pucB, seedThreshold, jetThreshold,
                                                 ATOB1_dat, ATOB2_dat,
                                                 BTOB1_dat, BTOB2_dat);

   m_gRhoTobWords.resize(2);
   m_gBlockTobWords.resize(8);
   m_gJetTobWords.resize(4);

   m_gRhoTobWords[0] = ATOB2_dat[0];//Pile up correction A
   m_gRhoTobWords[1] = BTOB2_dat[0];//Pile up correction B

   //Placing the gBlock TOBs into a dedicated array
   m_gBlockTobWords[0] = ATOB1_dat[1];//leading gBlock in FPGA A, eta bins (0--5)
   m_gBlockTobWords[1] = ATOB2_dat[1];//leading gBlock in FPGA A, eta bins (6--11)
   m_gBlockTobWords[2] = BTOB1_dat[1];//leading gBlock in FPGA B, eta bins (0--5)
   m_gBlockTobWords[3] = BTOB2_dat[1];//leading gBlock in FPGA B, eta bins (6--11)

   m_gBlockTobWords[4] = ATOB1_dat[2];//subleading gBlock in FPGA A, eta bins (0--5)
   m_gBlockTobWords[5] = ATOB2_dat[2];//subleading gBlock in FPGA A, eta bins (6--11)
   m_gBlockTobWords[6] = BTOB1_dat[2];//subleading gBlock in FPGA B, eta bins (0--5)
   m_gBlockTobWords[7] = BTOB2_dat[2];//subleading gBlock in FPGA B, eta bins (6--11)

   //Placing the gJet TOBs into a dedicated array
   m_gJetTobWords[0] = ATOB1_dat[3];//leading gJet in FPGA A, eta bins (0--5)
   m_gJetTobWords[1] = ATOB2_dat[3];//leading gJet in FPGA A, eta bins (6--11)
   m_gJetTobWords[2] = BTOB1_dat[3];//leading gJet in FPGA B, eta bins (0--5)
   m_gJetTobWords[3] = BTOB2_dat[3];//leading gJet in FPGA B, eta bins (6--11)

   gFEXOutputCollection* gFEXOutputs;
   ATH_CHECK(evtStore()->retrieve(gFEXOutputs, "gFEXOutputCollection"));


   for (int i = 0; i <14; i++){
     gFEXOutputs->addJetTob(tobs_v[i]->getWord());
     gFEXOutputs->addValueJet("EtaJet", tobs_v[i]->getEta());
     gFEXOutputs->addValueJet("PhiJet", tobs_v[i]->getPhi());
     gFEXOutputs->addValueJet("ETJet", tobs_v[i]->getET());
     gFEXOutputs->addValueJet("StatusJet", tobs_v[i]->getStatus());
     gFEXOutputs->addValueJet("TobIDJet", tobs_v[i]->getTobID());
     gFEXOutputs->fillJet();

   }

    return StatusCode::SUCCESS;

}

std::vector<uint32_t> gFEXSim::getgRhoTOBs() const
{
  return m_gRhoTobWords;
}

std::vector<uint32_t> gFEXSim::getgBlockTOBs() const
{
  return m_gBlockTobWords;
}

std::vector<uint32_t> gFEXSim::getgJetTOBs() const
{
  return m_gJetTobWords;
}


} // end of namespace bracket
