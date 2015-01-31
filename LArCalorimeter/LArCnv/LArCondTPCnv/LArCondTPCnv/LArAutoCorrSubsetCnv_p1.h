/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARAUTOCORRSUBSETCNV_P1_H
#define LARCONDTPCNV_LARAUTOCORRSUBSETCNV_P1_H

#include "LArCondTPCnv/LArAutoCorrSubset_p1.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArAutoCorrP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArAutoCorrP1> LArAutoCorrTransType;
typedef LArAutoCorrSubset_p1               LArAutoCorrPersType;

class LArAutoCorrSubsetCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArAutoCorrTransType, LArAutoCorrPersType >
{
public:

    LArAutoCorrSubsetCnv_p1() {}
    virtual void   persToTrans(const LArAutoCorrPersType* persObj, LArAutoCorrTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArAutoCorrTransType* transObj, LArAutoCorrPersType* persObj, MsgStream &log) ;

};

#endif
