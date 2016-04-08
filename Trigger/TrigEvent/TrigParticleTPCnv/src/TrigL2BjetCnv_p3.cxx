/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p3.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p3.h"
 

//* Persistent to transient *//
void TrigL2BjetCnv_p3::persToTrans(const TrigL2Bjet_p3 *persObj, TrigL2Bjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::persToTrans called " << endreq;

  *transObj = TrigL2Bjet (persObj->m_roiID,
                          0,        // eta
                          0,        // phi
                          nullptr, // track
                          nullptr, // prmvertex,
                          nullptr, // secvertex
                          persObj->m_allTheFloats[0], // prmVtx
                          0,       // ptjet
                          persObj->m_allTheFloats[1], // xcomb
                          persObj->m_allTheFloats[2], // xIP1d
                          persObj->m_allTheFloats[3], // xIP2d
                          persObj->m_allTheFloats[4], // xIP3d
                          persObj->m_allTheFloats[5], // xChi2
                          persObj->m_allTheFloats[6], // xSv,
                          persObj->m_allTheFloats[7], // xmvtx
                          persObj->m_allTheFloats[8], // xevtx
                          persObj->m_allTheFloats[9]); // xnvtx
  transObj->validate (persObj->m_valid);

  m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);

//  fillTransFromPStore(&m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiMCnv, transObj, log);

}
 
//* Transient to persistent *//
void TrigL2BjetCnv_p3::transToPers(const TrigL2Bjet *transObj, TrigL2Bjet_p3 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::transToPers called " << endreq;
   
  persObj->m_valid  = transObj->isValid();
  persObj->m_roiID  = transObj->roiId();

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::transToPers Integers done! " << endreq;

  persObj->m_allTheFloats[0] = transObj->prmVtx();
  persObj->m_allTheFloats[1] = transObj->xComb();
  persObj->m_allTheFloats[2] = transObj->xIP1D();
  persObj->m_allTheFloats[3] = transObj->xIP2D();
  persObj->m_allTheFloats[4] = transObj->xIP3D();
  persObj->m_allTheFloats[5] = transObj->xCHI2();
  persObj->m_allTheFloats[6] = transObj->xSV();
  persObj->m_allTheFloats[7] = transObj->xMVtx();
  persObj->m_allTheFloats[8] = transObj->xEVtx();
  persObj->m_allTheFloats[9] = transObj->xNVtx();

  m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);
//  persObj->m_p4PtEtaPhiMCnv = baseToPersistent(&m_p4PtEtaPhiMCnv, transObj, log);
 
}
