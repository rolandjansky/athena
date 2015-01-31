/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARPEDESTALMCCNV_P1_H
#define LARCONDTPCNV_LARPEDESTALMCCNV_P1_H

#include "LArCondTPCnv/LArPedestalMC_p1.h"
#include "LArRawConditions/LArPedestalMC.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArPedestalMC     LArPedMCTransType;
typedef LArPedestalMC_p1  LArPedMCPersType;

class LArPedestalMCCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArPedMCTransType, LArPedMCPersType >
{
public:

    LArPedestalMCCnv_p1() {}
    virtual void   persToTrans(const LArPedMCPersType* persObj, LArPedMCTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArPedMCTransType* transObj, LArPedMCPersType* persObj, MsgStream &log) ;

};

#endif
