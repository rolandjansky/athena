/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMSTRIPHITCNV_P1_H
#define ZDC_SIMSTRIPHITCNV_P1_H

#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class ZDC_SimStripHitCnv_p1: public T_AthenaPoolTPCnvBase<ZDC_SimStripHit, ZDC_SimStripHit_p1> {

 public:
  
  ZDC_SimStripHitCnv_p1() {}
  
  virtual void persToTrans(const ZDC_SimStripHit_p1* persObj, ZDC_SimStripHit*	 transObj, MsgStream& log);
  virtual void transToPers(const ZDC_SimStripHit*   transObj, ZDC_SimStripHit_p1* persObj, MsgStream& log);


};


#endif
