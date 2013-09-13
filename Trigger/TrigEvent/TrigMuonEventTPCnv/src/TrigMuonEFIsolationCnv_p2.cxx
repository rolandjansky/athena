/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p2.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

typedef ElementLinkCnv_p3< ElementLink<TrigMuonEFInfoContainer> > InfoEleLinkCnv_t;

static InfoEleLinkCnv_t infoEleLinkCnv;

/**
 * Convert persistent representation of object to the transient one
 */
void TrigMuonEFIsolationCnv_p2::persToTrans(const TrigMuonEFIsolation_p2* persObj, TrigMuonEFIsolation* transObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p2::persToTrans" << endreq;

  transObj->m_sumTrkPtCone02 = persObj->allTheFloats[0];
  transObj->m_sumTrkPtCone03 = persObj->allTheFloats[1];
  transObj->m_sumEtCone01 = persObj->allTheFloats[2];
  transObj->m_sumEtCone02 = persObj->allTheFloats[3];
  transObj->m_sumEtCone03 = persObj->allTheFloats[4];
  transObj->m_sumEtCone04 = persObj->allTheFloats[5];

  transObj->m_trkPos = persObj->m_trkPos;

  infoEleLinkCnv.persToTrans( &persObj->m_muonEFLink, &transObj->m_muonEFLink, log);

}//persToTrans

/**
 * Convert transient representation to persistent one.
 */
void TrigMuonEFIsolationCnv_p2::transToPers(const TrigMuonEFIsolation* transObj, TrigMuonEFIsolation_p2* persObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p2::transToPers" << endreq;

  persObj->allTheFloats[0] = transObj->m_sumTrkPtCone02;
  persObj->allTheFloats[1] = transObj->m_sumTrkPtCone03;
  persObj->allTheFloats[2] = transObj->m_sumEtCone01;
  persObj->allTheFloats[3] = transObj->m_sumEtCone02;
  persObj->allTheFloats[4] = transObj->m_sumEtCone03;
  persObj->allTheFloats[5] = transObj->m_sumEtCone04;

  persObj->m_trkPos = transObj->m_trkPos;

  infoEleLinkCnv.transToPers( &transObj->m_muonEFLink, &persObj->m_muonEFLink, log);

}//transToPers
