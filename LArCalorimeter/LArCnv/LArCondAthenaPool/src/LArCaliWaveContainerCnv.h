/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArCaliWaveContainerCnv.h
 * @brief AthenaPool converter LArCaliWaveContainer
 * @author Ilija Vukotic <ivukotic@cern.ch>
 */

#ifndef LArCaliWaveContainerCnv_H
#define LArCaliWaveContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArCaliWaveContainer.h"
#undef private
#undef protected
#include "LArRawConditions/LArCaliWave.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"

typedef LArConditionsSubset<LArCaliWaveVec>    LArCaliWaveTransType;
typedef LArCaliWaveSubset_p2                   LArCaliWavePersType;

typedef T_AthenaPoolCoolMultChanCnv<LArCaliWaveContainer, LArCaliWaveTransType, LArCaliWavePersType > LArCaliWaveContainerCnvBase;

class LArCaliWaveContainerCnv : public LArCaliWaveContainerCnvBase {
friend class CnvFactory<LArCaliWaveContainerCnv >;

protected:
    LArCaliWaveContainerCnv (ISvcLocator* svcloc) : LArCaliWaveContainerCnvBase(svcloc) {}
    LArCaliWavePersType*   createPersistent (LArCaliWaveTransType* transObj);
    LArCaliWaveTransType*  createTransient ();
    LArCaliWaveTransType*  createTransient(LArCaliWaveTransType* orig); //used to read non t/p split version
};
#endif
