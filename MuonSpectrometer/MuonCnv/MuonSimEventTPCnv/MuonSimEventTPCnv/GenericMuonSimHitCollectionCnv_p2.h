/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericMuonSimHitCOLLECTIONCNV_P2_H
#define GenericMuonSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "GenericMuonSimHitCollection_p2.h"
// namespace Muon {
//     class GenericMuonSimHitCollection_p2;
// }

class GenericMuonSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<GenericMuonSimHitCollection, Muon::GenericMuonSimHitCollection_p2>
{
 public:
  
  GenericMuonSimHitCollectionCnv_p2()  {};
  
  virtual GenericMuonSimHitCollection* createTransient(const Muon::GenericMuonSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::GenericMuonSimHitCollection_p2* persCont,
                GenericMuonSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const GenericMuonSimHitCollection* transCont,
                Muon::GenericMuonSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

