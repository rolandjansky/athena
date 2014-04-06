/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p3.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigL2BjetCnv_p3.h"
 

//* Persistent to transient *//
void TrigL2BjetCnv_p3::persToTrans(const TrigL2Bjet_p3 *persObj, TrigL2Bjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::persToTrans called " << endreq;

  transObj->m_valid  = persObj->m_valid;
  transObj->m_roiID  = persObj->m_roiID;

  transObj->m_prmVtx = persObj->m_allTheFloats[0];
  transObj->m_xcomb  = persObj->m_allTheFloats[1];
  transObj->m_xIP1d  = persObj->m_allTheFloats[2];
  transObj->m_xIP2d  = persObj->m_allTheFloats[3];
  transObj->m_xIP3d  = persObj->m_allTheFloats[4];
  transObj->m_xChi2  = persObj->m_allTheFloats[5];
  transObj->m_xSv    = persObj->m_allTheFloats[6];
  transObj->m_xmvtx  = persObj->m_allTheFloats[7];
  transObj->m_xevtx  = persObj->m_allTheFloats[8];
  transObj->m_xnvtx  = persObj->m_allTheFloats[9];

  m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);

//  fillTransFromPStore(&m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiMCnv, transObj, log);

}
 
//* Transient to persistent *//
void TrigL2BjetCnv_p3::transToPers(const TrigL2Bjet *transObj, TrigL2Bjet_p3 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::transToPers called " << endreq;
   
  persObj->m_valid  = transObj->m_valid;
  persObj->m_roiID  = transObj->m_roiID;

  log << MSG::DEBUG << "TrigL2BjetCnv_p3::transToPers Integers done! " << endreq;

  persObj->m_allTheFloats[0] = transObj->m_prmVtx;
  persObj->m_allTheFloats[1] = transObj->m_xcomb;
  persObj->m_allTheFloats[2] = transObj->m_xIP1d;
  persObj->m_allTheFloats[3] = transObj->m_xIP2d;
  persObj->m_allTheFloats[4] = transObj->m_xIP3d;
  persObj->m_allTheFloats[5] = transObj->m_xChi2;
  persObj->m_allTheFloats[6] = transObj->m_xSv;
  persObj->m_allTheFloats[7] = transObj->m_xmvtx;
  persObj->m_allTheFloats[8] = transObj->m_xevtx;
  persObj->m_allTheFloats[9] = transObj->m_xnvtx;

  m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);
//  persObj->m_p4PtEtaPhiMCnv = baseToPersistent(&m_p4PtEtaPhiMCnv, transObj, log);
 
}
