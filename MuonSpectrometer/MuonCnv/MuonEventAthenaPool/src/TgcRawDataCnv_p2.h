/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATACNV_P2_H
#define MUON_TGCRAWDATACNV_P2_H

/*
Transient/Persistent converter for TgcRawData class
Author: Zvi Tarem
        CERN, August 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/TgcRawData_p2.h"
#include "MuonRDO/TgcRawData.h"

class MsgStream;

class TgcRawDataCnv_p2 : public T_AthenaPoolTPCnvBase<TgcRawData, TgcRawData_p2>
{
public:
    TgcRawDataCnv_p2()
    {
    }

    virtual void persToTrans(const TgcRawData_p2* persObj, TgcRawData* transObj, MsgStream &) ;
    virtual void transToPers(const TgcRawData* transObj, TgcRawData_p2* persObj, MsgStream &) ;
};

#endif

