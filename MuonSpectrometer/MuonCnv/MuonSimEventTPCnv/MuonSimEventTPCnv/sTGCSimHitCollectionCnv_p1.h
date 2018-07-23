/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSimHitCOLLECTIONCNV_P1_H
#define sTGCSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "sTGCSimHitCollection_p1.h"

class sTGCSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<sTGCSimHitCollection, Muon::sTGCSimHitCollection_p1>
{
 public:
  
  sTGCSimHitCollectionCnv_p1()  {};
  
  virtual sTGCSimHitCollection* createTransient(const Muon::sTGCSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::sTGCSimHitCollection_p1* persCont,
                sTGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const sTGCSimHitCollection* transCont,
                Muon::sTGCSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

