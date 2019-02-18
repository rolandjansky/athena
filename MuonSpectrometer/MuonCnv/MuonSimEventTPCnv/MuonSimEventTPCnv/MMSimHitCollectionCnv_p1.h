/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSimHitCOLLECTIONCNV_P1_H
#define MMSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MMSimHitCollection_p1.h"

class MMSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<MMSimHitCollection, Muon::MMSimHitCollection_p1>
{
 public:
  
  MMSimHitCollectionCnv_p1()  {};
  
  virtual MMSimHitCollection* createTransient(const Muon::MMSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MMSimHitCollection_p1* persCont,
                MMSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MMSimHitCollection* transCont,
                Muon::MMSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

