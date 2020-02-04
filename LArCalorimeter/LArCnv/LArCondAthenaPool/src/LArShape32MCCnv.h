/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArShape32MCCnv.h
 * @brief AthenaPool converter LArShape32MC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArShape32MCCnv_H
#define LArShape32MCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LArShapeP.h"
#include "LArCondTPCnv/LArShapeSubsetCnv_p1.h"

// the latest persistent representation type of LArShape32MC
typedef LArConditionsSubset<LArShapeP1> LArShapeTransType;
typedef LArShapeSubset_p1               LArShapePersType;

typedef T_AthenaPoolCoolMultChanCnv<LArShape32MC, LArShapeTransType, LArShapePersType > LArShape32MCCnvBase;

class LArShape32MCCnv : public LArShape32MCCnvBase {
friend class CnvFactory<LArShape32MCCnv >;

protected:
public:
    LArShape32MCCnv (ISvcLocator* svcloc) : LArShape32MCCnvBase(svcloc) {}
protected:
    virtual LArShapePersType*   createPersistent (LArShapeTransType* transObj);
    virtual LArShapeTransType*  createTransient ();
    LArConditionsSubset<LArShapeP1>* createTransient(LArConditionsSubset<LArShapeP>* orig);
};


#endif

