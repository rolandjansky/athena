/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTIONCNV_P3_H
#define MUONSIMDATACOLLECTIONCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimDataCollection_p3.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p3.h"

class MuonSimDataCollectionCnv_p3 : public T_AthenaPoolTPCnvBase <MuonSimDataCollection, Muon::MuonSimDataCollection_p3> {

   public:
     MuonSimDataCollectionCnv_p3() {};
     void persToTrans(const Muon::MuonSimDataCollection_p3* persCol, MuonSimDataCollection* transCol, MsgStream &log);
     void transToPers(const MuonSimDataCollection* transCol, Muon::MuonSimDataCollection_p3* persCol, MsgStream &log);

   private:
     MuonSimDataCnv_p3 m_muonsimdataCnv;

};

#endif
