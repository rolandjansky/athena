/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArShapeMCCnv.h
 * @brief AthenaPool converter LArShapeMC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArShapeMCCnv_H
#define LArShapeMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArShapeMC.h"
#undef private
#undef protected
#include "LArRawConditions/LArShapeP.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"

// the latest persistent representation type of LArShapeMC
typedef LArConditionsSubset<LArShapeP1> LArShapeTransType1;
typedef LArShapeSubset_p1               LArShapePersType1;

typedef T_AthenaPoolCoolMultChanCnv<LArShapeMC, LArShapeTransType1, LArShapePersType1 > LArShapeMCCnvBase;

class LArShapeMCCnv : public LArShapeMCCnvBase {
friend class CnvFactory<LArShapeMCCnv >;

protected:
    LArShapeMCCnv (ISvcLocator* svcloc) : LArShapeMCCnvBase(svcloc) {}
    virtual LArShapePersType1*   createPersistent (LArShapeTransType1* transObj);
    virtual LArShapeTransType1*  createTransient ();
    LArShapeTransType1*          createTransient(LArConditionsSubset<LArShapeP>* orig);
};

#endif

