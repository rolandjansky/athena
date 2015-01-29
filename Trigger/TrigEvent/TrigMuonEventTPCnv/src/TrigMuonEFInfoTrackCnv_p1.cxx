/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#undef private
#undef protected

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackCnv_p1.h"


void TrigMuonEFInfoTrackCnv_p1::persToTrans(const TrigMuonEFInfoTrack_p1* persObj, TrigMuonEFInfoTrack* transObj, MsgStream &log){

  delete transObj->m_spectrometerTrack;
  transObj->m_spectrometerTrack = createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log);
  delete transObj->m_extrapolatedTrack;
  transObj->m_extrapolatedTrack = createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log);
  delete transObj->m_combinedTrack;
  transObj->m_combinedTrack     = createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log);
  transObj->m_muonType          = persObj->m_muonType;
}


void TrigMuonEFInfoTrackCnv_p1::transToPers(const TrigMuonEFInfoTrack* transObj, TrigMuonEFInfoTrack_p1* persObj, MsgStream &log) {
    // std::cout<<"--- InfoTrackCnv_p1 writing"<<std::endl;
  persObj->m_spectrometerTrack = toPersistent( &m_trackCnv, transObj->m_spectrometerTrack, log);
  persObj->m_extrapolatedTrack = toPersistent( &m_trackCnv, transObj->m_extrapolatedTrack, log);
    // std::cout<<"---InfoTrackCnv_p1 before cb "<<std::endl;
  persObj->m_combinedTrack     = toPersistent( &m_cbTrackCnv, transObj->m_combinedTrack, log);
    // std::cout<<"---InfoTrackCnv_p1 after cb "<<std::endl;
  persObj->m_muonType          = transObj->m_muonType;

}
