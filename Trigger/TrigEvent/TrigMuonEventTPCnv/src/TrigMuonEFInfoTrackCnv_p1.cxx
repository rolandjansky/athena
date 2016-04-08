/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackCnv_p1.h"


void TrigMuonEFInfoTrackCnv_p1::persToTrans(const TrigMuonEFInfoTrack_p1* persObj, TrigMuonEFInfoTrack* transObj, MsgStream &log){

  std::unique_ptr<TrigMuonEFTrack> spectrometerTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log));
  std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log));
  std::unique_ptr<TrigMuonEFCbTrack> combinedTrack
    (createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log));

  transObj->setSpectrometerTrack (std::move (spectrometerTrack));
  transObj->setExtrapolatedTrack (std::move (extrapolatedTrack));
  transObj->setCombinedTrack (std::move (combinedTrack));
  transObj->setMuonType (persObj->m_muonType);
}


void TrigMuonEFInfoTrackCnv_p1::transToPers(const TrigMuonEFInfoTrack* transObj, TrigMuonEFInfoTrack_p1* persObj, MsgStream &log) {
    // std::cout<<"--- InfoTrackCnv_p1 writing"<<std::endl;
  persObj->m_spectrometerTrack = toPersistent( &m_trackCnv, transObj->SpectrometerTrack(), log);
  persObj->m_extrapolatedTrack = toPersistent( &m_trackCnv, transObj->ExtrapolatedTrack(), log);
    // std::cout<<"---InfoTrackCnv_p1 before cb "<<std::endl;
  persObj->m_combinedTrack     = toPersistent( &m_cbTrackCnv, transObj->CombinedTrack(), log);
    // std::cout<<"---InfoTrackCnv_p1 after cb "<<std::endl;
  persObj->m_muonType          = transObj->MuonType();

}
