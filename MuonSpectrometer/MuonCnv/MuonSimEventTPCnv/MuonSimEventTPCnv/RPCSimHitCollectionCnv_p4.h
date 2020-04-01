/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitCOLLECTIONCNV_P4_H
#define RPCSimHitCOLLECTIONCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "RPCSimHitCollection_p4.h"
// namespace Muon {
//     class RPCSimHitCollection_p1;
// }

class RPCSimHitCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<RPCSimHitCollection, Muon::RPCSimHitCollection_p4>
{
 public:
  
  RPCSimHitCollectionCnv_p4()  {};
  
  virtual RPCSimHitCollection* createTransient(const Muon::RPCSimHitCollection_p4* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::RPCSimHitCollection_p4* persCont,
                RPCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const RPCSimHitCollection* transCont,
                Muon::RPCSimHitCollection_p4* persCont,
                MsgStream &log) ;

};

#endif

