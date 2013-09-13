/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonEF_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFCnv_p2.h 363411 2011-05-03 15:04:50Z salvator $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p2.h"


class MsgStream;


class TrigMuonEFCnv_p2 : public T_AthenaPoolTPCnvBase<TrigMuonEF, TrigMuonEF_p2> {

 public:
  TrigMuonEFCnv_p2() {}

  virtual void persToTrans(const TrigMuonEF_p2* persObj, TrigMuonEF* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEF* transObj, TrigMuonEF_p2* persObj, MsgStream &log);

  protected:
  P4IPtCotThPhiMCnv_p1 m_P4IPtCotThPhiMCnv;
};


#endif
