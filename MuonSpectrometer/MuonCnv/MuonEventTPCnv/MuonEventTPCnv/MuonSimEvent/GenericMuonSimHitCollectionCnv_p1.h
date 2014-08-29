/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericMuonSimHitCOLLECTIONCNV_P1_H
#define GenericMuonSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "GenericMuonSimHitCollection_p1.h"
// namespace Muon {
//     class GenericMuonSimHitCollection_p1;
// }

class GenericMuonSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<GenericMuonSimHitCollection, Muon::GenericMuonSimHitCollection_p1>
{
 public:
  
  GenericMuonSimHitCollectionCnv_p1()  {};
  
  virtual GenericMuonSimHitCollection* createTransient(const Muon::GenericMuonSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::GenericMuonSimHitCollection_p1* persCont,
                GenericMuonSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const GenericMuonSimHitCollection* transCont,
                Muon::GenericMuonSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

