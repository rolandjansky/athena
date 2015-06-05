/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSimHitCOLLECTIONCNV_P1_H
#define MDTSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MDTSimHitCollection_p1.h"
// namespace Muon {
//     class MDTSimHitCollection_p1;
// }

class MDTSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<MDTSimHitCollection, Muon::MDTSimHitCollection_p1>
{
 public:
  
  MDTSimHitCollectionCnv_p1()  {};
  
  virtual MDTSimHitCollection* createTransient(const Muon::MDTSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::MDTSimHitCollection_p1* persCont,
                MDTSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const MDTSimHitCollection* transCont,
                Muon::MDTSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

