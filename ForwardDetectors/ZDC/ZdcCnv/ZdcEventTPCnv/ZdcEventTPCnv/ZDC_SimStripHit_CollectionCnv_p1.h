/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SimStripHit_CollectionCnv_p1_H
#define ZDC_SimStripHit_CollectionCnv_p1_H

#include <vector>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_Collection_p1.h"

class MsgStream;

class ZDC_SimStripHit_CollectionCnv_p1: public T_AthenaPoolTPCnvBase<ZDC_SimStripHit_Collection, ZDC_SimStripHit_Collection_p1> {
  
public:
  
  ZDC_SimStripHit_CollectionCnv_p1() {}
  
  virtual void persToTrans(const ZDC_SimStripHit_Collection_p1* persObj, ZDC_SimStripHit_Collection* transObj, MsgStream& log);
  virtual void transToPers(const ZDC_SimStripHit_Collection*   transObj, ZDC_SimStripHit_Collection_p1* persObj, MsgStream& log);
};


template<>
class T_TPCnv<ZDC_SimStripHit_Collection, ZDC_SimStripHit_Collection_p1>
  : public ZDC_SimStripHit_CollectionCnv_p1
{
public:
};





/*
// First attempt
typedef T_AtlasHitsVectorCnv< ZDC_SimStripHit_Collection, ZDC_SimStripHit_Collection_p1, ZDC_SimStripHit_p1 > ZDC_SimStripHit_CollectionCnv_p1;
*/

#endif

