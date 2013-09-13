/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEFInfo_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

class MsgStream;


class TrigMuonEFInfoCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMuonEFInfo, TrigMuonEFInfo_p1>
{

 public:

  TrigMuonEFInfoCnv_p1() : m_trackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFInfo_p1* persObj, TrigMuonEFInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p1* persObj, MsgStream &log);

 private:


 protected:

  ITPConverterFor<TrigMuonEFTrack> *m_trackCnv;

};


#endif
