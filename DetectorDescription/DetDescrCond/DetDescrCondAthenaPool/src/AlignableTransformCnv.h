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


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<AlignableTransform,
                             AlignableTransformCnv_p1,
                             T_TPCnvNull<AlignableTransform> >
  AlignableTransformCnv;


#endif

