/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticleTPCnv/TrigEFBjet_p2.h"
#include "TrigParticleTPCnv/TrigEFBjetCnv_p2.h"
 

//* Persistent to transient *//
void TrigEFBjetCnv_p2::persToTrans(const TrigEFBjet_p2 *persObj, TrigEFBjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigEFBjetCnv_p2::persToTrans called " << endreq;

  *transObj = TrigEFBjet (persObj->m_roiID,
                          0, // eta
                          0, // phi
                          nullptr,  // track
                          nullptr,  // prmvertex
                          nullptr,  // secvertex
                          persObj->m_prmVtx,
                          0,        // ptjet
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
void TrigEFBjetCnv_p2::transToPers(const TrigEFBjet *transObj, TrigEFBjet_p2 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigEFBjetCnv_p2::transToPers called " << endreq;
  
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
