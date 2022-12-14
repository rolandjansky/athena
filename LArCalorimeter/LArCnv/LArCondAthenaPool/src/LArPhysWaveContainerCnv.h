/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArPhysWaveContainerCnv.h
 * @brief AthenaPool converter LArPhysWaveContainer
 * @author Ilija Vukotic <ivukotic@cern.ch>
 */

#ifndef LArPhysWaveContainerCnv_H
#define LArPhysWaveContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArPhysWaveContainer.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h"

typedef LArConditionsSubset<LArPhysWave>    	LArPhysWaveTransType;
typedef LArPhysWaveSubset_p1                  	LArPhysWavePersType;

typedef T_AthenaPoolCoolMultChanCnv<LArPhysWaveContainer, LArPhysWaveTransType, LArPhysWavePersType > LArPhysWaveContainerCnvBase;

class LArPhysWaveContainerCnv : public LArPhysWaveContainerCnvBase {
friend class CnvFactory<LArPhysWaveContainerCnv>;

protected:
public:
    LArPhysWaveContainerCnv (ISvcLocator* svcloc) : LArPhysWaveContainerCnvBase(svcloc) {}
protected:
    LArPhysWavePersType*   createPersistent (LArPhysWaveTransType* transObj);
    LArPhysWaveTransType*  createTransient ();
    LArPhysWaveTransType*  createTransient(LArPhysWaveTransType* orig); //used to read non t/p split version
};
#endif
