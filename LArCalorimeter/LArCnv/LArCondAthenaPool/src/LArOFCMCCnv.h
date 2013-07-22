/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file LArOFCMCCnv.h
 * @brief AthenaPool converter LArOfcMC
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef LArOFCMCCnv_H
#define LArOFCMCCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#define private public
#define protected public
#include "LArRawConditions/LArOFCMC.h"
#undef private
#undef protected
#include "LArRawConditions/LArOFCP.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"

// the latest persistent representation type of LArOFCMC
typedef LArConditionsSubset<LArOFCP1> LArOFCTransType;
typedef LArOFCSubset_p1               LArOFCPersType;

typedef T_AthenaPoolCoolMultChanCnv<LArOFCMC, LArOFCTransType, LArOFCPersType > LArOFCMCCnvBase;

class LArOFCMCCnv : public LArOFCMCCnvBase {
friend class CnvFactory<LArOFCMCCnv >;
protected:
    LArOFCMCCnv (ISvcLocator* svcloc) : LArOFCMCCnvBase(svcloc) {}
    virtual LArOFCPersType*   createPersistent (LArOFCTransType* transObj);
    virtual LArOFCTransType*  createTransient();
    LArOFCTransType*          createTransient  (LArConditionsSubset<LArOFCP>* );
};

#endif

