/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LAROFCSUBSETCNV_P1_H
#define LARCONDTPCNV_LAROFCSUBSETCNV_P1_H

#include "LArCondTPCnv/LArOFCSubset_p1.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArOFCP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArOFCP1> LArOFCTransType;
typedef LArOFCSubset_p1               LArOFCPersType;

class LArOFCSubsetCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArOFCTransType, LArOFCPersType >
{
public:

    LArOFCSubsetCnv_p1() {}
    virtual void   persToTrans(const LArOFCPersType* persObj, LArOFCTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArOFCTransType* transObj, LArOFCPersType* persObj, MsgStream &log) ;

};

#endif
