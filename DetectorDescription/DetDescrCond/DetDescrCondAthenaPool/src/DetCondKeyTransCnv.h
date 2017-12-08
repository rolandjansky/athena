/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file DetCondKeyTransCnv.h
 * @brief Header file for AthenaPool converter for DetCondKeyTrans
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef DETCONDKEYTRANSCNV_H
#define DETCONDKEYTRANSCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondTPCnv/DetCondKeyTrans_p1.h"

// the latest persistent representation type of DetCondKeyTrans
typedef  DetCondKeyTrans_p1  DetCondKeyTrans_PERS;
typedef  T_AthenaPoolCustomCnv<DetCondKeyTrans, DetCondKeyTrans_PERS >   DetCondKeyTransCnvBase;

class DetCondKeyTransCnv : public DetCondKeyTransCnvBase {
    friend class CnvFactory<DetCondKeyTransCnv >;

protected:
    DetCondKeyTransCnv (ISvcLocator* svcloc);
    virtual DetCondKeyTrans_PERS*   createPersistent (DetCondKeyTrans* transObj);
    virtual DetCondKeyTrans*        createTransient ();
};

#endif

