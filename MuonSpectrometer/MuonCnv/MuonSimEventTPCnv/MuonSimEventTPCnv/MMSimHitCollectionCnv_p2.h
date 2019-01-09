/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSimHitCOLLECTIONCNV_P2_H
#define MMSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MMSimHitCollection_p2.h"

class MMSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<MMSimHitCollection, Muon::MMSimHitCollection_p2>
{
 public:
  
  MMSimHitCollectionCnv_p2()  {};
  
  virtual MMSimHitCollection* createTransient(const Muon::MMSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MMSimHitCollection_p2* persCont,
                MMSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MMSimHitCollection* transCont,
                Muon::MMSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

