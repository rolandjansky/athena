/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollectionCnv_p2
 *
 * @brief transient persistent converter for TrigRoiDescriptorCollectionCnv
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p2_H
#define TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p2.h"
 



//typedef T_AthenaPoolTPCnvVector< MuonFeatureDetailsContainer, MuonFeatureDetailsContainer_p2, MuonFeatureDetailsCnv_p2 > MuonFeatureDetailsContainerCnv_p2;
typedef T_AthenaPoolTPCnvVector< TrigRoiDescriptorCollection, TrigRoiDescriptorCollection_p2, TrigRoiDescriptorCnv_p2 > TrigRoiDescriptorCollectionCnv_p2;

#endif
