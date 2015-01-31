/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARRAMPSUBSETCNV_P1_H
#define LARCONDTPCNV_LARRAMPSUBSETCNV_P1_H

#include "LArCondTPCnv/LArRampSubset_p1.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArRampP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArRampP1> LArRampTransType;
typedef LArRampSubset_p1               LArRampPersType;

class LArRampSubsetCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArRampTransType, LArRampPersType >
{
public:

    LArRampSubsetCnv_p1() {}
    virtual void   persToTrans(const LArRampPersType* persObj, LArRampTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArRampTransType* transObj, LArRampPersType* persObj, MsgStream &log) ;

};

#endif
