/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigVertexCollectionCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigVertexCollection
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: TrigOperationalInfoCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_CNV_TLP1_H
#define  TRIGSTEERINGEVENTTPCNV_TrigOperationalInfoCollection_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_tlp1.h"
 
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_p1.h"   

 
class TrigOperationalInfoCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigOperationalInfoCollectionCnv_p1, TrigOperationalInfoCollection_tlp1 >
{
 
 public:
 
  TrigOperationalInfoCollectionCnv_tlp1();
  virtual ~TrigOperationalInfoCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigOperationalInfoCollection_tlp1 *storage );
  
 protected: 
  TrigOperationalInfoCollectionCnv_p1    m_TrigOperationalInfoCollectionCnv;
  TrigOperationalInfoCnv_p1              m_TrigOperationalInfoCnv;

 
}; //end of class definitions

 
#endif
