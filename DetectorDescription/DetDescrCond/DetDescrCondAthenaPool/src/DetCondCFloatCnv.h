/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file DetCondCFloatCnv.h
 * @brief Header file for AthenaPool converter for DetCondCFloat
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef DETCONDCFLOATCNV_H
#define DETCONDCFLOATCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "DetDescrCondTPCnv/DetCondCFloat_p1.h"

// the latest persistent representation type of DetCondCFloat
typedef  DetCondCFloat_p1  DetCondCFloat_PERS;
typedef  T_AthenaPoolCustomCnv<DetCondCFloat, DetCondCFloat_PERS >   DetCondCFloatCnvBase;

class DetCondCFloatCnv : public DetCondCFloatCnvBase {
    friend class CnvFactory<DetCondCFloatCnv >;

    virtual StatusCode initialize();

protected:
    DetCondCFloatCnv (ISvcLocator* svcloc);
    virtual DetCondCFloat_PERS*   createPersistent (DetCondCFloat* transObj);
    virtual DetCondCFloat*        createTransient ();
};

#endif


