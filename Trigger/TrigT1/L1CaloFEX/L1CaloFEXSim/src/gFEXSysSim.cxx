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

      ATH_CHECK(m_gFexRhoOutKey.initialize()); 

      ATH_CHECK(m_gFexBlockOutKey.initialize());      

      ATH_CHECK(m_gFexJetOutKey.initialize());

      ATH_CHECK(m_gScalarEJwojOutKey.initialize());

      ATH_CHECK(m_gMETComponentsJwojOutKey.initialize());

      ATH_CHECK(m_gMHTComponentsJwojOutKey.initialize());
      
      ATH_CHECK(m_gMSTComponentsJwojOutKey.initialize());

   

      return StatusCode::SUCCESS;
   }



   void gFEXSysSim::cleanup()   {

      m_gFEXCollection.clear();

   }


   int gFEXSysSim::calcTowerID(int eta, int phi, int nphi, int mod)   {

      return ((nphi*eta) + phi + mod);

   }


   StatusCode gFEXSysSim::execute(gFEXOutputCollection* gFEXOutputs)   {

      SG::ReadHandle<LVL1::gTowerContainer> this_gTowerContainer(m_gTowerContainerSGKey);
      if(!this_gTowerContainer.isValid()){
         ATH_MSG_FATAL("Could not retrieve gTowerContainer " << m_gTowerContainerSGKey.key());
         return StatusCode::FAILURE;
      }

      // remove TOBs of the previous events from the array
      m_allgRhoTobs.clear();
      m_allgBlockTobs.clear();
      m_allgJetTobs.clear();
      m_allgScalarEJwojTobs.clear();
      m_allgMETComponentsJwojTobs.clear();
      m_allgMHTComponentsJwojTobs.clear();
      m_allgMSTComponentsJwojTobs.clear();


      // int centralNphi = 32;
      // int forwardNphi = 16;

      int fcalEta = 19; int fcalPhi = 0; int fcalMod = 900000;
      int initialFCAL = calcTowerID(fcalEta,fcalPhi,FEXAlgoSpaceDefs::forwardNphi,fcalMod);//900304
      int transfcalEta = 15; int transfcalPhi = 0; int transfcalMod = 700000;
      int initialTRANSFCAL = calcTowerID(transfcalEta,transfcalPhi,FEXAlgoSpaceDefs::forwardNphi,transfcalMod);//700240
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
      int fcalposEta = 16; int fcalposPhi = 0; int fcalposMod = 1000000;
      int initialposFCAL = calcTowerID(fcalposEta,fcalposPhi,FEXAlgoSpaceDefs::forwardNphi,fcalposMod);//1000240


      // Since gFEX consists of a single module, here we are just (re)assigning the gTowerID

      // Defining a matrix 32x40 corresponding to the gFEX structure (32 phi x 40 eta in the most general case - forward region has 16 phi bins)
      typedef  std::array<std::array<int, 40>, 32> gTowersIDs;
      gTowersIDs tmp_gTowersIDs_subset;

      int rows = tmp_gTowersIDs_subset.size();
      int cols = tmp_gTowersIDs_subset[0].size();

      // set the FCAL negative part
      for(int thisCol=0; thisCol<4; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialFCAL - ((thisCol) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the TRANSFCAL negative part (FCAL-EMEC overlap)
      for(int thisCol=4; thisCol<7; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialTRANSFCAL - ((thisCol-4) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the EMEC negative part
      for(int thisCol=7; thisCol<12; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialEMEC - ((thisCol-7) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the TRANSEMB (EMB-EMEC overlap) negative part
      for(int thisRow = 0; thisRow < rows; thisRow++){
         int thisCol = 12;
         int towerid = initialTRANSEMB + thisRow;
         tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
      }

      // set the EMB negative part
      for(int thisCol = 13; thisCol < 20; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
           int towerid = initialEMB - ( (thisCol-13) * 32) + thisRow;
           tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

         // set the EMB positive part
      for(int thisCol = 20; thisCol < 27; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialposEMB + ( (thisCol-20) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the TRANSEMB (EMB-EMEC overlap) positive part
      for(int thisRow = 0; thisRow < rows; thisRow++){
         int thisCol = 27;
         int towerid = initialposTRANSEMB + thisRow;
         tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
      }
      // set the EMEC positive part
      for(int thisCol=28; thisCol<33; thisCol++){
         for(int thisRow=0; thisRow<rows; thisRow++){
            int towerid = initialposEMEC + ((thisCol-28) * 32) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the TRANSFCAL positive part (EMEC-FCAL overlap)
      for(int thisCol=33; thisCol<36; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialposTRANSFCAL + ((thisCol-33) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
         }
      }

      // set the FCAL positive part
      for(int thisCol=36; thisCol<cols; thisCol++){
         for(int thisRow=0; thisRow<rows/2; thisRow++){
            int towerid = initialposFCAL + ((thisCol-36) * 16) + thisRow;
            tmp_gTowersIDs_subset[thisRow][thisCol] = towerid;
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

      ATH_CHECK(m_gFEXSimTool->executegFEXSim(tmp_gTowersIDs_subset, gFEXOutputs));
      
      m_allgRhoTobs = m_gFEXSimTool->getgRhoTOBs();
      m_allgBlockTobs = m_gFEXSimTool->getgBlockTOBs();
      m_allgJetTobs = m_gFEXSimTool->getgJetTOBs();
       
      m_allgScalarEJwojTobs = m_gFEXSimTool->getgScalarEJwojTOBs();
      m_allgMETComponentsJwojTobs = m_gFEXSimTool->getgMETComponentsJwojTOBs();
      m_allgMHTComponentsJwojTobs = m_gFEXSimTool->getgMHTComponentsJwojTOBs();
      m_allgMSTComponentsJwojTobs = m_gFEXSimTool->getgMSTComponentsJwojTOBs();

      m_gFEXSimTool->reset();

      //Makes containers for different gFEX Jet objects
      m_gRhoContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gRhoAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gRhoContainer->setStore(m_gRhoAuxContainer.get());

      m_gBlockContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gBlockAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gBlockContainer->setStore(m_gBlockAuxContainer.get());

      m_gJetContainer = std::make_unique<xAOD::gFexJetRoIContainer> ();
      m_gJetAuxContainer = std::make_unique<xAOD::gFexJetRoIAuxContainer> ();
      m_gJetContainer->setStore(m_gJetAuxContainer.get());

      //Makes containers for different gFEX Global objects
      m_gScalarEJwojContainer = std::make_unique<xAOD::gFexGlobalRoIContainer> ();
      m_gScalarEJwojAuxContainer = std::make_unique<xAOD::gFexGlobalRoIAuxContainer> ();
      m_gScalarEJwojContainer->setStore(m_gScalarEJwojAuxContainer.get());

      m_gMETComponentsJwojContainer = std::make_unique<xAOD::gFexGlobalRoIContainer> ();
      m_gMETComponentsJwojAuxContainer = std::make_unique<xAOD::gFexGlobalRoIAuxContainer> ();
      m_gMETComponentsJwojContainer->setStore(m_gMETComponentsJwojAuxContainer.get());

      m_gMHTComponentsJwojContainer = std::make_unique<xAOD::gFexGlobalRoIContainer> ();
      m_gMHTComponentsJwojAuxContainer = std::make_unique<xAOD::gFexGlobalRoIAuxContainer> ();
      m_gMHTComponentsJwojContainer->setStore(m_gMHTComponentsJwojAuxContainer.get());

      m_gMSTComponentsJwojContainer = std::make_unique<xAOD::gFexGlobalRoIContainer> ();
      m_gMSTComponentsJwojAuxContainer = std::make_unique<xAOD::gFexGlobalRoIAuxContainer> ();
      m_gMSTComponentsJwojContainer->setStore(m_gMSTComponentsJwojAuxContainer.get());

      

      //iterate over all gRho Tobs and fill EDM with them
      for(auto &tob : m_allgRhoTobs){
         ATH_CHECK(fillgRhoEDM(tob));
      }
      //iterate over all gBlock Tobs and fill EDM with them
      for(auto &tob : m_allgBlockTobs){
         //Check the status of the TOB (bit 7)
         //Only fill the EDM if status = 1 (means it passed the threshold, see JetFinder algorithm) 
         int statusBit = 7;
         int statusMask = 0x1;
         int tob_status = (tob >> statusBit) & statusMask;
         if (tob_status == 1){
         ATH_CHECK(fillgBlockEDM(tob));
         }
      }

      //iterate over all gJet Tobs and fill EDM with them
      for(auto &tob : m_allgJetTobs){
         //Check the status of the TOB (bit 7)
         //Only fill the EDM if status = 1 (means it passed the threshold, see JetFinder algorithm) 
         int statusBit = 7;
         int statusMask = 0x1;
         int tob_status = (tob >> statusBit) & statusMask;
         if (tob_status == 1){
            ATH_CHECK(fillgJetEDM(tob));   
         }
      }

      //iterate over all JwoJ scalar energy Tobs and fill EDM with them (should be only one)
      for(auto &tob : m_allgScalarEJwojTobs){
         ATH_CHECK(fillgScalarEJwojEDM(tob));
      }
      //iterate over all JwoJ METcomponents Tobs and fill EDM with them (should be only one)
      for(auto &tob : m_allgMETComponentsJwojTobs){
         ATH_CHECK(fillgMETComponentsJwojEDM(tob));
      }
      //iterate over all JwoJ MHTcomponents Tobs and fill EDM with them (should be only one)
      for(auto &tob : m_allgMHTComponentsJwojTobs){
         ATH_CHECK(fillgMHTComponentsJwojEDM(tob));
      }
      //iterate over all JwoJ MSTcomponents Tobs and fill EDM with them (should be only one)
      for(auto &tob : m_allgMSTComponentsJwojTobs){
         ATH_CHECK(fillgMSTComponentsJwojEDM(tob));
      }

      



      SG::WriteHandle<xAOD::gFexJetRoIContainer> outputgFexRhoHandle(m_gFexRhoOutKey);
      ATH_MSG_DEBUG("   write: " << outputgFexRhoHandle.key() << " = " << "..." );
      ATH_CHECK(outputgFexRhoHandle.record(std::move(m_gRhoContainer),std::move(m_gRhoAuxContainer)));

      SG::WriteHandle<xAOD::gFexJetRoIContainer> outputgFexBlockHandle(m_gFexBlockOutKey);
      ATH_MSG_DEBUG("   write: " << outputgFexBlockHandle.key() << " = " << "..." );
      ATH_CHECK(outputgFexBlockHandle.record(std::move(m_gBlockContainer),std::move(m_gBlockAuxContainer)));

      SG::WriteHandle<xAOD::gFexJetRoIContainer> outputgFexJetHandle(m_gFexJetOutKey);
      ATH_MSG_DEBUG("   write: " << outputgFexJetHandle.key() << " = " << "..." );
      ATH_CHECK(outputgFexJetHandle.record(std::move(m_gJetContainer),std::move(m_gJetAuxContainer)));


      SG::WriteHandle<xAOD::gFexGlobalRoIContainer> outputgScalarEJwojHandle(m_gScalarEJwojOutKey);
      ATH_MSG_DEBUG("   write: " << outputgScalarEJwojHandle.key() << " = " << "..." );
      ATH_CHECK(outputgScalarEJwojHandle.record(std::move(m_gScalarEJwojContainer),std::move(m_gScalarEJwojAuxContainer)));

      SG::WriteHandle<xAOD::gFexGlobalRoIContainer> outputgMETComponentsJwojHandle(m_gMETComponentsJwojOutKey);
      ATH_MSG_DEBUG("   write: " << outputgMETComponentsJwojHandle.key() << " = " << "..." );
      ATH_CHECK(outputgMETComponentsJwojHandle.record(std::move(m_gMETComponentsJwojContainer),std::move(m_gMETComponentsJwojAuxContainer)));

      SG::WriteHandle<xAOD::gFexGlobalRoIContainer> outputgMHTComponentsJwojHandle(m_gMHTComponentsJwojOutKey);
      ATH_MSG_DEBUG("   write: " << outputgMHTComponentsJwojHandle.key() << " = " << "..." );
      ATH_CHECK(outputgMHTComponentsJwojHandle.record(std::move(m_gMHTComponentsJwojContainer),std::move(m_gMHTComponentsJwojAuxContainer)));

      SG::WriteHandle<xAOD::gFexGlobalRoIContainer> outputgMSTComponentsJwojHandle(m_gMSTComponentsJwojOutKey);
      ATH_MSG_DEBUG("   write: " << outputgMSTComponentsJwojHandle.key() << " = " << "..." );
      ATH_CHECK(outputgMSTComponentsJwojHandle.record(std::move(m_gMSTComponentsJwojContainer),std::move(m_gMSTComponentsJwojAuxContainer)));

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgRhoEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
      m_gRhoContainer->push_back(std::move(myEDM));
      m_gRhoContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgBlockEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
      m_gBlockContainer->push_back(std::move(myEDM));
      m_gBlockContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgJetEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
      m_gJetContainer->push_back(std::move(myEDM));
      m_gJetContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgMETComponentsJwojEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
      m_gMETComponentsJwojContainer->push_back(std::move(myEDM));
      m_gMETComponentsJwojContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgMHTComponentsJwojEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
      m_gMHTComponentsJwojContainer->push_back(std::move(myEDM));
      m_gMHTComponentsJwojContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgMSTComponentsJwojEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
      m_gMSTComponentsJwojContainer->push_back(std::move(myEDM));
      m_gMSTComponentsJwojContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

   StatusCode gFEXSysSim::fillgScalarEJwojEDM(uint32_t tobWord){

      std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
      m_gScalarEJwojContainer->push_back(std::move(myEDM));
      m_gScalarEJwojContainer->back()->initialize(tobWord);

      return StatusCode::SUCCESS;
   }

} // end of namespace bracket
