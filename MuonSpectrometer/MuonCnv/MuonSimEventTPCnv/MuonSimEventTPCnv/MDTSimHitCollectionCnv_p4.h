/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCOLLECTIONCNV_P4_H
#define MDTSimHitCOLLECTIONCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MDTSimHitCollection_p4.h"

class MDTSimHitCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<MDTSimHitCollection, Muon::MDTSimHitCollection_p4>
{
 public:
  
  MDTSimHitCollectionCnv_p4()  {};
  
  virtual MDTSimHitCollection* createTransient(const Muon::MDTSimHitCollection_p4* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MDTSimHitCollection_p4* persCont,
                MDTSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MDTSimHitCollection* transCont,
                Muon::MDTSimHitCollection_p4* persCont,
                MsgStream &log) ;

};

#endif

