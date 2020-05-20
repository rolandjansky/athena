/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSimHitCOLLECTIONCNV_P4_H
#define CSCSimHitCOLLECTIONCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "CSCSimHitCollection_p4.h"
// namespace Muon {
//     class CSCSimHitCollection_p1;
// }

class CSCSimHitCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<CSCSimHitCollection, Muon::CSCSimHitCollection_p4>
{
 public:
  
  CSCSimHitCollectionCnv_p4()  {};
  
  virtual CSCSimHitCollection* createTransient(const Muon::CSCSimHitCollection_p4* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::CSCSimHitCollection_p4* persCont,
                CSCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const CSCSimHitCollection* transCont,
                Muon::CSCSimHitCollection_p4* persCont,
                MsgStream &log) ;

};

#endif

