/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LARSIMPLESUBSETCNV_P1_H
#define LARCONDTPCNV_LARSIMPLESUBSETCNV_P1_H

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArMphysOverMcalSubset_p1.h"
#include "LArRawConditions/LArMphysOverMcalP1.h"
#include "LArRawConditions/LArSingleFloatP.h"

class MsgStream;

typedef LArConditionsSubset<LArSingleFloatP> LArMpMcTransType;
typedef LArMphysOverMcalSubset_p1         LArMpMcPersType;

class LArMphysOverMcalSubsetCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArMpMcTransType,LArMpMcPersType >
{
public:
    LArMphysOverMcalSubsetCnv_p1() {}
    virtual void   persToTrans(const LArMpMcPersType* persObj, LArMpMcTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArMpMcTransType* transObj, LArMpMcPersType* persObj, MsgStream &log) ;
};


#endif
