/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMDATACOLLECTIONCNV_P2_H
#define MUONSIMDATACOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimDataCollection_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p2.h"

class MuonSimDataCollectionCnv_p2 : public T_AthenaPoolTPCnvBase <MuonSimDataCollection, Muon::MuonSimDataCollection_p2> {

   public:
     MuonSimDataCollectionCnv_p2() {};
     void persToTrans(const Muon::MuonSimDataCollection_p2* persCol, MuonSimDataCollection* transCol, MsgStream &log);
     void transToPers(const MuonSimDataCollection* transCol, Muon::MuonSimDataCollection_p2* persCol, MsgStream &log);

   private:
     MuonSimDataCnv_p2 m_muonsimdataCnv;

};

#endif
