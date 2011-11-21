/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LUCID_SimEvent/LUCID_SimHit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCnv_p1.h"

void LUCID_SimHitCnv_p1::persToTrans(const LUCID_SimHit_p1* persObj, LUCID_SimHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << "LUCID_SimHitCnv_p1::persToTrans called " << endreq;
  
  HepMcParticleLinkCnv_p1 HepMcPLCnv;
  
  transObj->m_tubeID        = persObj->m_tubeID;
  transObj->m_track         = persObj->m_track;
  transObj->m_pdgCode       = persObj->m_pdgCode;
  transObj->m_genVolume     = persObj->m_genVolume;
  transObj->m_stepStartPosX = persObj->m_stepStartPosX;
  transObj->m_stepStartPosY = persObj->m_stepStartPosY;
  transObj->m_stepStartPosZ = persObj->m_stepStartPosZ;
  transObj->m_stepEndPosX   = persObj->m_stepEndPosX;
  transObj->m_stepEndPosY   = persObj->m_stepEndPosY;
  transObj->m_stepEndPosZ   = persObj->m_stepEndPosZ;
  transObj->m_preStepTime   = persObj->m_preStepTime;
  transObj->m_postStepTime  = persObj->m_postStepTime; 
  transObj->m_wavelength    = persObj->m_wavelength;
  transObj->m_energy        = persObj->m_energy;
  
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);   
}

void LUCID_SimHitCnv_p1::transToPers(const LUCID_SimHit* transObj, LUCID_SimHit_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << "LUCID_SimHitCnv_p1::transToPers called " << endreq;
  
  HepMcParticleLinkCnv_p1 HepMcPLCnv;
  
  persObj->m_tubeID        = transObj->m_tubeID;
  persObj->m_track         = transObj->m_track;
  persObj->m_pdgCode       = transObj->m_pdgCode;
  persObj->m_genVolume     = transObj->m_genVolume;
  persObj->m_stepStartPosX = transObj->m_stepStartPosX;
  persObj->m_stepStartPosY = transObj->m_stepStartPosY;
  persObj->m_stepStartPosZ = transObj->m_stepStartPosZ;
  persObj->m_stepEndPosX   = transObj->m_stepEndPosX;
  persObj->m_stepEndPosY   = transObj->m_stepEndPosY;
  persObj->m_stepEndPosZ   = transObj->m_stepEndPosZ;
  persObj->m_preStepTime   = transObj->m_preStepTime;
  persObj->m_postStepTime  = transObj->m_postStepTime;
  persObj->m_wavelength    = transObj->m_wavelength;
  persObj->m_energy        = transObj->m_energy;
  
  HepMcPLCnv.transToPers(&(transObj->m_partLink),&(persObj->m_partLink), log);   
}
