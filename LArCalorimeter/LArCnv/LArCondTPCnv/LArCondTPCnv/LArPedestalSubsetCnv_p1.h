/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARPEDESTALSUBSETCNV_P1_H
#define LARCONDTPCNV_LARPEDESTALSUBSETCNV_P1_H

#include "LArCondTPCnv/LArPedestalSubset_p1.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArPedestalP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArPedestalP1> LArPedestalTransType;
typedef LArPedestalSubset_p1               LArPedestalPersType;

class LArPedestalSubsetCnv_p1  
    : public T_AthenaPoolTPCnvConstBase<LArPedestalTransType, LArPedestalPersType >
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArPedestalSubsetCnv_p1() {}
    virtual void   persToTrans(const LArPedestalPersType* persObj, LArPedestalTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const LArPedestalTransType* transObj, LArPedestalPersType* persObj, MsgStream &log) const override;

};

#endif
