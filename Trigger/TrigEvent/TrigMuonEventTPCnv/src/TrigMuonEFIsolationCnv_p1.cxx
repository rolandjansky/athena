/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p1.h"


/**
 * Convert persistent representation of object to the transient one
 */
void TrigMuonEFIsolationCnv_p1::persToTrans(const TrigMuonEFIsolation_p1* persObj, TrigMuonEFIsolation* transObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p1::persToTrans" << endreq;

  transObj->setSumTrkPtCone02 (persObj->allTheFloats[0]);
  transObj->setSumTrkPtCone03 (persObj->allTheFloats[1]);

  transObj->setEFMuonInfoTrackLink (persObj->m_trkPos);

}//persToTrans

/**
 * Convert transient representation to persistent one.
 */
void TrigMuonEFIsolationCnv_p1::transToPers(const TrigMuonEFIsolation* transObj, TrigMuonEFIsolation_p1* persObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p1::transToPers" << endreq;

  persObj->allTheFloats[0] = transObj->sumTrkPtCone02();
  persObj->allTheFloats[1] = transObj->sumTrkPtCone03();

  persObj->m_trkPos = transObj->trackPosition();

}//transToPers
