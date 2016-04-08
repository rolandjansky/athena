/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIRECTPCNV_HITRTINFOCNV_P1_H
#define HIRECTPCNV_HITRTINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HIRecTPCnv/HITRTInfo_p1.h"
#include "HIGlobal/HITRTInfo.h"

class MsgStream;

class HITRTInfoCnv_p1 : public T_AthenaPoolTPCnvBase<HITRTInfo, HITRTInfo_p1>
{
 public:
  HITRTInfoCnv_p1() {};
  virtual void persToTrans( const HITRTInfo_p1    *persObj,
                            HITRTInfo             *transObj,
                            MsgStream             &msg );
  
  virtual void transToPers( const HITRTInfo       *transObj,
                            HITRTInfo_p1          *persObj,
                            MsgStream             &msg );
};

template <>
class T_TPCnv<HITRTInfo, HITRTInfo_p1 >
  : public HITRTInfoCnv_p1
{ };

#endif
