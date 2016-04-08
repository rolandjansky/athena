/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p2.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p2.h"
 

//* Persistent to transient *//
void TrigL2BjetCnv_p2::persToTrans(const TrigL2Bjet_p2 *persObj, TrigL2Bjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p2::persToTrans called " << endreq;

  *transObj = TrigL2Bjet (persObj->m_roiID,
                          0,        // eta
                          0,        // phi
                          nullptr, // track
                          nullptr, // prmvertex,
                          nullptr, // secvertex
                          persObj->m_prmVtx,
                          0,       // ptjet
                          persObj->m_xcomb,
                          persObj->m_xIP1d,
                          persObj->m_xIP2d,
                          persObj->m_xIP3d,
                          persObj->m_xChi2,
                          persObj->m_xSv,
                          persObj->m_xmvtx,
                          persObj->m_xevtx,
                          persObj->m_xnvtx);
  transObj->validate (persObj->m_valid);

  fillTransFromPStore(&m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log);
}
 
//* Transient to persistent *//
void TrigL2BjetCnv_p2::transToPers(const TrigL2Bjet *transObj, TrigL2Bjet_p2 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p2::transToPers called " << endreq;
   
  persObj->m_valid  = transObj->isValid();
  persObj->m_roiID  = transObj->roiId();
  persObj->m_prmVtx = transObj->prmVtx();
  persObj->m_xcomb  = transObj->xComb();
  persObj->m_xIP1d  = transObj->xIP1D();
  persObj->m_xIP2d  = transObj->xIP2D();
  persObj->m_xIP3d  = transObj->xIP3D();
  persObj->m_xChi2  = transObj->xCHI2();
  persObj->m_xSv    = transObj->xSV();
  persObj->m_xmvtx  = transObj->xMVtx();
  persObj->m_xevtx  = transObj->xEVtx();
  persObj->m_xnvtx  = transObj->xNVtx();

  persObj->m_p4PtEtaPhiM = baseToPersistent(&m_p4PtEtaPhiMCnv, transObj, log);
 
}
