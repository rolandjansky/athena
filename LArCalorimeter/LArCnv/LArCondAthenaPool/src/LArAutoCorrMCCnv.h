/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArAutoCorrMCCnv.h
 * @brief AthenaPool converter LArAutoCorrMC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArAutoCorrMCCnv_H
#define LArAutoCorrMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArAutoCorrMC.h"
#undef private
#undef protected
#include "LArRawConditions/LArAutoCorrP.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

// the latest persistent representation type of LArAutoCorrMC
typedef LArConditionsSubset<LArAutoCorrP1> LArAutoCorrTransType;
typedef LArAutoCorrSubset_p1               LArAutoCorrPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArAutoCorrMC, LArAutoCorrTransType, LArAutoCorrPersType > LArAutoCorrMCCnvBase;

class LArAutoCorrMCCnv : public LArAutoCorrMCCnvBase {
friend class CnvFactory<LArAutoCorrMCCnv >;
protected:
    LArAutoCorrMCCnv (ISvcLocator* svcloc) : LArAutoCorrMCCnvBase(svcloc) {}
    virtual LArAutoCorrPersType*   createPersistent (LArAutoCorrTransType* transObj);
    virtual LArAutoCorrTransType*  createTransient ();
    LArAutoCorrTransType*          createTransient(LArConditionsSubset<LArAutoCorrP>* orig);
};


#endif

