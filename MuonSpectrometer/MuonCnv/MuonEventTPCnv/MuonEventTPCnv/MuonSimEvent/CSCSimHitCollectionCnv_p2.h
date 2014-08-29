/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCOLLECTIONCNV_P2_H
#define CSCSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "CSCSimHitCollection_p2.h"
// namespace Muon {
//     class CSCSimHitCollection_p1;
// }

class CSCSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<CSCSimHitCollection, Muon::CSCSimHitCollection_p2>
{
 public:
  
  CSCSimHitCollectionCnv_p2()  {};
  
  virtual CSCSimHitCollection* createTransient(const Muon::CSCSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::CSCSimHitCollection_p2* persCont,
                CSCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const CSCSimHitCollection* transCont,
                Muon::CSCSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

