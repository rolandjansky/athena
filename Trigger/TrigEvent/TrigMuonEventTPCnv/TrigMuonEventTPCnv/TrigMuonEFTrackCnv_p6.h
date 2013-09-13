/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFTrackCnv_p6
 *
 * @brief transient-persistent converter for TrigMuonEFTrack_p6
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACKCNV_P6_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACKCNV_P6_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p6.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

class MsgStream;

class TrigMuonEFTrackCnv_p6 : public T_AthenaPoolTPCnvBase<TrigMuonEFTrack, TrigMuonEFTrack_p6> {

 public:

  TrigMuonEFTrackCnv_p6() : m_P4IPtCotThPhiMCnv(0) {}

  virtual void persToTrans(const TrigMuonEFTrack_p6* persObj, TrigMuonEFTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFTrack* transObj, TrigMuonEFTrack_p6* persObj, MsgStream &log);

  protected:
  P4IPtCotThPhiMCnv_p1      *m_P4IPtCotThPhiMCnv;
};


#endif
