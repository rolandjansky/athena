/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXSysSim - Overall gFEX simulation
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gFEXSysSim.h"
#include "L1CaloFEXSim/gFEXSim.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gTowerContainer.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

#include <ctime>

namespace LVL1 {

   // default constructor for persistency

   gFEXSysSim::gFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent):
      AthAlgTool(type,name,parent)
   {
      declareInterface<IgFEXSysSim>(this);

   }


   //---------------- Initialisation -------------------------------------------------

   StatusCode gFEXSysSim::initialize()
   {

      ATH_CHECK(m_gTowerContainerSGKey.initialize());

      ATH_CHECK( m_gFEXSimTool.retrieve() );

      ATH_CHECK(m_gFexJetOutKey.initialize());

      return StatusCode::SUCCESS;
   }



   void gFEXSysSim::cleanup()   {

      m_gFEXCollection.clear();

   }


   int gFEXSysSim::calcTowerID(int eta, int phi, int nphi, int mod)   {

      return ((nphi*eta) + phi + mod);

   }


   StatusCode gFEXSysSim::execute()   {

      SG::ReadHandle<LVL1::gTowerContainer> this_gTowerContainer(m_gTowerContainerSGKey/*,ctx*/);
      if(!this_gTowerContainer.isValid()){
         ATH_MSG_FATAL("Could not retrieve gTowerContainer " << m_gTowerContainerSGKey.key());
         return StatusCode::FAILURE;
      }

      // remove TOBs of the previous events from the array
      m_allgRhoTobs.clear();
      m_allgBlockTobs.clear();
      m_allgJetTobs.clear();


      // int centralNphi = 32;
      // int forwardNphi = 16;

      int fcalEta = 18; int fcalPhi = 0; int fcalMod = 900000;
      int initialFCAL = calcTowerID(fcalEta,fcalPhi,FEXAlgoSpaceDefs::forwardNphi,fcalMod);//900288
      int transfcalEta = 14; int transfcalPhi = 0; int transfcalMod = 700000;
      int initialTRANSFCAL = calcTowerID(transfcalEta,transfcalPhi,FEXAlgoSpaceDefs::forwardNphi,transfcalMod);//700224
      int emecEta = 12; int emecPhi = 0; int emecMod = 500000;
      int initialEMEC = calcTowerID(emecEta,emecPhi,FEXAlgoSpaceDefs::centralNphi,emecMod);//500384
      int transembEta = 7; int transembPhi = 0; int transembMod = 300000;
      int initialTRANSEMB = calcTowerID(transembEta,transembPhi,FEXAlgoSpaceDefs::centralNphi,transembMod);///300224
      int embEta = 6; int embPhi = 0; int embMod = 100000;
      int initialEMB = calcTowerID(embEta,embPhi,FEXAlgoSpaceDefs::centralNphi,embMod);//100192


      int embposEta = 0; int embposPhi = 0; int embposMod = 200000;
      int initialposEMB = calcTowerID(embposEta,embposPhi,FEXAlgoSpaceDefs::centralNphi,embposMod);//200000
      int transembposEta = 7; int transembposPhi = 0; int transembposMod = 400000;
      int initialposTRANSEMB = calcTowerID(transembposEta,transembposPhi,FEXAlgoSpaceDefs::centralNphi,transembposMod);//400224
      int emecposEta = 8; int emecposPhi = 0; int emecposMod = 600000;
      int initialposEMEC = calcTowerID(emecposEta,emecposPhi,FEXAlgoSpaceDefs::centralNphi,emecposMod);//600256
      int transfcalposEta = 13; int transfcalposPhi = 0; int transfcalposMod = 800000;
      int initialposTRANSFCAL = calcTowerID(transfcalposEta,transfcalposPhi,FEXAlgoSpaceDefs::forwardNphi,transfcalposMod);//800208
      int fcalposEta = 15; int fcalposPhi = 0; int fcalposMod = 1000000;
      int initialposFCAL = calcTowerID(fcalposEta,fcalposPhi,FEXAlgoSpaceDefs::forwardNphi,fcalposMod);//1000240


      // Since gFEX consists of a single module, here we are just (re)assigning the gTowerID
      std::map<int,gTower> tmp_gTowersColl_subset;

      // Defining a matrix 32x38 corresponding to the gFEX structure (32 phi x 38 eta in the most general case - forward region has 16 phi bins)
      // int tmp_gTowersIDs_subset [32][38];
      typedef  std::array<std::array<int, 38>, 32> gTowersIDs;
      gTowersIDs tmp_gTowersIDs_subset;

      int rows = tmp_gTowersIDs_subset.size();
      int cols = tmp_gTowersIDs_subset[0].size();

      // set the FCAL negative part
      for(int thisCol=0; thisCol<4; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialFCAL - ((thisCol) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the TRANSFCAL negative part (FCAL-EMEC overlap)
      for(int thisCol=4; thisCol<6; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialTRANSFCAL - ((thisCol-4) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the EMEC negative part
      for(int thisCol=6; thisCol<11; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialEMEC - ((thisCol-6) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the TRANSEMB (EMB-EMEC overlap) negative part
      for(int thisRow = 0; thisRow < rows; thisRow++){
         int thisCol = 11;
         int towerid = initialTRANSEMB + thisRow;
         tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

      }

      // set the EMB negative part
      for(int thisCol = 12; thisCol < 19; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
           int towerid = initialEMB - ( (thisCol-12) * 32) + thisRow;
           tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
           tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

         // set the EMB positive part
      for(int thisCol = 19; thisCol < 26; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialposEMB + ( (thisCol-19) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the TRANSEMB (EMB-EMEC overlap) positive part
      for(int thisRow = 0; thisRow < rows; thisRow++){
         int thisCol = 26;
         int towerid = initialposTRANSEMB + thisRow;
         tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

      }
      // set the EMEC positive part
      for(int thisCol=27; thisCol<32; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialposEMEC + ((thisCol-27) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the TRANSFCAL positive part (EMEC-FCAL overlap)
      for(int thisCol=32; thisCol<34; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialposTRANSFCAL + ((thisCol-32) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      // set the FCAL positive part
      for(int thisCol=34; thisCol<cols; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialposFCAL + ((thisCol-34) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
            tmp_gTowersColl_subset.insert( std::map<int, gTower>::value_type(towerid,   *(this_gTowerContainer->findTower(towerid))));

         }
      }

      if(false){
         ATH_MSG_DEBUG("CONTENTS OF gFEX : ");
         for (int thisRow=rows-1; thisRow>=0; thisRow--){
            for (int thisCol=0; thisCol<cols; thisCol++){
               int tmptowerid = tmp_gTowersIDs_subset[thisRow][thisCol];
               const float tmptowereta = this_gTowerContainer->findTower(tmptowerid)->eta();
               const float tmptowerphi = this_gTowerContainer->findTower(tmptowerid)->phi();
               if(thisCol != cols-1){ ATH_MSG_DEBUG("|   " << tmptowerid << "([" << tmptowerphi << "][" << tmptowereta << "])   "); }
               else { ATH_MSG_DEBUG("|   " << tmptowerid << "([" << tmptowereta << "][" << tmptowerphi << "])   |"); }
            }
         }
      }

      ATH_CHECK(m_gFEXSimTool->executegFEXSim(tmp_gTowersIDs_subset));
      m_allgRhoTobs = m_gFEXSimTool->getgRhoTOBs();
      m_allgBlockTobs = m_gFEXSimTool->getgBlockTOBs();
      m_allgJetTobs = m_gFEXSimTool->getgJetTOBs();
      m_gFEXSimTool->reset();


      m_gRhoContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gRhoAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gRhoContainer->setStore(m_gRhoAuxContainer.get());

      m_gBlockContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gBlockAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gBlockContainer->setStore(m_gBlockAuxContainer.get());

      m_gJetContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gJetAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gJetContainer->setStore(m_gJetAuxContainer.get());

      //iterate over all gRho Tobs and fill EDM with them
      for(auto &tob : m_allgRhoTobs){
         ATH_CHECK(fillgRhoEDM(tob));
      }
      //iterate over all gBlock Tobs and fill EDM with them
      for(auto &tob : m_allgBlockTobs){
         ATH_CHECK(fillgBlockEDM(tob));

      }
      //iterate over all gJet Tobs and fill EDM with them
      for(auto &tob : m_allgJetTobs){
         ATH_CHECK(fillgJetEDM(tob));
      }

      SG::WriteHandle<xAOD::gFexJetRoIContainer> outputgFexJetHandle(m_gFexJetOutKey/*, ctx*/);
      ATH_MSG_DEBUG("   write: " << outputgFexJetHandle.key() << " = " << "..." );
      ATH_CHECK(outputgFexJetHandle.record(std::move(m_gJetContainer),std::move(m_gJetAuxContainer)));

       return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgRhoEDM(uint32_t tobWord){
      xAOD::gFexJetRoI* myEDM = new xAOD::gFexJetRoI();
      m_gRhoContainer->push_back( myEDM );
      myEDM->initialize(tobWord);
      ATH_MSG_DEBUG(" setting gRho : " << myEDM->tobEt()  );

      return StatusCode::SUCCESS;

   }

   StatusCode gFEXSysSim::fillgBlockEDM(uint32_t tobWord){
      xAOD::gFexJetRoI* myEDM = new xAOD::gFexJetRoI();
      m_gBlockContainer->push_back( myEDM );

      myEDM->initialize(tobWord);
      ATH_MSG_DEBUG(" setting gBlock et: " << myEDM->tobEt() << " eta: " << myEDM->iEta() <<   " phi: " << myEDM->iPhi() );

      return StatusCode::SUCCESS;

   }

   StatusCode gFEXSysSim::fillgJetEDM(uint32_t tobWord){

      xAOD::gFexJetRoI* myEDM = new xAOD::gFexJetRoI();
      m_gJetContainer->push_back( myEDM );

      myEDM->initialize(tobWord);
      ATH_MSG_DEBUG(" setting gJet et: " << myEDM->tobEt() << " eta: " << myEDM->iEta() <<   " phi: " << myEDM->iPhi() );

      return StatusCode::SUCCESS;

   }


} // end of namespace bracket