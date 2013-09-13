/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoCnv_p4
 *
 * @brief transient-persistent converter for TrigMuonEFInfo_p3
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P4_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p4.h"


class MsgStream;


class TrigMuonEFInfoCnv_p4 : public T_AthenaPoolTPCnvBase<TrigMuonEFInfo, TrigMuonEFInfo_p4>
{

 public:

  TrigMuonEFInfoCnv_p4() : m_trackCnv(0), m_cbTrackCnv(0), m_trackContainerCnv(0) {}

  virtual void persToTrans(const TrigMuonEFInfo_p4* persObj, TrigMuonEFInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p4* persObj, MsgStream &log);

 private:


 protected:

  ITPConverterFor<TrigMuonEFTrack> *m_trackCnv;
  ITPConverterFor<TrigMuonEFCbTrack> *m_cbTrackCnv;
  ITPConverterFor<TrigMuonEFInfoTrackContainer> *m_trackContainerCnv;

};


#endif
