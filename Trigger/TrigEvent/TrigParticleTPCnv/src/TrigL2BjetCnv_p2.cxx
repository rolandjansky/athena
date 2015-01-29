/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigL2Bjet.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p2.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigL2BjetCnv_p2.h"
 

//* Persistent to transient *//
void TrigL2BjetCnv_p2::persToTrans(const TrigL2Bjet_p2 *persObj, TrigL2Bjet *transObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p2::persToTrans called " << endreq;

  transObj->m_valid  = persObj->m_valid;
  transObj->m_roiID  = persObj->m_roiID;
  transObj->m_prmVtx = persObj->m_prmVtx;
  transObj->m_xcomb  = persObj->m_xcomb;
  transObj->m_xIP1d  = persObj->m_xIP1d;
  transObj->m_xIP2d  = persObj->m_xIP2d;
  transObj->m_xIP3d  = persObj->m_xIP3d;
  transObj->m_xChi2  = persObj->m_xChi2;
  transObj->m_xSv    = persObj->m_xSv;
  transObj->m_xmvtx  = persObj->m_xmvtx;
  transObj->m_xevtx  = persObj->m_xevtx;
  transObj->m_xnvtx  = persObj->m_xnvtx;

  fillTransFromPStore(&m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log);

}
 
//* Transient to persistent *//
void TrigL2BjetCnv_p2::transToPers(const TrigL2Bjet *transObj, TrigL2Bjet_p2 *persObj, MsgStream &log) {

  log << MSG::DEBUG << "TrigL2BjetCnv_p2::transToPers called " << endreq;
   
  persObj->m_valid  = transObj->m_valid;
  persObj->m_roiID  = transObj->m_roiID;
  persObj->m_prmVtx = transObj->m_prmVtx;
  persObj->m_xcomb  = transObj->m_xcomb;
  persObj->m_xIP1d  = transObj->m_xIP1d;
  persObj->m_xIP2d  = transObj->m_xIP2d;
  persObj->m_xIP3d  = transObj->m_xIP3d;
  persObj->m_xChi2  = transObj->m_xChi2;
  persObj->m_xSv    = transObj->m_xSv;
  persObj->m_xmvtx  = transObj->m_xmvtx;
  persObj->m_xevtx  = transObj->m_xevtx;
  persObj->m_xnvtx  = transObj->m_xnvtx;

  persObj->m_p4PtEtaPhiM = baseToPersistent(&m_p4PtEtaPhiMCnv, transObj, log);
 
}
