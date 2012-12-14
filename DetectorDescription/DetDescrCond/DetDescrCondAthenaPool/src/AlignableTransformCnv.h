/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file AlignableTransformCnv.h
 * @brief Header file for AthenaPool converter for AlignableTransform
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef ALIGNABLETRANSFORMCNV_H
#define ALIGNABLETRANSFORMCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "DetDescrCondTPCnv/AlignableTransform_p1.h"

// the latest persistent representation type of AlignableTransform
typedef  AlignableTransform_p1  AlignableTransform_PERS;
typedef  T_AthenaPoolCustomCnv<AlignableTransform, AlignableTransform_PERS >   AlignableTransformCnvBase;

class AlignableTransformCnv : public AlignableTransformCnvBase {
    friend class CnvFactory<AlignableTransformCnv >;

    virtual StatusCode initialize();

protected:
    AlignableTransformCnv (ISvcLocator* svcloc);
    virtual AlignableTransform_PERS*   createPersistent (AlignableTransform* transObj);
    virtual AlignableTransform*        createTransient ();
};

#endif

