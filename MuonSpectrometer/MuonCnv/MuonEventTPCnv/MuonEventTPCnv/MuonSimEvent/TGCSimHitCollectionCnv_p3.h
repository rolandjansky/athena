/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCOLLECTIONCNV_P3_H
#define TGCSimHitCOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TGCSimHitCollection_p3.h"

class TGCSimHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<TGCSimHitCollection, Muon::TGCSimHitCollection_p3>
{
 public:
  
  TGCSimHitCollectionCnv_p3()  {};
  
  virtual TGCSimHitCollection* createTransient(const Muon::TGCSimHitCollection_p3* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::TGCSimHitCollection_p3* persCont,
                TGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const TGCSimHitCollection* transCont,
                Muon::TGCSimHitCollection_p3* persCont,
                MsgStream &log) ;

};

#endif // TGCSimHitCOLLECTIONCNV_P3_H

