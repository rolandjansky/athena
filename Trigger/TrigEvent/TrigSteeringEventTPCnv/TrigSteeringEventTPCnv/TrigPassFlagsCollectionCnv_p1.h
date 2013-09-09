/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassFlagsCollectionCnv_p1
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGPASSFLAGSCONTAINERCNV_P1_H 
#define TRIGSTEERINGEVENTTPCNV_TRIGPASSFLAGSCONTAINERCNV_P1_H 
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< TrigPassFlagsCollection, TrigPassFlagsCollection_p1, TrigPassFlagsCnv_p1 > TrigPassFlagsCollectionCnv_p1;

#endif
