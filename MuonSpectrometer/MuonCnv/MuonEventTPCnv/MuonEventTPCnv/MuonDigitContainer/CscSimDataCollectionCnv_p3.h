/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTIONCNV_P3_H
#define CSCSIMDATACOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "CscSimDataCollection_p3.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p3.h"

class CscSimDataCollectionCnv_p3 : public T_AthenaPoolTPCnvBase <CscSimDataCollection, Muon::CscSimDataCollection_p3> {

   public:
     CscSimDataCollectionCnv_p3() {};
     void persToTrans(const Muon::CscSimDataCollection_p3* persCol, CscSimDataCollection* transCol, MsgStream &log);
     void transToPers(const CscSimDataCollection* transCol, Muon::CscSimDataCollection_p3* persCol, MsgStream &log);

   private:
     CscSimDataCnv_p3 m_cscsimdataCnv;

};

#endif
