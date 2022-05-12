/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARcaliWAVEsubsetCNV_P1_H
#define LARCONDTPCNV_LARcaliWAVEsubsetCNV_P1_H

#include "LArCondTPCnv/LArCaliWaveSubset_p1.h"
#include "LArRawConditions/LArCaliWave.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArRawConditions/LArConditionsSubset.h"

class MsgStream;

typedef LArConditionsSubset<LArCaliWaveVec>   LArCWTransType;  
typedef LArCaliWaveSubset_p1                  LArCWPersType;

class LArCaliWaveSubsetCnv_p1  
    : public T_AthenaPoolTPCnvConstBase<LArCWTransType, LArCWPersType >
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArCaliWaveSubsetCnv_p1() {}
    virtual void   persToTrans(const LArCWPersType*   persObj, LArCWTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const LArCWTransType* transObj, LArCWPersType*   persObj, MsgStream &log) const override;
};

#endif
