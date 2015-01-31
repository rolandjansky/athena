/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LARSINGLEFLOATSUBSETCNV_P1_H
#define LARCONDTPCNV_LARSINGLEFLOATSUBSETCNV_P1_H

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArSingleFloatSubset_p1.h"
#include "LArRawConditions/LArSingleFloatP.h"

class MsgStream;

typedef LArConditionsSubset<LArSingleFloatP> SingleFloatTransType;
typedef LArSingleFloatSubset_p1              SingleFloatPersType;

class LArSingleFloatSubsetCnv_p1
    : public T_AthenaPoolTPCnvBase<SingleFloatTransType,SingleFloatPersType>
{
public:

    LArSingleFloatSubsetCnv_p1() {}
    virtual void   persToTrans(const SingleFloatPersType* persObj, SingleFloatTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const SingleFloatTransType* transObj, SingleFloatPersType* persObj, MsgStream &log) ;
};
    
  

#endif
