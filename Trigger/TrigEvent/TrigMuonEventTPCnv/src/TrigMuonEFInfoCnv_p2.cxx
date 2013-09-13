/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p2.h"


void TrigMuonEFInfoCnv_p2::persToTrans(const TrigMuonEFInfo_p2* persObj,
				       TrigMuonEFInfo* transObj,
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p2::persToTrans called " << endreq;

  transObj->m_roi               = persObj->m_roi;
  delete transObj->m_spectrometerTrack;
  transObj->m_spectrometerTrack = createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log);
  delete transObj->m_extrapolatedTrack;
  transObj->m_extrapolatedTrack = createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log);
  delete transObj->m_combinedTrack;
  transObj->m_combinedTrack     = createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log);

}


void TrigMuonEFInfoCnv_p2::transToPers(const TrigMuonEFInfo* /*transObj*/,
				       TrigMuonEFInfo_p2* /*persObj*/,
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p2::transToPers called " << endreq;

//  persObj->m_roi               = transObj->m_roi;
//  persObj->m_spectrometerTrack = toPersistent( &m_trackCnv, transObj->m_spectrometerTrack, log);
//  persObj->m_extrapolatedTrack = toPersistent( &m_trackCnv, transObj->m_extrapolatedTrack, log);
//  persObj->m_combinedTrack     = toPersistent( &m_cbTrackCnv, transObj->m_combinedTrack, log);

}
