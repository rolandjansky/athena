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

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"


namespace G4UA
{

  //----------------------------------------------------------------------------
  G4HitFilter::G4HitFilter(const Config& config)
    : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),
                   "G4HitFilter"),
      m_config(config)
  {}

  //----------------------------------------------------------------------------
  void G4HitFilter::BeginOfRunAction(const G4Run*)
  {
    if(m_config.volumenames.size()==0){
      ATH_MSG_ERROR("No hit containers declared");
    }

    int hitType = 999;

    for(const auto& vol: m_config.volumenames){
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

      if(hitType == 999){
        ATH_MSG_ERROR("unknown hit tipe" << vol);
      }

      m_hitContainers.push_back( std::make_pair(hitType,vol) );
    }
  }

  //----------------------------------------------------------------------------
  void G4HitFilter::EndOfEventAction(const G4Event*)
  {
    unsigned int counter = 0;

    m_report.ntot++;

    for(const auto& hitCont : m_hitContainers) {

      if(hitCont.first == CALOCALIB) {
        SG::ReadHandle<CaloCalibrationHitContainer> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == CSC) {
        SG::ReadHandle<CSCSimHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == LAR) {
        SG::ReadHandle<LArHitContainer> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == LUCID) {
        SG::ReadHandle<LUCID_SimHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == MDT) {
        SG::ReadHandle<MDTSimHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == RPC) {
        SG::ReadHandle<RPCSimHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == SI) {
        SG::ReadHandle<SiHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == TGC) {
        SG::ReadHandle<TGCSimHitCollection> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == TILE) {
        SG::ReadHandle<TileHitVector> cont(hitCont.second);
        if(! cont.isValid()) continue;
        counter += cont->size();
      }
      else if(hitCont.first == TRT) {
        SG::ReadHandle<TRTUncompressedHitCollection> cont(hitCont.second);
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
