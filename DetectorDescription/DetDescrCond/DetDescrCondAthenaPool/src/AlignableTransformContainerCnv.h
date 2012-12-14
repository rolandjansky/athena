/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file AlignableTransformContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef AlignableTransformContainerCnv_H
#define AlignableTransformContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "DetDescrCondTPCnv/AlignableTransform_p1.h"

// the latest persistent representation type of AlignableTransform
typedef  AlignableTransformContainer::base_value_type  AlignableTransform_TRANS;
typedef  AlignableTransform_p1                         AlignableTransform_PERS;

typedef T_AthenaPoolCoolMultChanCnv<AlignableTransformContainer, AlignableTransform_TRANS, AlignableTransform_PERS > AlignableTransformContainerCnvBase;

class AlignableTransformContainerCnv : public AlignableTransformContainerCnvBase {
friend class CnvFactory<AlignableTransformContainerCnv >;

    virtual StatusCode initialize();

protected:
    AlignableTransformContainerCnv (ISvcLocator* svcloc) : AlignableTransformContainerCnvBase(svcloc) {}
    virtual AlignableTransform_PERS*   createPersistent (AlignableTransform_TRANS* transObj);
    virtual AlignableTransform_TRANS*  createTransient ();
};

#endif

