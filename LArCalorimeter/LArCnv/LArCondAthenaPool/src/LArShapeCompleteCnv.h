/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArShapeCompleteCnv.h
 * @brief AthenaPool converter LArShapeComplete
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArShapeCompleteCnv_H
#define LArShapeCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArShapeP.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p2.h"

// the latest persistent representation type of LArShapeComplete
typedef LArConditionsSubset<LArShapeP2> LArShapeTransType2;
typedef LArShapeSubset_p1               LArShapePersType1;
typedef LArShapeSubset_p2               LArShapePersType2;

typedef T_AthenaPoolCoolMultChanCnv<LArShapeComplete, LArShapeTransType2, LArShapePersType2 > LArShapeCompleteCnvBase;

class LArShapeCompleteCnv : public LArShapeCompleteCnvBase {
friend class CnvFactory<LArShapeCompleteCnv >;

protected:
public:
    LArShapeCompleteCnv (ISvcLocator* svcloc) : LArShapeCompleteCnvBase(svcloc) {}
protected:
    virtual LArShapePersType2*   createPersistent (LArShapeTransType2* transObj);
    virtual LArShapeTransType2*  createTransient ();
    LArShapeTransType1*          createTransient(LArConditionsSubset<LArShapeP>* orig);
};

class LArShapeCopy
{
public:
    // Copy LArShapeP subset to LArShapeP1
    void copyOldtoNew(const LArConditionsSubset<LArShapeP>* oldShape,
                      LArConditionsSubset<LArShapeP1>* newShape);
};

#endif

