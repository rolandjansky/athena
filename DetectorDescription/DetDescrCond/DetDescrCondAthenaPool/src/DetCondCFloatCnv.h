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


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "DetDescrCondTPCnv/DetCondCFloatCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<DetCondCFloat,
                             DetCondCFloatCnv_p1,
                             T_TPCnvNull<DetCondCFloat> >
  DetCondCFloatCnv;


#endif


