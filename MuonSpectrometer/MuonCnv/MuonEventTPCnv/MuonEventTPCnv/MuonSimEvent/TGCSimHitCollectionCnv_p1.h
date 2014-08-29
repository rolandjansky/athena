/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCOLLECTIONCNV_P1_H
#define TGCSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TGCSimHitCollection_p1.h"
// namespace Muon {
//     class TGCSimHitCollection_p1;
// }

class TGCSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<TGCSimHitCollection, Muon::TGCSimHitCollection_p1>
{
 public:
  
  TGCSimHitCollectionCnv_p1()  {};
  
  virtual TGCSimHitCollection* createTransient(const Muon::TGCSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::TGCSimHitCollection_p1* persCont,
                TGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const TGCSimHitCollection* transCont,
                Muon::TGCSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

