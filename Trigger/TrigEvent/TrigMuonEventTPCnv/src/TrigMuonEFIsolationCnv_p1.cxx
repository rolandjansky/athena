/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p1.h"


/**
 * Convert persistent representation of object to the transient one
 */
void TrigMuonEFIsolationCnv_p1::persToTrans(const TrigMuonEFIsolation_p1* persObj, TrigMuonEFIsolation* transObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p1::persToTrans" << endreq;

  transObj->m_sumTrkPtCone02 = persObj->allTheFloats[0];
  transObj->m_sumTrkPtCone03 = persObj->allTheFloats[1];

  transObj->m_trkPos = persObj->m_trkPos;

}//persToTrans

/**
 * Convert transient representation to persistent one.
 */
void TrigMuonEFIsolationCnv_p1::transToPers(const TrigMuonEFIsolation* transObj, TrigMuonEFIsolation_p1* persObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p1::transToPers" << endreq;

  persObj->allTheFloats[0] = transObj->m_sumTrkPtCone02;
  persObj->allTheFloats[1] = transObj->m_sumTrkPtCone03;

  persObj->m_trkPos = transObj->m_trkPos;

}//transToPers
