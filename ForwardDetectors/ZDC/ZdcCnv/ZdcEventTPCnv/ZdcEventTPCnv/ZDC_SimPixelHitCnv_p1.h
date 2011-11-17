/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMPIXELHITCNV_P1_H
#define ZDC_SIMPIXELHITCNV_P1_H

#include "ZDC_SimEvent/ZDC_SimPixelHit.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class ZDC_SimPixelHitCnv_p1: public T_AthenaPoolTPCnvBase<ZDC_SimPixelHit, ZDC_SimPixelHit_p1> {

 public:
  
  ZDC_SimPixelHitCnv_p1() {}
  
  virtual void persToTrans(const ZDC_SimPixelHit_p1* persObj, ZDC_SimPixelHit*	 transObj, MsgStream& log);
  virtual void transToPers(const ZDC_SimPixelHit*   transObj, ZDC_SimPixelHit_p1* persObj, MsgStream& log);


};


#endif
