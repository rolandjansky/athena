/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArAutoCorrCompleteCnv.h
 * @brief AthenaPool converter LArAutoCorrComplete
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArAutoCorrCompleteCnv_H
#define LArAutoCorrCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArRawConditions/LArAutoCorrP.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

// the latest persistent representation type of LArAutoCorrComplete
typedef LArConditionsSubset<LArAutoCorrP1> LArAutoCorrTransType;
typedef LArAutoCorrSubset_p1               LArAutoCorrPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArAutoCorrComplete, LArAutoCorrTransType, LArAutoCorrPersType > LArAutoCorrCompleteCnvBase;

class LArAutoCorrCompleteCnv : public LArAutoCorrCompleteCnvBase {
friend class CnvFactory<LArAutoCorrCompleteCnv >;
protected:
public:
    LArAutoCorrCompleteCnv (ISvcLocator* svcloc) : LArAutoCorrCompleteCnvBase(svcloc) {}
protected:
    virtual LArAutoCorrPersType*   createPersistent (LArAutoCorrTransType* transObj);
    virtual LArAutoCorrTransType*  createTransient ();
    LArAutoCorrTransType*          createTransient(LArConditionsSubset<LArAutoCorrP>* orig);
};

class LArAutoCorrCopy
{
public:
    // Copy LArAutoCorrP subset to LArAutoCorrP1
    void copyOldtoNew(const LArConditionsSubset<LArAutoCorrP>* oldAutoCorr,
                      LArConditionsSubset<LArAutoCorrP1>* newAutoCorr);
};

#endif

