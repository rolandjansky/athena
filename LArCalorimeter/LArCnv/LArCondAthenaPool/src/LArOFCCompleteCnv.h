/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArOFCCompleteCnv.h
 * @brief AthenaPool converter LArOfcComplete
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArOFCCompleteCnv_H
#define LArOFCCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArOFCComplete.h"
#undef private
#undef protected
#include "LArRawConditions/LArOFCP.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"

// the latest persistent representation type of LArOFCComplete
typedef LArConditionsSubset<LArOFCP1> LArOFCTransType;
typedef LArOFCSubset_p1               LArOFCPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArOFCComplete, LArOFCTransType, LArOFCPersType > LArOFCCompleteCnvBase;

class LArOFCCompleteCnv : public LArOFCCompleteCnvBase {
friend class CnvFactory<LArOFCCompleteCnv >;
protected:
    LArOFCCompleteCnv (ISvcLocator* svcloc) : LArOFCCompleteCnvBase(svcloc) {}
    virtual LArOFCPersType*    createPersistent (LArOFCTransType* transObj);
    virtual LArOFCTransType*   createTransient();
    LArOFCTransType*           createTransient(LArConditionsSubset<LArOFCP>* );
};

class LArOFCCopy
{
public:
    // Copy LArOFCP subset to LArOFCP1
    void copyOldtoNew(const LArConditionsSubset<LArOFCP>* oldOFC,
                      LArConditionsSubset<LArOFCP1>* newOFC);
};

#endif

