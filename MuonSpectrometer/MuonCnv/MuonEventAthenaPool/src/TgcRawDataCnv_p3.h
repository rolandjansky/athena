/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATACNV_P3_H
#define MUON_TGCRAWDATACNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/TgcRawData_p3.h"
#include "MuonRDO/TgcRawData.h"

class MsgStream;

class TgcRawDataCnv_p3 : public T_AthenaPoolTPCnvBase<TgcRawData, TgcRawData_p3>
{
public:
    TgcRawDataCnv_p3()
    {
    }

    virtual void persToTrans(const TgcRawData_p3* persObj, TgcRawData* transObj, MsgStream &) ;
    virtual void transToPers(const TgcRawData* transObj, TgcRawData_p3* persObj, MsgStream &) ;
};

#endif

