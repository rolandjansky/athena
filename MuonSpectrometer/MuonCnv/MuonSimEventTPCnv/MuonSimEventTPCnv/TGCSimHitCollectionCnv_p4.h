/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCOLLECTIONCNV_P4_H
#define TGCSimHitCOLLECTIONCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TGCSimHitCollection_p4.h"

class TGCSimHitCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<TGCSimHitCollection, Muon::TGCSimHitCollection_p4>
{
 public:
  
  TGCSimHitCollectionCnv_p4()  {};
  
  virtual TGCSimHitCollection* createTransient(const Muon::TGCSimHitCollection_p4* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::TGCSimHitCollection_p4* persCont,
                TGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const TGCSimHitCollection* transCont,
                Muon::TGCSimHitCollection_p4* persCont,
                MsgStream &log) ;

};

#endif // TGCSimHitCOLLECTIONCNV_P4_H

