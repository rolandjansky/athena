/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigRoiDescriptorCollectionCnv
 *
 *
 * File and Version Information:
 * $Id: TrigRoiDescriptorCollectionCnv_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p1_H
#define TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollectionCnv_p1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"
 
class TrigRoiDescriptorCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigRoiDescriptorCollection, TrigRoiDescriptorCollection_p1, TrigRoiDescriptorCnv_p1 >
{
 
 public:
  TrigRoiDescriptorCollectionCnv_p1 () {}
 
}; //end of class definitions
 
#endif
