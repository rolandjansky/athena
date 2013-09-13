/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"
#undef private
#undef protected

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p1.h"


void TrigMuonEFInfoCnv_p1::persToTrans(const TrigMuonEFInfo_p1* persObj, 
				       TrigMuonEFInfo* transObj, 
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p1::persToTrans called " << endreq;

  transObj->m_roi               = persObj->m_roi;
  delete transObj->m_spectrometerTrack;
  transObj->m_spectrometerTrack = createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log);
  delete transObj->m_extrapolatedTrack;
  transObj->m_extrapolatedTrack = createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log);
  delete transObj->m_combinedTrack;
  TrigMuonEFTrack* pers = createTransFromPStore( &m_trackCnv, persObj->m_combinedTrack, log);
  transObj->m_combinedTrack = new TrigMuonEFCbTrack(*pers);
  delete pers;

}


void TrigMuonEFInfoCnv_p1::transToPers(const TrigMuonEFInfo* /*transObj*/,
				       TrigMuonEFInfo_p1* /*persObj*/,
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p1::transToPers called " << endreq;
/*
  persObj->m_roi               = transObj->m_roi;
  persObj->m_spectrometerTrack = toPersistent( &m_trackCnv, transObj->m_spectrometerTrack, log);
  persObj->m_extrapolatedTrack = toPersistent( &m_trackCnv, transObj->m_extrapolatedTrack, log);
  persObj->m_combinedTrack     = toPersistent( &m_trackCnv, transObj->m_combinedTrack, log);
*/
}
