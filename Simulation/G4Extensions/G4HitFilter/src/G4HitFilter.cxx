/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4HitFilter.h"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "LArSimEvent/LArHitContainer.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "TileSimEvent/TileHitVector.h"
#include "LUCID_SimEvent/LUCID_SimHitCollection.h"

#include "StoreGate/ReadHandle.h"

#include <iostream>


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{


  G4HitFilter::G4HitFilter(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"G4HitFilter"),
    m_config(config),m_report(),
    m_evtStore("StoreGateSvc/StoreGateSvc","G4HitFilter"),
    m_detStore("StoreGateSvc/DetectorStore","G4HitFilter"){;
  }


  void G4HitFilter::BeginOfRunAction(const G4Run*){

    if(m_config.volumenames.size()==0){

      ATH_MSG_ERROR("No hit containers declared");

    }

    int hitType = 999;

    for(auto vol: m_config.volumenames){
      if(vol == "BCMHits") hitType = SI;
      else if(vol == "BLMHits") hitType = SI;
      else if(vol == "CSC_Hits") hitType = CSC;
      else if(vol == "LArCalibrationHitActive") hitType = CALOCALIB;
      else if(vol == "LArCalibrationHitDeadMaterial") hitType = CALOCALIB;
      else if(vol == "LArCalibrationHitInactive") hitType = CALOCALIB;
      else if(vol == "LArHitEMB") hitType = LAR;
      else if(vol == "LArHitEMEC") hitType = LAR;
      else if(vol == "LArHitFCAL") hitType = LAR;
      else if(vol == "LArHitHEC") hitType = LAR;
      else if(vol == "LucidSimHitsVector") hitType = LUCID;
      else if(vol == "MBTSHits") hitType = TILE;
      else if(vol == "MDT_Hits") hitType = MDT;
      else if(vol == "PixelHits") hitType = SI;
      else if(vol == "RPC_Hits") hitType = RPC;
      else if(vol == "SCT_Hits") hitType = SI;
      else if(vol == "TGC_Hits") hitType = TGC;
      else if(vol == "TRTUncompressedHits") hitType = TRT;
      else if(vol == "TileHitVec") hitType = TILE;


      if(hitType==999){

        ATH_MSG_ERROR("unknown hit tipe"<<vol);

      }

      m_hitContainers.push_back(std::make_pair(hitType,vol));

    }



  }


  void G4HitFilter::EndOfEventAction(const G4Event*){
    unsigned int counter = 0;

    m_report.ntot++;

    std::vector<std::pair<int, std::string> >::iterator itr = m_hitContainers.begin();
    std::vector<std::pair<int, std::string> >::iterator itr_end = m_hitContainers.end();

    for(;itr!=itr_end;++itr) {

      if((*itr).first == CALOCALIB) {
        SG::ReadHandle<CaloCalibrationHitContainer> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == CSC) {
        SG::ReadHandle<CSCSimHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == LAR) {
        SG::ReadHandle<LArHitContainer> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == LUCID) {
        SG::ReadHandle<LUCID_SimHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == MDT) {
        SG::ReadHandle<MDTSimHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == RPC) {
        SG::ReadHandle<RPCSimHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == SI) {
        SG::ReadHandle<SiHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == TGC) {
        SG::ReadHandle<TGCSimHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == TILE) {
        SG::ReadHandle<TileHitVector> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if((*itr).first == TRT) {
        SG::ReadHandle<TRTUncompressedHitCollection> cont((*itr).second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else {
        continue;
      }

      // Require at least one hit from one selected detector volume.
      if(counter>0) break;
    }

    if (counter==0){
      ATH_MSG_INFO("G4HitFilter: failing the event");
      G4RunManager::GetRunManager()->AbortEvent();
    }
    else {
      ATH_MSG_INFO("G4HitFilter: passing the event");

      m_report.npass++;

    }

  }

} // namespace G4UA
