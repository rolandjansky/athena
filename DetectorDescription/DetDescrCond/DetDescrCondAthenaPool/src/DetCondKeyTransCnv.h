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


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondTPCnv/DetCondKeyTransCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<DetCondKeyTrans,
                             DetCondKeyTransCnv_p1,
                             T_TPCnvNull<DetCondKeyTrans> >
  DetCondKeyTransCnv;


#endif

