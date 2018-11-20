/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSimHitCOLLECTIONCNV_P2_H
#define sTGCSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "sTGCSimHitCollection_p2.h"

/**
 ** The _p2 converter was added to support HepMcParticleLink_p2 in 21.3.
 ** As of 2018-09-24, HepMcParticleLink_p2 is not fully supported in master.
 ** Therefore, the default is set to _p2, while the _p2 is provided
 ** in case of it is needed in the future.
 ** -chav.chhiv.Chau@cern.ch
 **/

class sTGCSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<sTGCSimHitCollection, Muon::sTGCSimHitCollection_p2>
{
 public:
  
  sTGCSimHitCollectionCnv_p2()  {};
  
  virtual sTGCSimHitCollection* createTransient(const Muon::sTGCSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::sTGCSimHitCollection_p2* persCont,
                sTGCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const sTGCSimHitCollection* transCont,
                Muon::sTGCSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

