/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassBitsCollectionCnv_p1
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGPASSBITSCONTAINERCNV_P1_H 
#define TRIGSTEERINGEVENTTPCNV_TRIGPASSBITSCONTAINERCNV_P1_H 
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< TrigPassBitsCollection, TrigPassBitsCollection_p1, TrigPassBitsCnv_p1 > TrigPassBitsCollectionCnv_p1;

#endif
