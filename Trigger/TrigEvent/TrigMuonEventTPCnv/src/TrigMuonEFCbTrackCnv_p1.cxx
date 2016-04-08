/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p1.h"


void TrigMuonEFCbTrackCnv_p1::persToTrans(const TrigMuonEFCbTrack_p1* persObj,
					  TrigMuonEFCbTrack* transObj,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p1::persToTrans called " << endreq;

   transObj->setMatchChi2 (persObj->m_matchChi2);

   //fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p1::transToPers(const TrigMuonEFCbTrack* /*transObj*/,
					  TrigMuonEFCbTrack_p1* /*persObj*/,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p1::transToPers called " << endreq;
//
//  persObj->m_matchChi2 = transObj->m_matchChi2;
//
//  //persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
//  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
}
