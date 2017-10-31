/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_SimEvent/LUCID_SimHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCnv_p2.h"

void LUCID_SimHitCnv_p2::persToTrans(const LUCID_SimHit_p2* persObj, LUCID_SimHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << "LUCID_SimHitCnv_p2::persToTrans called " << endreq;

  // redundant with m_track.
  //HepMcParticleLinkCnv_p2 HepMcPLCnv;
  //HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);
  *transObj = LUCID_SimHit (persObj->m_tubeID,
                            persObj->m_pdgCode,
                            persObj->m_track,
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

void LUCID_SimHitCnv_p2::transToPers(const LUCID_SimHit*, LUCID_SimHit_p2*, MsgStream& /*log*/) {
  throw std::runtime_error("LUCID_SimHitCnv_p2::transToPers is not supported in this release!");
}
