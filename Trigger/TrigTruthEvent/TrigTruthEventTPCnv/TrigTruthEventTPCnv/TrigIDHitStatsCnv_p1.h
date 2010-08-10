/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigIDHitStatsCnv_p1
 *
 * @brief transient-persistent converter for TrigIDHitStats
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigIDHitStatsCnv_p1.h,v 1.1 2008-04-04 14:46:41 ahamil Exp $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGIDHITSTATSCNV_P1_H
#define TRIGEVENTTPCNV_TRIGIDHITSTATSCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetTruthEvent/TrigIDHitStats.h"
#include "TrigTruthEventTPCnv/TrigIDHitStats_p1.h"


class MsgStream;


class TrigIDHitStatsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigIDHitStats, TrigIDHitStats_p1> 
{

 public:

  TrigIDHitStatsCnv_p1() {}

  virtual void persToTrans(const TrigIDHitStats_p1* persObj, TrigIDHitStats* transObj, MsgStream &log);
  virtual void transToPers(const TrigIDHitStats* transObj, TrigIDHitStats_p1* persObj, MsgStream &log);
};


#endif
