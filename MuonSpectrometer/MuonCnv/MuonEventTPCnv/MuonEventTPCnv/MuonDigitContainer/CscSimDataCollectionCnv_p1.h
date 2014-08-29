/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSIMDATACOLLECTIONCNV_P1_H
#define CSCSIMDATACOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "CscSimDataCollection_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p1.h"

class CscSimDataCollectionCnv_p1 : public T_AthenaPoolTPCnvBase <CscSimDataCollection, Muon::CscSimDataCollection_p1> {

   public:
     CscSimDataCollectionCnv_p1() {};
     void persToTrans(const Muon::CscSimDataCollection_p1* persCol, CscSimDataCollection* transCol, MsgStream &log);
     void transToPers(const CscSimDataCollection* transCol, Muon::CscSimDataCollection_p1* persCol, MsgStream &log);

   private:
     CscSimDataCnv_p1 m_cscsimdataCnv;

};

#endif
