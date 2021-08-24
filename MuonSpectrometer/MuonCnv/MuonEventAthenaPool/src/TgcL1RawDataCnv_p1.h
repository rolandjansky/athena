/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RAWDATACNV_P1_H
#define MUON_TGCL1RAWDATACNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/TgcL1RawData_p1.h"
#include "MuonRDO/TgcL1RawData.h"

class MsgStream;

class TgcL1RawDataCnv_p1 : public T_AthenaPoolTPCnvBase<TgcL1RawData, TgcL1RawData_p1>
{
public:
    TgcL1RawDataCnv_p1()
    {
    }

    virtual void persToTrans(const TgcL1RawData_p1* persObj, TgcL1RawData* transObj, MsgStream &) ;
    virtual void transToPers(const TgcL1RawData* transObj, TgcL1RawData_p1* persObj, MsgStream &) ;
};

#endif

