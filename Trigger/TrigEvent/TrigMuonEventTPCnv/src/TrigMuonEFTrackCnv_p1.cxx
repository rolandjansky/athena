/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p1.h"


void TrigMuonEFTrackCnv_p1::persToTrans(const TrigMuonEFTrack_p1* persObj,
					TrigMuonEFTrack* transObj,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p1::persToTrans called " << endreq;

   transObj->setCharge   (persObj->m_charge);

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p1::transToPers(const TrigMuonEFTrack* /*transObj*/,
					TrigMuonEFTrack_p1* /*persObj*/,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p1::transToPers called " << endreq;
//
//  persObj->m_charge   = transObj->m_charge   ;
//
//  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
}
