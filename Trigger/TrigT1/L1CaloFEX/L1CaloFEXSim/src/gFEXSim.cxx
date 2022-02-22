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

   StatusCode gFEXSim::initialize(){
      ATH_CHECK( m_gFEXFPGA_Tool.retrieve() );
      ATH_CHECK( m_gFEXJetAlgoTool.retrieve() );
      ATH_CHECK( m_gFEXJwoJAlgoTool.retrieve() );
      ATH_CHECK(m_l1MenuKey.initialize());
      return StatusCode::SUCCESS;
   }

 void gFEXSim::execute(){

 }

StatusCode gFEXSim::executegFEXSim(gTowersIDs tmp_gTowersIDs_subset, gFEXOutputCollection* gFEXOutputs){

   typedef  std::array<std::array<int, FEXAlgoSpaceDefs::centralNeta>, FEXAlgoSpaceDefs::centralNphi> gTowersCentral;
   typedef  std::array<std::array<int, FEXAlgoSpaceDefs::forwardNeta>, FEXAlgoSpaceDefs::centralNphi> gTowersForward;

   int rows = tmp_gTowersIDs_subset.size();
   int cols = tmp_gTowersIDs_subset[0].size();

   std::copy(&tmp_gTowersIDs_subset[0][0], &tmp_gTowersIDs_subset[0][0]+(rows*cols),&m_gTowersIDs[0][0]);

   gTowersCentral Atwr = {{{0}}};
   gTowersCentral Btwr = {{{0}}};
   gTowersForward CPtwr = {{{0}}};
   gTowersForward CNtwr = {{{0}}};



   //FPGA A----------------------------------------------------------------------------------------------------------------------------------------------
   gTowersCentral tmp_gTowersIDs_subset_centralFPGA;
   memset(&tmp_gTowersIDs_subset_centralFPGA, 0, sizeof tmp_gTowersIDs_subset_centralFPGA);
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::centralNphi; myrow++){
      for (int mycol = 0; mycol<12; mycol++){
         tmp_gTowersIDs_subset_centralFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+8];
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
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::centralNphi; myrow++){
      for (int mycol = 0; mycol<12; mycol++){
         tmp_gTowersIDs_subset_centralFPGA_B[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+20];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(1));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_centralFPGA_B);
   m_gFEXFPGA_Tool->GetEnergyMatrix(Btwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA B----------------------------------------------------------------------------------------------------------------------------------------------


   //FPGA C-P ----------------------------------------------------------------------------------------------------------------------------------------------
   //Use a matrix with 32 rows, even if FPGA-C (positive) also deals with regions of 16 bins in phi (those connected to FCAL).
   //We have 4 columns with 32 rows and 4 columns with 16 rows for each FPGA-C.
   //So we use a matrix 32x8 but we fill only half of it in the region 3.3<|eta|<4.8.
   gTowersForward tmp_gTowersIDs_subset_forwardFPGA;
   memset(&tmp_gTowersIDs_subset_forwardFPGA, 0, sizeof tmp_gTowersIDs_subset_forwardFPGA);
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::centralNphi; myrow++){
      for (int mycol = 0; mycol<4; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+32];
      }
   }
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::forwardNphi; myrow++){
      for (int mycol = 4; mycol<8; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol+32];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(2));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_forwardFPGA);
   m_gFEXFPGA_Tool->GetEnergyMatrix(CPtwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA C-P----------------------------------------------------------------------------------------------------------------------------------------------


   //FPGA C-N----------------------------------------------------------------------------------------------------------------------------------------------
   //Use a matrix with 32 rows, even if FPGA-N (negative) also deals with regions of 16 bins in phi (those connected to FCAL).
   //We have 4 columns with 32 rows and 4 columns with 16 rows for each FPGA-C.
   //So we use a matrix 32x8 but we fill only half of it in the region 3.3<|eta|<4.8.
   gTowersForward tmp_gTowersIDs_subset_forwardFPGA_N;
   memset(&tmp_gTowersIDs_subset_forwardFPGA_N, 0, sizeof tmp_gTowersIDs_subset_forwardFPGA_N);
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::forwardNphi; myrow++){
      for (int mycol = 0; mycol<4; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA_N[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol];
      }
   }
   for (int myrow = 0; myrow<FEXAlgoSpaceDefs::centralNphi; myrow++){
      for (int mycol = 4; mycol<8; mycol++){
         tmp_gTowersIDs_subset_forwardFPGA_N[myrow][mycol] = tmp_gTowersIDs_subset[myrow][mycol];
      }
   }
   ATH_CHECK(m_gFEXFPGA_Tool->init(3));
   m_gFEXFPGA_Tool->SetTowersAndCells_SG(tmp_gTowersIDs_subset_forwardFPGA_N);
   m_gFEXFPGA_Tool->GetEnergyMatrix(CNtwr);
   m_gFEXFPGA_Tool->reset();
   //FPGA C-N----------------------------------------------------------------------------------------------------------------------------------------------


   // Retrieve the L1 menu configuration
   SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey/*, ctx*/);
   ATH_CHECK(l1Menu.isValid());

   //Parameters related to gLJ (large-R jet objects - gJet)
   auto & thr_gLJ = l1Menu->thrExtraInfo().gLJ();
   int gLJ_seedThrA = 0;
   int gLJ_seedThrB = 0;
   gLJ_seedThrA = thr_gLJ.seedThr('A'); //defined in GeV by default
   gLJ_seedThrA = gLJ_seedThrA/0.2; //rescaling with 0.2 GeV scale to get counts (corresponding to hw units) 
   gLJ_seedThrB = thr_gLJ.seedThr('B'); //defined in GeV by default
   gLJ_seedThrB = gLJ_seedThrB/0.2; //rescaling with 0.2 GeV scale to get counts (corresponding to hw units) 
   int gLJ_ptMinToTopoCounts1 = 0;
   int gLJ_ptMinToTopoCounts2 = 0;
   gLJ_ptMinToTopoCounts1 = thr_gLJ.ptMinToTopoCounts(1); 
   gLJ_ptMinToTopoCounts2 = thr_gLJ.ptMinToTopoCounts(2); 
   
   //Parameters related to gJ (small-R jet objects - gBlock)
   auto & thr_gJ = l1Menu->thrExtraInfo().gJ();
   int gJ_ptMinToTopoCounts1 = 0;
   int gJ_ptMinToTopoCounts2 = 0;
   gJ_ptMinToTopoCounts1 = thr_gJ.ptMinToTopoCounts(1); 
   gJ_ptMinToTopoCounts2 = thr_gJ.ptMinToTopoCounts(2); 

   int pucA = 0;
   int pucB = 0;
   //note that jetThreshold is not a configurable parameter in firmware, it is used to check that jet values are positive
   int jetThreshold = 0;

   // The output TOBs, to be filled by the gFEXJetAlgoTool
   std::array<uint32_t, 7> ATOB1_dat = {0};
   std::array<uint32_t, 7> ATOB2_dat = {0};
   std::array<uint32_t, 7> BTOB1_dat = {0};
   std::array<uint32_t, 7> BTOB2_dat = {0};

   // Use the gFEXJetAlgoTool

   // Pass the energy matrices to the algo tool, and run the algorithms
   auto tobs_v = m_gFEXJetAlgoTool->largeRfinder(Atwr, Btwr, CNtwr, CPtwr, pucA, pucB, 
                                                gLJ_seedThrA, gLJ_seedThrB, gJ_ptMinToTopoCounts1, gJ_ptMinToTopoCounts2, 
                                                jetThreshold, gLJ_ptMinToTopoCounts1, gLJ_ptMinToTopoCounts2,
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


   // Use the gFEXJetAlgoTool
   std::array<uint32_t, 4> outTOB = {0};

   //Parameters related to gXE (JwoJ MET objects)
   auto & thr_gXE = l1Menu->thrExtraInfo().gXE();
   int gXE_seedThrA = 0;
   int gXE_seedThrB = 0;
   gXE_seedThrA = thr_gXE.seedThr('A'); //defined in GeV by default
   gXE_seedThrA = gXE_seedThrA/0.2; //rescaling with 0.2 GeV scale to get counts (corresponding to hw units) 
   gXE_seedThrB = thr_gXE.seedThr('B'); //defined in GeV by default
   gXE_seedThrB = gXE_seedThrB/0.2; //rescaling with 0.2 GeV scale to get counts (corresponding to hw units) 

   unsigned int aFPGA_A = 0;
   unsigned int bFPGA_A = 0;
   unsigned int aFPGA_B = 0;
   unsigned int bFPGA_B = 0;
   aFPGA_A = thr_gXE.JWOJ_param('A','a');
   bFPGA_A = thr_gXE.JWOJ_param('A','b');
   aFPGA_B = thr_gXE.JWOJ_param('B','a');
   bFPGA_B = thr_gXE.JWOJ_param('B','b');
   

   m_gFEXJwoJAlgoTool->setAlgoConstant(aFPGA_A, bFPGA_A,
                                       aFPGA_B, bFPGA_B,
                                       gXE_seedThrA, gXE_seedThrB);

   auto global_tobs = m_gFEXJwoJAlgoTool->jwojAlgo(Atwr, Btwr, outTOB);

   m_gScalarEJwojTobWords.resize(1);
   m_gMETComponentsJwojTobWords.resize(1);
   m_gMHTComponentsJwojTobWords.resize(1);
   m_gMSTComponentsJwojTobWords.resize(1);


   //Placing the global TOBs into a dedicated array
   m_gScalarEJwojTobWords[0] = outTOB[0];//
   m_gMETComponentsJwojTobWords[0] = outTOB[1];//
   m_gMHTComponentsJwojTobWords[0] = outTOB[2];//
   m_gMSTComponentsJwojTobWords[0] = outTOB[3];//


   for (int i = 0; i <14; i++){
     gFEXOutputs->addJetTob(tobs_v[i]->getWord());
     gFEXOutputs->addValueJet("EtaJet", tobs_v[i]->getEta());
     gFEXOutputs->addValueJet("PhiJet", tobs_v[i]->getPhi());
     gFEXOutputs->addValueJet("ETJet", tobs_v[i]->getET());
     gFEXOutputs->addValueJet("StatusJet", tobs_v[i]->getStatus());
     gFEXOutputs->addValueJet("TobIDJet", tobs_v[i]->getTobID());
     gFEXOutputs->fillJet();

   }

   for (int i = 0; i <4; i++){
     gFEXOutputs->addGlobalTob(global_tobs[i]->getWord());
     gFEXOutputs->addValueGlobal("GlobalQuantity1", global_tobs[i]->getQuantity1());
     gFEXOutputs->addValueGlobal("GlobalQuantity2", global_tobs[i]->getQuantity2());
     gFEXOutputs->addValueGlobal("SaturationGlobal", global_tobs[i]->getSaturation());
     gFEXOutputs->addValueGlobal("TobIDGlobal", global_tobs[i]->getTobID());
     gFEXOutputs->addValueGlobal("GlobalStatus1", global_tobs[i]->getStatus1());
     gFEXOutputs->addValueGlobal("GlobalStatus2", global_tobs[i]->getStatus2());
     gFEXOutputs->fillGlobal();

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

std::vector<uint32_t> gFEXSim::getgScalarEJwojTOBs() const
{
  return m_gScalarEJwojTobWords;
}

std::vector<uint32_t> gFEXSim::getgMETComponentsJwojTOBs() const
{
  return m_gMETComponentsJwojTobWords;
}

std::vector<uint32_t> gFEXSim::getgMHTComponentsJwojTOBs() const
{
  return m_gMHTComponentsJwojTobWords;
}

std::vector<uint32_t> gFEXSim::getgMSTComponentsJwojTOBs() const
{
  return m_gMSTComponentsJwojTobWords;
}



} // end of namespace bracket
