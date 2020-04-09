/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSimHitCOLLECTIONCNV_P3_H
#define MMSimHitCOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MMSimHitCollection_p3.h"

class MMSimHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<MMSimHitCollection, Muon::MMSimHitCollection_p3>
{
 public:
  
  MMSimHitCollectionCnv_p3()  {};
  
  virtual MMSimHitCollection* createTransient(const Muon::MMSimHitCollection_p3* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MMSimHitCollection_p3* persCont,
                MMSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MMSimHitCollection* transCont,
                Muon::MMSimHitCollection_p3* persCont,
                MsgStream &log) ;

};

#endif

