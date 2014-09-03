/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATACNV_P1_H
#define MUON_TGCRAWDATACNV_P1_H

/*
Transient/Persistent converter for TgcRawData class
Author: Marcin Nowak
        CERN, February 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/TgcRawData_p1.h"
#include "MuonRDO/TgcRawData.h"

class MsgStream;

class TgcRawDataCnv_p1 : public T_AthenaPoolTPCnvBase<TgcRawData, TgcRawData_p1>
{
public:
    TgcRawDataCnv_p1()
    {
    }

    virtual void persToTrans(const TgcRawData_p1* persObj, TgcRawData* transObj, MsgStream &) ;
    virtual void transToPers(const TgcRawData* transObj, TgcRawData_p1* persObj, MsgStream &) ;
};

#endif

