/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCOLLECTIONCNV_P1_H
#define CSCSimHitCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "CSCSimHitCollection_p1.h"
// namespace Muon {
//     class CSCSimHitCollection_p1;
// }

class CSCSimHitCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<CSCSimHitCollection, Muon::CSCSimHitCollection_p1>
{
 public:
  
  CSCSimHitCollectionCnv_p1()  {};
  
  virtual CSCSimHitCollection* createTransient(const Muon::CSCSimHitCollection_p1* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::CSCSimHitCollection_p1* persCont,
                CSCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const CSCSimHitCollection* transCont,
                Muon::CSCSimHitCollection_p1* persCont,
                MsgStream &log) ;

};

#endif

