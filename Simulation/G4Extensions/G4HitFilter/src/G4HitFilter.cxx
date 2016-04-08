/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4HitFilter/G4HitFilter.h"

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

#include <iostream>

G4HitFilter::G4HitFilter(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
  m_volumenames(),
  m_ntot(0),
  m_npass(0),
  m_hitContainers(){

  declareProperty("VolumeNames",m_volumenames);
  
}

StatusCode G4HitFilter::initialize(){

  if(m_volumenames.size()==0){

    ATH_MSG_ERROR("No hit containers declared");
    return StatusCode::FAILURE;

  }

  int hitType = 999;

  for(auto vol: m_volumenames){
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
      return StatusCode::FAILURE;

    }
    
    m_hitContainers.push_back(std::make_pair(hitType,vol));


  }

  return StatusCode::SUCCESS;

}


void G4HitFilter::EndOfEvent(const G4Event*){
  unsigned int counter = 0;
  
  m_ntot++;

  std::vector<std::pair<int, std::string> >::iterator itr = m_hitContainers.begin();
  std::vector<std::pair<int, std::string> >::iterator itr_end = m_hitContainers.end();
  for(;itr!=itr_end;++itr) {
    if((*itr).first == CALOCALIB) {
      const CaloCalibrationHitContainer *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == CSC) {
      const CSCSimHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == LAR) {
      const LArHitContainer *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == LUCID) {
      const LUCID_SimHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == MDT) {
      const MDTSimHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == RPC) {
      const RPCSimHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == SI) {
      const SiHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TGC) {
      const TGCSimHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TILE) {
      const TileHitVector *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TRT) {
      const TRTUncompressedHitCollection *cont = 0;
      if(evtStore()->retrieve(cont, (*itr).second).isFailure() || !cont) continue;
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
    m_npass++;
  }
}


StatusCode G4HitFilter::finalize() {
  ATH_MSG_INFO("processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
  return StatusCode::SUCCESS;
}


StatusCode G4HitFilter::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
