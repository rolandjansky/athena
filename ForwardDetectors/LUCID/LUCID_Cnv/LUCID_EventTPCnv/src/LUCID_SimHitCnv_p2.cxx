/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_SimEvent/LUCID_SimHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCnv_p2.h"

void LUCID_SimHitCnv_p2::persToTrans(const LUCID_SimHit_p2* persObj, LUCID_SimHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << "LUCID_SimHitCnv_p2::persToTrans called " << endmsg;

  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  HepMcParticleLink link;
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(link), log);
  *transObj = LUCID_SimHit (persObj->m_tubeID,
                            persObj->m_pdgCode,
                            link,
                            persObj->m_genVolume,
                            persObj->m_stepStartPosX,
                            persObj->m_stepStartPosY,
                            persObj->m_stepStartPosZ,
                            persObj->m_stepEndPosX,
                            persObj->m_stepEndPosY,
                            persObj->m_stepEndPosZ,
                            persObj->m_preStepTime,
                            persObj->m_postStepTime,
                            persObj->m_wavelength,
                            persObj->m_energy);
}

void LUCID_SimHitCnv_p2::transToPers(const LUCID_SimHit* transObj, LUCID_SimHit_p2* persObj, MsgStream& log) {

  log << MSG::DEBUG << "LUCID_SimHitCnv_p2::transToPers called " << endmsg;

  HepMcParticleLinkCnv_p2 HepMcPLCnv;

  persObj->m_tubeID        = transObj->GetTubeID();
  persObj->m_track         = transObj->GetTrack();
  persObj->m_pdgCode       = transObj->GetPdgCode();
  persObj->m_genVolume     = transObj->GetGenVolume();
  persObj->m_stepStartPosX = transObj->GetX();
  persObj->m_stepStartPosY = transObj->GetY();
  persObj->m_stepStartPosZ = transObj->GetZ();
  persObj->m_stepEndPosX   = transObj->GetEPX();
  persObj->m_stepEndPosY   = transObj->GetEPY();
  persObj->m_stepEndPosZ   = transObj->GetEPZ();
  persObj->m_preStepTime   = transObj->GetPreStepTime();
  persObj->m_postStepTime  = transObj->GetPostStepTime();
  persObj->m_wavelength    = transObj->GetWavelength();
  persObj->m_energy        = transObj->GetEnergy();

  HepMcPLCnv.transToPers(&(transObj->particleLink()),&(persObj->m_partLink), log);
}
