/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARphysWAVEsubsetCNV_P1_H
#define LARCONDTPCNV_LARphysWAVEsubsetCNV_P1_H

#include "LArCondTPCnv/LArPhysWaveSubset_p1.h"
#include "LArRawConditions/LArPhysWave.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArRawConditions/LArConditionsSubset.h"

class MsgStream;

typedef LArConditionsSubset<LArPhysWave>	LArPWTransType;  
typedef LArPhysWaveSubset_p1              	LArPWPersType;

class LArPhysWaveSubsetCnv_p1  
    : public T_AthenaPoolTPCnvConstBase<LArPWTransType, LArPWPersType >
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArPhysWaveSubsetCnv_p1() {}
    virtual void   persToTrans(const LArPWPersType*   persObj, LArPWTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const LArPWTransType* transObj, LArPWPersType*   persObj, MsgStream &log) const override;
};

#endif
