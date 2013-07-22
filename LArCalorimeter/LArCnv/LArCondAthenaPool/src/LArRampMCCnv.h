/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArRampMCCnv.h
 * @brief AthenaPool converter LArRampMC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArRampMCCnv_H
#define LArRampMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArRampMC.h"
#undef private
#undef protected
#include "LArRawConditions/LArRampP.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"

// the latest persistent representation type of LArRampMC
typedef LArConditionsSubset<LArRampP1> LArRampTransType;
typedef LArRampSubset_p1               LArRampPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArRampMC, LArRampTransType, LArRampPersType > LArRampMCCnvBase;

class LArRampMCCnv : public LArRampMCCnvBase {
friend class CnvFactory<LArRampMCCnv >;
protected:
    LArRampMCCnv (ISvcLocator* svcloc) : LArRampMCCnvBase(svcloc) {}
    virtual LArRampPersType*   createPersistent (LArRampTransType* transObj);
    virtual LArRampTransType*  createTransient ();
    LArRampTransType*          createTransient(LArConditionsSubset<LArRampP>* orig);
};


#endif

