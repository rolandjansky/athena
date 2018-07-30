/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIPixelTracklets.h"

#include "HIRecTPCnv/HIPixelTrackletsCnv_p1.h"

void HIPixelTrackletsCnv_p1::persToTrans(const HIPixelTracklets_p1 *persObj, HIPixelTracklets *transObj, MsgStream &log) {
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_pt = persObj->m_pt;
  transObj->m_etaS = persObj->m_etaS;
  transObj->m_phiS = persObj->m_phiS;
}

void HIPixelTrackletsCnv_p1::transToPers(const HIPixelTracklets *transObj, HIPixelTracklets_p1 *persObj, MsgStream &log) {
  log << MSG::DEBUG << "HIPixelTrackletsCnv_p1:transToPers:starting." << endmsg;
  persObj->m_eta = transObj->m_eta;
  persObj->m_phi = transObj->m_phi;
  persObj->m_pt = transObj->m_pt;
  persObj->m_etaS = transObj->m_etaS;
  persObj->m_phiS = transObj->m_phiS;
}
