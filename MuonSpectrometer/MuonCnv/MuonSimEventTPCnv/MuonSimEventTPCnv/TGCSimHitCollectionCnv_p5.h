/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSimHitCOLLECTIONCNV_P5_H
#define TGCSimHitCOLLECTIONCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TGCSimHitCollection_p5.h"

class TGCSimHitCollectionCnv_p5 : public T_AthenaPoolTPCnvBase<TGCSimHitCollection, Muon::TGCSimHitCollection_p5>
{
 public:
  
  TGCSimHitCollectionCnv_p5()  {};
  
  virtual TGCSimHitCollection* createTransient(const Muon::TGCSimHitCollection_p5* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::TGCSimHitCollection_p5* persCont,
                TGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const TGCSimHitCollection* transCont,
                Muon::TGCSimHitCollection_p5* persCont,
                MsgStream &log) ;

};

#endif // TGCSimHitCOLLECTIONCNV_P5_H

