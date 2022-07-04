/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SimPixelHit_CollectionCnv_p1_H
#define ZDC_SimPixelHit_CollectionCnv_p1_H

#include <vector>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_Collection_p1.h"

class MsgStream;

class ZDC_SimPixelHit_CollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ZDC_SimPixelHit_Collection, ZDC_SimPixelHit_Collection_p1> {
  
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  ZDC_SimPixelHit_CollectionCnv_p1() {}
  
  virtual void persToTrans(const ZDC_SimPixelHit_Collection_p1* persObj, ZDC_SimPixelHit_Collection* transObj, MsgStream& log) const override;
  virtual void transToPers(const ZDC_SimPixelHit_Collection*   transObj, ZDC_SimPixelHit_Collection_p1* persObj, MsgStream& log) const override;
};


template<>
class T_TPCnv<ZDC_SimPixelHit_Collection, ZDC_SimPixelHit_Collection_p1>
  : public ZDC_SimPixelHit_CollectionCnv_p1
{
public:
};


#endif

