/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LARSIMPLESUBSETCNV_P1_H
#define LARCONDTPCNV_LARSIMPLESUBSETCNV_P1_H

#include "LArRawConditions/LArConditionsSubset.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArMphysOverMcalSubset_p1.h"
#include "LArRawConditions/LArMphysOverMcalP1.h"
#include "LArRawConditions/LArSingleFloatP.h"

class MsgStream;

typedef LArConditionsSubset<LArSingleFloatP> LArMpMcTransType;
typedef LArMphysOverMcalSubset_p1         LArMpMcPersType;

class LArMphysOverMcalSubsetCnv_p1  
    : public T_AthenaPoolTPCnvConstBase<LArMpMcTransType,LArMpMcPersType >
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArMphysOverMcalSubsetCnv_p1() {}
    virtual void   persToTrans(const LArMpMcPersType* persObj, LArMpMcTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const LArMpMcTransType* transObj, LArMpMcPersType* persObj, MsgStream &log) const override;
};


#endif
