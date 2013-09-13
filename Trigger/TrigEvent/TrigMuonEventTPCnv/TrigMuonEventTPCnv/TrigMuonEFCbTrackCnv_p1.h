/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrackCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEFCbTrack_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p1.h"

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class MsgStream;


class TrigMuonEFCbTrackCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMuonEFCbTrack, TrigMuonEFCbTrack_p1> 
{

 public:

  TrigMuonEFCbTrackCnv_p1() : m_P4IPtCotThPhiMCnv(0), m_TrigMuonEFTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFCbTrack_p1* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p1* persObj, MsgStream &log);

 protected:
  P4IPtCotThPhiMCnv_p1      *m_P4IPtCotThPhiMCnv;
  TrigMuonEFTrackCnv_p1     *m_TrigMuonEFTrackCnv;

};


#endif
