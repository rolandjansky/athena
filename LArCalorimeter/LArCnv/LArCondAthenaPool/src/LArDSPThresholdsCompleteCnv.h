/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArDSPThresholdsCompleteCnv.h
 * @brief AthenaPool converter LArDSPThresholdsComplete
 * @author Walter Lampl (@cern.ch)
 */

#ifndef LArDSPThresholdsCompleteCnv_H
#define LArDSPThresholdsCompleteCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "LArRawConditions/LArDSPThresholdsP.h"
#include "LArCondTPCnv/LArDSPThresholdsSubsetCnv_p1.h"

// the latest persistent representation type of LArDSPThresholdsComplete
typedef LArConditionsSubset<LArDSPThresholdsP> LArDSPThrTransType;
typedef LArDSPThresholdsSubset_p1              LArDSPThrPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArDSPThresholdsComplete, LArDSPThrTransType, LArDSPThrPersType > LArDSPThresholdsCompleteCnvBase;

class LArDSPThresholdsCompleteCnv : public LArDSPThresholdsCompleteCnvBase {
friend class CnvFactory<LArDSPThresholdsCompleteCnv >;
protected:
    LArDSPThresholdsCompleteCnv (ISvcLocator* svcloc) : LArDSPThresholdsCompleteCnvBase(svcloc) {}
    virtual LArDSPThrPersType*   createPersistent (LArDSPThrTransType* transObj);
    virtual LArDSPThrTransType*  createTransient ();
};


#endif

