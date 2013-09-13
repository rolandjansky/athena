/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonEFInfo_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p1.h"

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class MsgStream;


class TrigMuonEFInfoCnv_p2 : public T_AthenaPoolTPCnvBase<TrigMuonEFInfo, TrigMuonEFInfo_p2>
{

 public:

  TrigMuonEFInfoCnv_p2() : m_trackCnv(0), m_cbTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFInfo_p2* persObj, TrigMuonEFInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p2* persObj, MsgStream &log);

 private:


 protected:

  ITPConverterFor<TrigMuonEFTrack> *m_trackCnv;
  ITPConverterFor<TrigMuonEFCbTrack> *m_cbTrackCnv;

};


#endif
