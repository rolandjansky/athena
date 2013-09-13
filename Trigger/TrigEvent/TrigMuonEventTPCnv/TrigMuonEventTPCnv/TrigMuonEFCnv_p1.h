/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEF_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFCnv_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"


class MsgStream;


class TrigMuonEFCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMuonEF, TrigMuonEF_p1> 
{

 public:

  TrigMuonEFCnv_p1() : m_P4IPtCotThPhiMCnv(0) {}

  virtual void persToTrans(const TrigMuonEF_p1* persObj, TrigMuonEF* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEF* transObj, TrigMuonEF_p1* persObj, MsgStream &log);

  protected:
  P4IPtCotThPhiMCnv_p1      *m_P4IPtCotThPhiMCnv;
};


#endif
