/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCOLLECTIONCNV_P2_H
#define MDTSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MDTSimHitCollection_p2.h"

class MDTSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<MDTSimHitCollection, Muon::MDTSimHitCollection_p2>
{
 public:
  
  MDTSimHitCollectionCnv_p2()  {};
  
  virtual MDTSimHitCollection* createTransient(const Muon::MDTSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MDTSimHitCollection_p2* persCont,
                MDTSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MDTSimHitCollection* transCont,
                Muon::MDTSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

