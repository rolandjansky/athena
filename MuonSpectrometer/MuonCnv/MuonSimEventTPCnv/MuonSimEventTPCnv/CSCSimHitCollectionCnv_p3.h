/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCOLLECTIONCNV_P3_H
#define CSCSimHitCOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "CSCSimHitCollection_p3.h"
// namespace Muon {
//     class CSCSimHitCollection_p1;
// }

class CSCSimHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<CSCSimHitCollection, Muon::CSCSimHitCollection_p3>
{
 public:
  
  CSCSimHitCollectionCnv_p3()  {};
  
  virtual CSCSimHitCollection* createTransient(const Muon::CSCSimHitCollection_p3* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::CSCSimHitCollection_p3* persCont,
                CSCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const CSCSimHitCollection* transCont,
                Muon::CSCSimHitCollection_p3* persCont,
                MsgStream &log) ;

};

#endif

