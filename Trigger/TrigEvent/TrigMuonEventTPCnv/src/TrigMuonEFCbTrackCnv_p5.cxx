/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p4.h"

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p5.h"


void TrigMuonEFCbTrackCnv_p5::persToTrans(const TrigMuonEFCbTrack_p4* persObj,
					  TrigMuonEFCbTrack* transObj,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p5::persToTrans called " << endreq;

  transObj->setMatchChi2    (persObj->m_matchChi2);
  transObj->setNIdSctHits   (persObj->m_nIdSctHits);
  transObj->setNIdPixelHits (persObj->m_nIdPixelHits);
  transObj->setNTrtHits     (persObj->m_nTrtHits);

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p5::transToPers(const TrigMuonEFCbTrack* transObj,
					  TrigMuonEFCbTrack_p4* persObj,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p5::transToPers called " << endreq;

  persObj->m_matchChi2    = transObj->matchChi2();
  persObj->m_nIdSctHits   = transObj->NIdSctHits();
  persObj->m_nIdPixelHits = transObj->NIdPixelHits();
  persObj->m_nTrtHits     = transObj->NTrtHits();

  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
}
