/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"
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

  transObj->setSumTrkPtCone02 (persObj->allTheFloats[0]);
  transObj->setSumTrkPtCone03 (persObj->allTheFloats[1]);
  transObj->setSumEtCone01 (persObj->allTheFloats[2]);
  transObj->setSumEtCone02 (persObj->allTheFloats[3]);
  transObj->setSumEtCone03 (persObj->allTheFloats[4]);
  transObj->setSumEtCone04 (persObj->allTheFloats[5]);

  transObj->setEFMuonInfoTrackLink (persObj->m_trkPos);

  ElementLink<TrigMuonEFInfoContainer> muonEFLink;
  infoEleLinkCnv.persToTrans( &persObj->m_muonEFLink, &muonEFLink, log);
  transObj->setEFMuonInfoElementLink (muonEFLink);

}//persToTrans

/**
 * Convert transient representation to persistent one.
 */
void TrigMuonEFIsolationCnv_p2::transToPers(const TrigMuonEFIsolation* transObj, TrigMuonEFIsolation_p2* persObj, MsgStream &log) {

  log << MSG::DEBUG << "Called TrigMuonEFIsolationCnv_p2::transToPers" << endreq;

  persObj->allTheFloats[0] = transObj->sumTrkPtCone02();
  persObj->allTheFloats[1] = transObj->sumTrkPtCone03();
  persObj->allTheFloats[2] = transObj->sumEtCone01();
  persObj->allTheFloats[3] = transObj->sumEtCone02();
  persObj->allTheFloats[4] = transObj->sumEtCone03();
  persObj->allTheFloats[5] = transObj->sumEtCone04();

  persObj->m_trkPos = transObj->trackPosition();

  infoEleLinkCnv.transToPers( &transObj->getMuonInfoLink(), &persObj->m_muonEFLink, log);

}//transToPers
