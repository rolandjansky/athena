/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p2.h"
#include "CxxUtils/make_unique.h"


void TrigMuonEFInfoCnv_p2::persToTrans(const TrigMuonEFInfo_p2* persObj,
				       TrigMuonEFInfo* transObj,
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p2::persToTrans called " << endreq;

  std::unique_ptr<TrigMuonEFTrack> spectrometerTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log));
  std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log));
  std::unique_ptr<TrigMuonEFCbTrack> combinedTrack
    (createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log));

  *transObj = TrigMuonEFInfo (persObj->m_roi,
                              CxxUtils::make_unique<TrigMuonEFInfoTrackContainer>(),
                              std::move (spectrometerTrack),
                              std::move (extrapolatedTrack),
                              std::move (combinedTrack));
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
