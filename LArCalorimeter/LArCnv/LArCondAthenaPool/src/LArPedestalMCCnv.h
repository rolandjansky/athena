/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArPedestalMCCnv.h
 * @brief AthenaPool converter LArPedestalMC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArPedestalMCCnv_H
#define LArPedestalMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LArRawConditions/LArPedestalMC.h"
#include "LArCondTPCnv/LArPedestalMC_p1.h"

// the latest persistent representation type of LArPedestalMC
typedef LArPedestalMC        LArPedestalMCTransType;
typedef LArPedestalMC_p1     LArPedestalMCPersType;

typedef T_AthenaPoolCustomCnv<LArPedestalMCTransType, LArPedestalMCPersType > LArPedestalMCCnvBase;

class LArPedestalMCCnv : public LArPedestalMCCnvBase {
friend class CnvFactory<LArPedestalMCCnv >;
protected:
    LArPedestalMCCnv (ISvcLocator* svcloc) : LArPedestalMCCnvBase(svcloc) {}
    virtual LArPedestalMCPersType*   createPersistent (LArPedestalMCTransType* transObj);
    virtual LArPedestalMCTransType*  createTransient ();
};

#endif

