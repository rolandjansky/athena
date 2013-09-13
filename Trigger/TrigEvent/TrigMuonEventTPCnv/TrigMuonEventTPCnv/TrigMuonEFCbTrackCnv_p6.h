/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrackCnv_p6
 *
 * @brief transient-persistent converter for TrigMuonEFCbTrack_p4
 *
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P6_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P6_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class MsgStream;

class TrigMuonEFCbTrackCnv_p6 : public T_AthenaPoolTPCnvBase<TrigMuonEFCbTrack, TrigMuonEFCbTrack_p6>
{

 public:

  TrigMuonEFCbTrackCnv_p6() :  m_TrigMuonEFTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFCbTrack_p6* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p6* persObj, MsgStream &log);

 protected:
  ITPConverterFor<TrigMuonEFTrack>     *m_TrigMuonEFTrackCnv;

};


#endif
