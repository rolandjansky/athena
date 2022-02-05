/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSimHitCOLLECTIONCNV_P3_H
#define sTGCSimHitCOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "sTGCSimHitCollection_p3.h"

class sTGCSimHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<sTGCSimHitCollection, Muon::sTGCSimHitCollection_p3>
{
 public:
  
  sTGCSimHitCollectionCnv_p3()  {};
  
  virtual sTGCSimHitCollection* createTransient(const Muon::sTGCSimHitCollection_p3* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::sTGCSimHitCollection_p3* persCont,
                sTGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const sTGCSimHitCollection* transCont,
                Muon::sTGCSimHitCollection_p3* persCont,
                MsgStream &log) ;

};

#endif

