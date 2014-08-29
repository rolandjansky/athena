/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCMCDATACNV_P1_H
#define CSCMCDATACNV_P1_H

#include "MuonSimData/CscSimData.h"
#include "CscMcData_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class CscMcDataCnv_p1 : public T_AthenaPoolTPCnvBase<CscMcData, Muon::CscMcData_p1> {

  public:
    CscMcDataCnv_p1() {};
    void persToTrans( const Muon::CscMcData_p1 * persObj, CscMcData * transObj, MsgStream & log);
    void transToPers( const CscMcData * transObj, Muon::CscMcData_p1 * persObj, MsgStream & log);

};

#endif
