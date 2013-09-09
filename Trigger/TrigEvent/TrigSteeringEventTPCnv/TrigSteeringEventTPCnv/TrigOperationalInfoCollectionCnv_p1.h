/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigOperationalInfoCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigOperationalInfoCollectionCnv
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: TrigOperationalInfoCollectionCnv_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollectionCnv_p1_H
#define TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollectionCnv_p1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"
 
class TrigOperationalInfoCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigOperationalInfoCollection, TrigOperationalInfoCollection_p1, TrigOperationalInfoCnv_p1 >
{
 
 public:
  TrigOperationalInfoCollectionCnv_p1 () {}
 
}; //end of class definitions
 
#endif
