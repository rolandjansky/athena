/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMCDATACNV_P1_H
#define MUONMCDATACNV_P1_H

#include "MuonSimData/MuonSimData.h"
#include "MuonMCData_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class MuonMCDataCnv_p1 : public T_AthenaPoolTPCnvBase<MuonMCData, Muon::MuonMCData_p1> {

  public:
    MuonMCDataCnv_p1() {};
    void persToTrans( const Muon::MuonMCData_p1 * persObj, MuonMCData * transObj, MsgStream & log);
    void transToPers( const MuonMCData * transObj, Muon::MuonMCData_p1 * persObj, MsgStream & log);

};

#endif
