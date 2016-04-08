/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p1.h"
#include "CxxUtils/make_unique.h"


void TrigMuonEFInfoCnv_p1::persToTrans(const TrigMuonEFInfo_p1* persObj, 
                                       TrigMuonEFInfo* transObj, 
				       MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFInfoCnv_p1::persToTrans called " << endreq;

  std::unique_ptr<TrigMuonEFTrack> spectrometerTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log));
  std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log));
  std::unique_ptr<TrigMuonEFTrack> tmp
    (createTransFromPStore( &m_trackCnv, persObj->m_combinedTrack, log));

  *transObj = TrigMuonEFInfo (persObj->m_roi,
                              CxxUtils::make_unique<TrigMuonEFInfoTrackContainer>(),
                              std::move (spectrometerTrack),
                              std::move (extrapolatedTrack),
                              CxxUtils::make_unique<TrigMuonEFCbTrack>(*tmp));
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
