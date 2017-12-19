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


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "DetDescrConditions/DetStatusMap.h"
#include "DetDescrCondTPCnv/DetStatusMapCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<DetStatusMap,
                             DetStatusMapCnv_p1,
                             T_TPCnvNull<DetStatusMap> >
  DetStatusMapCnv;


#endif


