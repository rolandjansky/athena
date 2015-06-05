/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCOLLECTIONCNV_P2_H
#define TGCSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TGCSimHitCollection_p2.h"

class TGCSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<TGCSimHitCollection, Muon::TGCSimHitCollection_p2>
{
 public:
  
  TGCSimHitCollectionCnv_p2()  {};
  
  virtual TGCSimHitCollection* createTransient(const Muon::TGCSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::TGCSimHitCollection_p2* persCont,
                TGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const TGCSimHitCollection* transCont,
                Muon::TGCSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif // TGCSimHitCOLLECTIONCNV_P2_H

