/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_RECOTIMINGOBJCNV_P1_H
#define RECTPCNV_RECOTIMINGOBJCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/RecoTimingObj_p1.h"
#include "RecEvent/RecoTimingObj.h"

class MsgStream;

class RecoTimingObjCnv_p1 : public T_AthenaPoolTPCnvBase<RecoTimingObj, RecoTimingObj_p1>
{
 public:
  RecoTimingObjCnv_p1() {};
  virtual void persToTrans( const RecoTimingObj_p1    *persObj,
			    RecoTimingObj             *transObj,
			    MsgStream             &msg );
  
  virtual void transToPers( const RecoTimingObj       *transObj,
			    RecoTimingObj_p1          *persObj,
			    MsgStream             &msg );
};

template <>
class T_TPCnv<RecoTimingObj, RecoTimingObj_p1 >
  : public RecoTimingObjCnv_p1
{ };

#endif
