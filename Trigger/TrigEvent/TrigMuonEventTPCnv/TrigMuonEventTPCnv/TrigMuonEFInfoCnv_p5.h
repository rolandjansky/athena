/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoCnv_p5
 *
 * @brief transient-persistent converter for TrigMuonEFInfo_p3
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P5_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p5.h"


class MsgStream;


class TrigMuonEFInfoCnv_p5 : public T_AthenaPoolTPCnvBase<TrigMuonEFInfo, TrigMuonEFInfo_p5>
{

 public:

  TrigMuonEFInfoCnv_p5() :  m_trackContainerCnv(0) {} //m_trackCnv(0), m_cbTrackCnv(0),

  virtual void persToTrans(const TrigMuonEFInfo_p5* persObj, TrigMuonEFInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p5* persObj, MsgStream &log);

 private:


 protected:
  // 
  // ITPConverterFor<TrigMuonEFTrack> *m_trackCnv; // probably not needed
  // ITPConverterFor<TrigMuonEFCbTrack> *m_cbTrackCnv; // probably not needed
  ITPConverterFor<TrigMuonEFInfoTrackContainer> *m_trackContainerCnv;

};


#endif
