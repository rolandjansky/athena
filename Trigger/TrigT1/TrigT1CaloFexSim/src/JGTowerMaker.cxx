/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL1Calo includes
#include "iostream"
#include <sstream>
#include <fstream>
#include <TMath.h>

#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/WriteHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigT1CaloFexSim/JGTowerMaker.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TFile.h"
JGTowerMaker::JGTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  declareProperty("useSCQuality",m_useSCQuality=true);
  declareProperty("useAllCalo",m_useAllCalo=true);
}


JGTowerMaker::~JGTowerMaker() {


  for(unsigned iJT=0; iJT<jT.size(); iJT++){
     delete jT.at(iJT);
  }
  for(unsigned iGT=0; iGT<gT.size(); iGT++){
     delete gT.at(iGT);
  }
}


StatusCode JGTowerMaker::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc.retrieve() );

  CHECK(detStore()->retrieve(m_scid));
  CHECK(detStore()->retrieve(m_tid));
  CHECK(detStore()->retrieve(m_jTowerId));
  CHECK(detStore()->retrieve(m_gTowerId));
  CHECK(detStore()->retrieve(m_sem_mgr));
  CHECK(detStore()->retrieve (m_ccIdHelper) );
  detStore()->dump();
  CHECK(SCTowerMapping()); 
  m_TileMapped = false;

  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::JFexAlg(xAOD::JGTowerContainer*jTContainer){

  int m_jTowerHashMax=m_jTowerId->tower_hash_max();
  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, "SCell") );


  for (int hs=0;hs<m_jTowerHashMax;++hs){
      
      xAOD::JGTower* m_trigTower = new xAOD::JGTower();
      jTContainer->push_back(m_trigTower);
      JGTower*jt = jT.at(hs);      

      m_trigTower->initialize(hs,jt->Eta(),jt->Phi());
      float jEt=0;
      float lar_et=0;
      float tile_et=0;
      std::vector<int> jTowerTileIndex = jt->GetTileIndices();

      std::vector<int> jTowerSCIndex = jt->GetSCIndices();
      //Filling tower energy with SC
      const Identifier jid=m_jTowerId->tower_id(hs);
 
      for(unsigned i=0; i<jTowerSCIndex.size(); i++){
         const Identifier scid=m_scid->cell_id(jTowerSCIndex.at(i));
         const IdentifierHash sc_hash = m_scid->calo_cell_hash(scid);
         CaloCell* scell = (CaloCell*) scells->findCell(sc_hash);
         if(scell==nullptr||!(m_useSCQuality&&( scell->provenance()  & 0x40))) continue;
         float scell_et = scell->et();
         jEt += scell_et; 
         lar_et+=scell_et;
      }

      if(m_jTowerId->sampling(jid)==1){
        if(m_useAllCalo){
          const CaloCellContainer* cells = 0;
          CHECK( evtStore()->retrieve( cells, "AllCalo") );
          for(unsigned cell_hs=0 ; cell_hs<jTowerTileIndex.size(); cell_hs++){
             const CaloCell * cell = cells->findCell(jTowerTileIndex.at(cell_hs));
             jEt+=cell->e()*cell->sinTh();
             tile_et+=cell->e()*cell->sinTh();
          }
        } 

        else{
          const xAOD::TriggerTowerContainer* TTs;
          if(evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ) {
            ATH_MSG_INFO("ERROR loading trigger tower");
            return StatusCode::FAILURE;
          }

          for(unsigned tt_hs=0 ; tt_hs<TTs->size(); tt_hs++){
             const xAOD::TriggerTower * tt = TTs->at(tt_hs);
             if(tt->sampling()!=1 || fabs(tt->eta())>1.5) continue; // Tile raneg upto 1.5 with sampling == 1

             if(!inBox(jt->Eta(),tt->eta(),jt->dEta()/2,jt->Phi(),tt->phi(),jt->dPhi()/2)) continue;

             float cpET = 1000*tt->cpET();
             jEt+=cpET;
             tile_et+=cpET;
          }

        }
      }
      m_trigTower->setdEta(jt->dEta());
      m_trigTower->setdPhi(jt->dPhi());
      m_trigTower->setEt(jEt);
      m_trigTower->setTileFrac(tile_et/jEt);
      m_trigTower->setSCIndex(jTowerSCIndex);
      m_trigTower->setTileIndex(jTowerTileIndex);
      m_trigTower->setTileEt(tile_et);
      m_trigTower->setLArEt(lar_et);
      
  }
  return StatusCode::SUCCESS; 
}

StatusCode JGTowerMaker::GFexAlg(xAOD::JGTowerContainer*gTContainer)
{
  int m_gTowerHashMax=m_gTowerId->tower_hash_max();
  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, "SCell") );

  for (int hs=0;hs<m_gTowerHashMax;++hs){

      xAOD::JGTower* m_trigTower = new xAOD::JGTower();
      gTContainer->push_back(m_trigTower);
      const Identifier gid=m_gTowerId->tower_id(hs);
     

      JGTower*gt = gT.at(hs);

      m_trigTower->initialize(hs,gt->Eta(),gt->Phi());
      float gEt=0;
      float lar_et=0;
      float tile_et=0;
      std::vector<int> gTowerTileIndex = gt->GetTileIndices();

      std::vector<int> gTowerSCIndex = gt->GetSCIndices();
      //Filling tower energy with SC

      for(unsigned i=0;i<gTowerSCIndex.size();i++){
         
         CaloCell*scell = (CaloCell*) scells->findCell(gTowerSCIndex.at(i));
         if(scell==nullptr||!(m_useSCQuality&&( scell->provenance()  & 0x40))) continue;

         float scell_et = scell->et();
         gEt += scell_et;
         lar_et+=scell_et;
      }

      if(m_gTowerId->sampling(gid)==1) {

        if(m_useAllCalo){
          const CaloCellContainer* cells = 0;
          CHECK( evtStore()->retrieve( cells, "AllCalo") );
          for(unsigned cell_hs=0 ; cell_hs<gTowerTileIndex.size(); cell_hs++){
             const CaloCell * cell = cells->findCell(gTowerTileIndex.at(cell_hs));
             gEt+=cell->e()*cell->sinTh();
             tile_et+=cell->e()*cell->sinTh();
          }
        }

        else{
          const xAOD::TriggerTowerContainer* TTs;

          if (evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ) {
             ATH_MSG_INFO("ERROR loading trigger tower");
             return StatusCode::FAILURE;
          }

          for(unsigned tt_hs=0 ; tt_hs<TTs->size(); tt_hs++){
             const xAOD::TriggerTower * tt = TTs->at(tt_hs);
             if(tt->sampling()!=1 || fabs(tt->eta())>1.5) continue; // Tile raneg upto 1.5 with sampling == 1

             if(!inBox(gt->Eta(),tt->eta(),gt->dEta()/2,gt->Phi(),tt->phi(),gt->dPhi()/2)) continue;
             float cpET = 1000*tt->cpET();
             gEt+=cpET;
             tile_et+=cpET;
          }
        }
      }
      m_trigTower->setdEta(gt->dEta());
      m_trigTower->setdPhi(gt->dPhi());
      m_trigTower->setEt(gEt);
      m_trigTower->setTileFrac(tile_et/gEt);
      m_trigTower->setSCIndex(gTowerSCIndex);
      m_trigTower->setTileIndex(gTowerTileIndex);
      m_trigTower->setTileEt(tile_et);
      m_trigTower->setLArEt(lar_et);
    
  }
  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::execute() {  

  ATH_MSG_DEBUG ("Executing" << name() << "...");


  
  if(!m_TileMapped){
    if(m_useAllCalo) CHECK(AllCaloMapping());
  }

  m_TileMapped = true;

 
  xAOD::JGTowerAuxContainer* jTAuxContainer = new xAOD::JGTowerAuxContainer() ;
  xAOD::JGTowerContainer*    jTContainer =new xAOD::JGTowerContainer() ;
  jTContainer->setStore(jTAuxContainer);
  xAOD::JGTowerAuxContainer* gTAuxContainer = new xAOD::JGTowerAuxContainer() ;
  xAOD::JGTowerContainer*    gTContainer =new xAOD::JGTowerContainer() ;
  gTContainer->setStore(gTAuxContainer);

  CHECK( JFexAlg(jTContainer));
  CHECK( GFexAlg(gTContainer));


  CHECK(evtStore()->record( gTContainer, "GTower" ) );
  CHECK(evtStore()->record( gTAuxContainer, "GTowerAux." )) ;

  CHECK(evtStore()->record( jTContainer, "JTower" ) );
  CHECK(evtStore()->record( jTAuxContainer, "JTowerAux." )) ;

  return StatusCode::SUCCESS;
}


StatusCode JGTowerMaker::AllCaloMapping(){

  const CaloCellContainer* cells = 0;
  CHECK( evtStore()->retrieve( cells, "AllCalo") );


  for(unsigned cell_hs=0 ; cell_hs<cells->size(); cell_hs++){
     
     const CaloCell * cell = cells->findCell(cell_hs);
     if(cell==nullptr) continue;
     if(!(m_ccIdHelper->is_tile(cell->ID()))) continue;

     int m_jTowerHashMax=m_jTowerId->tower_hash_max();
     for (int hs=0;hs<m_jTowerHashMax;++hs){
         const Identifier jid=m_jTowerId->tower_id(hs);
         if(m_jTowerId->sampling(jid)==0) continue;
         JGTower*jt = jT.at(hs);
         if(!inBox(jt->Eta(),cell->eta(),jt->dEta()/2,jt->Phi(),cell->phi(),jt->dPhi()/2)) jt->SetTileIndices(cell_hs);
     }

     int m_gTowerHashMax=m_gTowerId->tower_hash_max();
     for (int hs=0;hs<m_gTowerHashMax;++hs){
         const Identifier gid=m_gTowerId->tower_id(hs);
         if(m_gTowerId->sampling(gid)==0) continue;
         JGTower*gt = gT.at(hs);
         if(!inBox(gt->Eta(),cell->eta(),gt->dEta()/2,gt->Phi(),cell->phi(),gt->dPhi()/2)) gt->SetTileIndices(cell_hs);
     }

  }
  return StatusCode::SUCCESS;
}


StatusCode JGTowerMaker::SCTowerMapping(){


  int m_jTowerHashMax=m_jTowerId->tower_hash_max();
  LArTTCell jSCs;

  for (int hs=0;hs<m_jTowerHashMax;++hs){

      const Identifier jid=m_jTowerId->tower_id(hs);
      const Identifier rid = m_jTowerId->region_id(jid);

      int detSide = 1;
      if ( m_jTowerId->pos_neg(jid) < 0) detSide = -1;

      // Get the phi and eta boundaries of the towers (only advisable for barrel)
      float jDEta = m_jTowerId->etaGranularity(rid);
      float jDPhi = m_jTowerId->phiGranularity(rid);
      int nTowers = (int)(TMath::Pi()/jDPhi)+1;
      jDPhi = TMath::Pi()/nTowers;

      float jEta = (m_jTowerId->eta(jid)+1-0.5)*jDEta*detSide+m_jTowerId->eta0(rid)*detSide;
      float jPhi = (m_jTowerId->phi(jid)+1-0.5)*jDPhi+m_jTowerId->phi0(rid);
      if(jPhi>TMath::Pi()) jPhi = jPhi-2*TMath::Pi(); //m_jTowerId->phi0(rid)-jDPhi*(2*nTowers-m_jTowerId->phi(jid));

      JGTower*JT = new JGTower(jEta,jDEta,jPhi,jDPhi);
 
      unsigned sc_hashMax = m_scid-> calo_cell_hash_max();

      for (unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
          const Identifier scid=m_scid->cell_id(sc_hs);
          const Identifier tid=m_tid->cell_id(scid);
          //const IdentifierHash sc_hash = m_scid->calo_cell_hash(scid);
        
          if( (m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2)) continue; //skip all tile SCs
          const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
          int det = m_scid->sub_calo(scid);
        
          // Very speical case being handle here. At the end of the barrel there is a constant-eta ring that has
          // eta_raw=1.4 (which is the midpoint of the scell). These will be put into the g/jTower that starts at eta=1.4
          float scEta = dde->eta_raw();
          float scPhi = dde->phi_raw();
          if(fabs(fabs(dde->eta_raw())-1.4)<0.001 && m_scid->region(scid) == 0 && m_scid->sampling(scid) == 2){
            if(scEta > 0) scEta += 0.05;
            else          scEta -= 0.05;
          }


          if(inBox(jEta,scEta,jDEta/2,jPhi,scPhi,jDPhi/2) && m_jTowerId->pos_neg(jid)*m_scid->pos_neg(scid) > 0 
            &&((m_scid->is_em(scid) && m_jTowerId->sampling(jid)==0) 
            ||(!m_scid->is_em(scid) && m_jTowerId->sampling(jid)==1))){

            JT->SetSCIndices(sc_hs);

            LArTTCell_t m;
            m.tpn     = m_jTowerId->pos_neg(jid);
            m.tsample = m_jTowerId->sampling(jid);
            m.tregion = m_jTowerId->region(jid);
            m.teta    = m_jTowerId->eta(jid);
            m.tphi    = m_jTowerId->phi(jid);
            m.det    = det;
            if(!m_scid->is_tile(scid)){
              m.pn     = m_scid->pos_neg(scid);
              m.sample = m_scid->sampling(scid);
              m.region = m_scid->region(scid);
              m.eta    = m_scid->eta(scid);
              m.phi    = m_scid->phi(scid);
            }
            else{
              m.pn     = m_tid->side(tid);
              m.sample = m_tid->sampling(tid);
              m.region = m_tid->region(tid);
              m.eta    = m_tid->tower(tid);
              m.phi    = m_tid->module(tid);
            }

            jSCs.push_back(m);
          } //inbox matching
          if(hs!=0) continue;

      }     //SC loop
      jT.push_back(JT);
  }         //jTower loop

  myJMap->set(jSCs);
  CHECK(detStore()->record(myJMap,"JTowerSCMap"));

  int m_gTowerHashMax = m_gTowerId->tower_hash_max();


  LArTTCell gSCs;
  for (int hs=0;hs<m_gTowerHashMax;++hs){
      const Identifier gid=m_gTowerId->tower_id(hs);
      const Identifier rid = m_gTowerId->region_id(gid);

      int detSide = 1;
      if ( m_gTowerId->pos_neg(gid) < 0) detSide = -1;

      // Get the phi and eta boundaries of the towers (only advisable for barrel)
      float gDEta = m_gTowerId->etaGranularity(rid);
      float gDPhi = m_gTowerId->phiGranularity(rid);
      int nTowers = (int)(TMath::Pi()/gDPhi)+1;
      gDPhi = TMath::Pi()/nTowers;

      float gEta = (m_gTowerId->eta(gid)+1-0.5)*gDEta*detSide+m_gTowerId->eta0(rid)*detSide;
      float gPhi = (m_gTowerId->phi(gid)+1-0.5)*gDPhi+m_gTowerId->phi0(rid);
      if(gPhi>TMath::Pi()) gPhi = gPhi-2*TMath::Pi();
      JGTower*GT = new JGTower(gEta,gDEta,gPhi,gDPhi);

      unsigned sc_hashMax = m_scid-> calo_cell_hash_max();
      for (unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
          const Identifier scid=m_scid->cell_id(sc_hs);
          const Identifier tid=m_tid->cell_id(scid);

          if(m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2) continue;
          const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
          int det = m_scid->sub_calo(scid);

          // Very speical case being handle here. At the end of the barrel there is a constant-eta ring that has
          // eta_raw=1.4 (which is the midpoint of the scell). These will be put into the g/gTower that starts at eta=1.4
          float scEta = dde->eta_raw();
          float scPhi = dde->phi_raw();
          if(fabs(fabs(dde->eta_raw())-1.4)<0.001 && m_scid->region(scid) == 0 && m_scid->sampling(scid) == 2)
            {
            if(scEta > 0) scEta += 0.05;
            else          scEta -= 0.05;
          }

          if(inBox(gEta,scEta,gDEta/2,gPhi,scPhi,gDPhi/2) 
            && m_gTowerId->pos_neg(gid)*m_scid->pos_neg(scid) > 0
            &&((m_scid->is_em(scid) && m_gTowerId->sampling(gid)==0)
            ||(!m_scid->is_em(scid) && m_gTowerId->sampling(gid)==1))){

            GT->SetSCIndices(sc_hs);

            LArTTCell_t m;
            m.tpn     = m_gTowerId->pos_neg(gid);
            m.tsample = m_gTowerId->sampling(gid);
            m.tregion = m_gTowerId->region(gid);
            m.teta    = m_gTowerId->eta(gid);
            m.tphi    = m_gTowerId->phi(gid);
            m.det    = det;
            if(!m_scid->is_tile(scid)){
              m.pn     = m_scid->pos_neg(scid);
              m.sample = m_scid->sampling(scid);
              m.region = m_scid->region(scid);
              m.eta    = m_scid->eta(scid);
              m.phi    = m_scid->phi(scid);
            }
            else{
              m.pn     = m_tid->side(tid);
              m.sample = m_tid->sampling(tid);
              m.region = m_tid->region(tid);
              m.eta    = m_tid->tower(tid);
              m.phi    = m_tid->module(tid);
            }
            gSCs.push_back(m);

        } //inbox matching
      }     //SC looping
      gT.push_back(GT);
  }         //gTower looping

  myGMap->set(gSCs);  //not properly in use, but still taken as the tool to identify duplication of SCs
  CHECK(detStore()->record(myGMap,"GTowerSCMap"));


  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::beginInputFile() {  

  return StatusCode::SUCCESS;
}


