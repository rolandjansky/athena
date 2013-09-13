/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTrackCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEFInfoTrack_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"

#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

class MsgStream;


class TrigMuonEFInfoTrackCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMuonEFInfoTrack, TrigMuonEFInfoTrack_p1>
{

 public:

  TrigMuonEFInfoTrackCnv_p1() : m_trackCnv(0), m_cbTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFInfoTrack_p1* persObj, TrigMuonEFInfoTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFInfoTrack* transObj, TrigMuonEFInfoTrack_p1* persObj, MsgStream &log);

 private:


 protected:

  ITPConverterFor<TrigMuonEFTrack> *m_trackCnv;
  ITPConverterFor<TrigMuonEFCbTrack> *m_cbTrackCnv;

};


#endif
