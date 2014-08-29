/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTIONCNV_P1_H
#define MUONSIMDATACOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimDataCollection_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p1.h"

class MuonSimDataCollectionCnv_p1 : public T_AthenaPoolTPCnvBase <MuonSimDataCollection, Muon::MuonSimDataCollection_p1> {

   public:
     MuonSimDataCollectionCnv_p1() {};
     void persToTrans(const Muon::MuonSimDataCollection_p1* persCol, MuonSimDataCollection* transCol, MsgStream &log);
     void transToPers(const MuonSimDataCollection* transCol, Muon::MuonSimDataCollection_p1* persCol, MsgStream &log);

   private:
     MuonSimDataCnv_p1 m_muonsimdataCnv;

};

#endif
