/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARcaliWAVEsubsetCNV_P2_H
#define LARCONDTPCNV_LARcaliWAVEsubsetCNV_P2_H

#include "LArCondTPCnv/LArCaliWaveSubset_p2.h"
#include "LArRawConditions/LArCaliWave.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArRawConditions/LArConditionsSubset.h"

class MsgStream;

typedef LArConditionsSubset<LArCaliWaveVec>   LArCWTransType;  

class LArCaliWaveSubsetCnv_p2  
    : public T_AthenaPoolTPCnvBase<LArCWTransType, LArCaliWaveSubset_p2 >
{
public:

    LArCaliWaveSubsetCnv_p2() {}
    virtual void   persToTrans(const LArCaliWaveSubset_p2*   persObj, LArCWTransType* transObj, MsgStream &log);
    virtual void   transToPers(const LArCWTransType* transObj, LArCaliWaveSubset_p2*   persObj, MsgStream &log);

};

#endif
