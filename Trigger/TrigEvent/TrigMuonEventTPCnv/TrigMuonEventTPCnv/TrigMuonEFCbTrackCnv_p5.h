/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrackCnv_p5
 *
 * @brief transient-persistent converter for TrigMuonEFCbTrack_p4
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>  - U.Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P5_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p4.h"

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class MsgStream;


class TrigMuonEFCbTrackCnv_p5 : public T_AthenaPoolTPCnvBase<TrigMuonEFCbTrack, TrigMuonEFCbTrack_p4>
{

 public:

  TrigMuonEFCbTrackCnv_p5() : m_P4IPtCotThPhiMCnv(0), m_TrigMuonEFTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFCbTrack_p4* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p4* persObj, MsgStream &log);

 protected:
  P4IPtCotThPhiMCnv_p1      *m_P4IPtCotThPhiMCnv;
  TrigMuonEFTrackCnv_p5     *m_TrigMuonEFTrackCnv;

};


#endif
