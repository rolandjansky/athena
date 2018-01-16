/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCOLLECTIONCNV_P3_H
#define MDTSimHitCOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MDTSimHitCollection_p3.h"

class MDTSimHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<MDTSimHitCollection, Muon::MDTSimHitCollection_p3>
{
 public:
  
  MDTSimHitCollectionCnv_p3()  {};
  
  virtual MDTSimHitCollection* createTransient(const Muon::MDTSimHitCollection_p3* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MDTSimHitCollection_p3* persCont,
                MDTSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MDTSimHitCollection* transCont,
                Muon::MDTSimHitCollection_p3* persCont,
                MsgStream &log) ;

};

#endif

