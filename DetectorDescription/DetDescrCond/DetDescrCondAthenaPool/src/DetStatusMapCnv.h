/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file DetStatusMapCnv.h
 * @brief Header file for AthenaPool converter for DetStatusMap
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef DETSTATUSMAPCNV_H
#define DETSTATUSMAPCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DetDescrConditions/DetStatusMap.h"
#include "DetDescrCondTPCnv/DetStatusMap_p1.h"

// the latest persistent representation type of DetStatusMap
typedef  DetStatusMap_p1  DetStatusMap_PERS;
typedef  T_AthenaPoolCustomCnv<DetStatusMap, DetStatusMap_PERS >   DetStatusMapCnvBase;

class DetStatusMapCnv : public DetStatusMapCnvBase {
    friend class CnvFactory<DetStatusMapCnv >;

protected:
    DetStatusMapCnv (ISvcLocator* svcloc);
    virtual DetStatusMap_PERS*   createPersistent (DetStatusMap* transObj);
    virtual DetStatusMap*        createTransient ();
};

#endif


