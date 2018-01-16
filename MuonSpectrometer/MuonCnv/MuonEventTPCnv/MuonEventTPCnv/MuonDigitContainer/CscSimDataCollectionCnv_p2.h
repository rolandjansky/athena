/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTIONCNV_P2_H
#define CSCSIMDATACOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "CscSimDataCollection_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p2.h"

class CscSimDataCollectionCnv_p2 : public T_AthenaPoolTPCnvBase <CscSimDataCollection, Muon::CscSimDataCollection_p2> {

   public:
     CscSimDataCollectionCnv_p2() {};
     void persToTrans(const Muon::CscSimDataCollection_p2* persCol, CscSimDataCollection* transCol, MsgStream &log);
     void transToPers(const CscSimDataCollection* transCol, Muon::CscSimDataCollection_p2* persCol, MsgStream &log);

   private:
     CscSimDataCnv_p2 m_cscsimdataCnv;

};

#endif
