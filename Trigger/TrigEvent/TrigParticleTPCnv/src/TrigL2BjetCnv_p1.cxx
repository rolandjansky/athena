/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p1.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p1.h"
 

//* Persistent to transient *//
void TrigL2BjetCnv_p1::persToTrans(const TrigL2Bjet_p1 *persObj, TrigL2Bjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p1::persToTrans called " << endreq;

  *transObj = TrigL2Bjet (persObj->m_roiID,
                          persObj->m_eta,
                          persObj->m_phi,
                          nullptr, // track
                          nullptr, // prmvertex,
                          nullptr, // secvertex
                          persObj->m_prmVtx,
                          0,       // ptjet
                          persObj->m_xcomb,
                          persObj->m_x2d,  // xIP1d
                          persObj->m_xd0,  // xIP2d
                          persObj->m_xz0,  // xIP3d
                          -9.9,            // xChi2
                          persObj->m_x3d,  // xSv
                          persObj->m_xmvtx,
                          persObj->m_xevtx,
                          persObj->m_xnvtx);
  transObj->validate (persObj->m_valid);
}
 
//* Transient to persistent *//
void TrigL2BjetCnv_p1::transToPers(const TrigL2Bjet *transObj, TrigL2Bjet_p1 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p1::transToPers called " << endreq;
   
  persObj->m_valid  = transObj->isValid();
  persObj->m_roiID  = transObj->roiId();
  persObj->m_eta    = transObj->eta();
  persObj->m_phi    = transObj->phi();
  persObj->m_prmVtx = transObj->prmVtx();
  persObj->m_xcomb  = transObj->xComb();
  persObj->m_xmvtx  = transObj->xMVtx();
  persObj->m_xevtx  = transObj->xEVtx();  
  persObj->m_xnvtx  = transObj->xNVtx();

  persObj->m_x2d    = transObj->xIP1D();
  persObj->m_xd0    = transObj->xIP2D();
  persObj->m_xz0    = transObj->xIP3D();
  persObj->m_x3d    = transObj->xSV();

}
