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

static G4HitFilter ts1("G4HitFilter");

G4HitFilter::G4HitFilter(std::string s):
  FADS::UserAction(s),
  m_storeGate(0),
  m_ntot(0),
  m_npass(0),
  m_init(false),
  m_hitContainers(){
}

void G4HitFilter::BeginOfEventAction(const G4Event*){
  if(!m_init) ParseProperties();
}

void G4HitFilter::EndOfEventAction(const G4Event*){
  unsigned int counter = 0;
  
  m_ntot++;

  std::vector<std::pair<int, std::string> >::iterator itr = m_hitContainers.begin();
  std::vector<std::pair<int, std::string> >::iterator itr_end = m_hitContainers.end();
  for(;itr!=itr_end;++itr) {
    if((*itr).first == CALOCALIB) {
      const CaloCalibrationHitContainer *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == CSC) {
      const CSCSimHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == LAR) {
      const LArHitContainer *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == LUCID) {
      const LUCID_SimHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == MDT) {
      const MDTSimHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == RPC) {
      const RPCSimHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == SI) {
      const SiHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TGC) {
      const TGCSimHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TILE) {
      const TileHitVector *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else if((*itr).first == TRT) {
      const TRTUncompressedHitCollection *cont = 0;
      StatusCode sc = m_storeGate->retrieve(cont, (*itr).second);
      if(sc.isFailure() || !cont) continue;
      counter += cont->size();
    }
    else {
      continue;
    }

    // Require at least one hit from one selected detector volume.
    if(counter>0) break;
  }

  if (counter==0){
    std::cout << "G4HitFilter: failing the event" << std::endl;
    G4RunManager::GetRunManager()->AbortEvent();
  } 
  else {
    std::cout << "G4HitFilter: passing the event" << std::endl;
    m_npass++;
  }
}

void G4HitFilter::BeginOfRunAction(const G4Run*){
  if (!m_init) ParseProperties();
}

void G4HitFilter::ParseProperties(){
  std::map<std::string, std::string>::iterator itr = theProperties.begin();
  std::map<std::string, std::string>::iterator itr_end = theProperties.end();
  int hitType = 999;
  for(;itr!=itr_end;++itr) {
    if((*itr).first.find("VolumeName") != std::string::npos) {

      // To avoid string comparison during the event loop, store the
      // container type in an index.
      if((*itr).second == "BCMHits") hitType = SI;
      else if((*itr).second == "BLMHits") hitType = SI;
      else if((*itr).second == "CSC_Hits") hitType = CSC;
      else if((*itr).second == "LArCalibrationHitActive") hitType = CALOCALIB;
      else if((*itr).second == "LArCalibrationHitDeadMaterial") hitType = CALOCALIB;
      else if((*itr).second == "LArCalibrationHitInactive") hitType = CALOCALIB;
      else if((*itr).second == "LArHitEMB") hitType = LAR;
      else if((*itr).second == "LArHitEMEC") hitType = LAR;
      else if((*itr).second == "LArHitFCAL") hitType = LAR;
      else if((*itr).second == "LArHitHEC") hitType = LAR;
      else if((*itr).second == "LucidSimHitsVector") hitType = LUCID;
      else if((*itr).second == "MBTSHits") hitType = TILE;
      else if((*itr).second == "MDT_Hits") hitType = MDT;
      else if((*itr).second == "PixelHits") hitType = SI;
      else if((*itr).second == "RPC_Hits") hitType = RPC;
      else if((*itr).second == "SCT_Hits") hitType = SI;
      else if((*itr).second == "TGC_Hits") hitType = TGC;
      else if((*itr).second == "TRTUncompressedHits") hitType = TRT;
      else if((*itr).second == "TileHitVec") hitType = TILE;
      else {
	std::cerr << "G4HitFilter: error unknown HitContainer type." << std::endl;
	continue;
      }
      m_hitContainers.push_back(std::make_pair(hitType,(*itr).second));
    }
  }

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()){
    std::cout<< "G4HitFilter: BeginOfRunAction could not access StoreGateSvc!"<<std::endl;
  }

  m_init=true;
}
void G4HitFilter::EndOfRunAction(const G4Run*) {
  std::cout<<"G4HitFilter: processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter "<<std::endl;
}

void G4HitFilter::SteppingAction(const G4Step*){;}

