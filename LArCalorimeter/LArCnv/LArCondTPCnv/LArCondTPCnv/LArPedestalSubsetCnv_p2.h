/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARPEDESTALSUBSETCNV_P2_H
#define LARCONDTPCNV_LARPEDESTALSUBSETCNV_P2_H

#include "LArCondTPCnv/LArPedestalSubset_p2.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArPedestalP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArPedestalP1> LArPedestalTransType;

class LArPedestalSubsetCnv_p2  
    : public T_AthenaPoolTPCnvBase<LArPedestalTransType, LArPedestalSubset_p2 >
{
public:

    LArPedestalSubsetCnv_p2() {}
    virtual void   persToTrans(const LArPedestalSubset_p2* persObj, LArPedestalTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArPedestalTransType* transObj, LArPedestalSubset_p2* persObj, MsgStream &log) ;

};

#endif
