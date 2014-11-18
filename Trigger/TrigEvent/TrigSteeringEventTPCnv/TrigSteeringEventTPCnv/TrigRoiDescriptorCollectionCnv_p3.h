/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollectionCnv_p3
 *
 * @brief transient persistent converter for TrigRoiDescriptorCollectionCnv
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p3_H
#define TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"
 
typedef T_AthenaPoolTPCnvVector< TrigRoiDescriptorCollection, TrigRoiDescriptorCollection_p3, TrigRoiDescriptorCnv_p3 > TrigRoiDescriptorCollectionCnv_p3;

#endif
