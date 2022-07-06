/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATACNV_P4_H
#define MUON_TGCRAWDATACNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/TgcRawData_p4.h"
#include "MuonRDO/TgcRawData.h"

class MsgStream;

class TgcRawDataCnv_p4 : public T_AthenaPoolTPCnvBase<TgcRawData, TgcRawData_p4>
{
public:
    TgcRawDataCnv_p4()
    {
    }

    virtual void persToTrans(const TgcRawData_p4* persObj, TgcRawData* transObj, MsgStream &) ;
    virtual void transToPers(const TgcRawData* transObj, TgcRawData_p4* persObj, MsgStream &) ;
};

#endif

