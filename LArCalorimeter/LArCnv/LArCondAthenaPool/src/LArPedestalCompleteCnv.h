/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArPedestalCompleteCnv.h
 * @brief AthenaPool converter LArPedestalComplete
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArPedestalCompleteCnv_H
#define LArPedestalCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArPedestalComplete.h"
#undef private
#undef protected
#include "LArRawConditions/LArPedestalP.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p2.h"

// the latest persistent representation type of LArPedestalComplete
typedef LArConditionsSubset<LArPedestalP1> LArPedTransType;
typedef LArPedestalSubset_p2               LArPedPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArPedestalComplete, LArPedTransType, LArPedPersType > LArPedestalCompleteCnvBase;

class LArPedestalCompleteCnv : public LArPedestalCompleteCnvBase {
friend class CnvFactory<LArPedestalCompleteCnv >;
protected:
    LArPedestalCompleteCnv (ISvcLocator* svcloc) : LArPedestalCompleteCnvBase(svcloc) {}
    virtual LArPedPersType*   createPersistent (LArPedTransType* transObj);
    virtual LArPedTransType*  createTransient ();
    LArPedTransType*          createTransient(LArConditionsSubset<LArPedestalP>* orig);
};

class LArPedestalCopy
{
public:
    // Copy LArPedestalP subset to LArPedestalP1
    void copyOldtoNew(const LArConditionsSubset<LArPedestalP>* oldPedestal,
                      LArConditionsSubset<LArPedestalP1>* newPedestal);
};

#endif

