/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LARDSPTHRESHOLDSCNV_P1_H
#define LARCONDTPCNV_LARDSPTHRESHOLDSCNV_P1_H

#include "LArRawConditions/LArConditionsSubset.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArDSPThresholdsSubset_p1.h"
#include "LArRawConditions/LArDSPThresholdsP.h"

class MsgStream;

typedef LArConditionsSubset<LArDSPThresholdsP> DSPThresholdsTransType;
typedef LArDSPThresholdsSubset_p1              DSPThresholdsPersType;

class LArDSPThresholdsSubsetCnv_p1
    : public T_AthenaPoolTPCnvConstBase<DSPThresholdsTransType,DSPThresholdsPersType>
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArDSPThresholdsSubsetCnv_p1() {}
    virtual void   persToTrans(const DSPThresholdsPersType* persObj, DSPThresholdsTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const DSPThresholdsTransType* transObj, DSPThresholdsPersType* persObj, MsgStream &log) const override;
};
    
  

#endif
