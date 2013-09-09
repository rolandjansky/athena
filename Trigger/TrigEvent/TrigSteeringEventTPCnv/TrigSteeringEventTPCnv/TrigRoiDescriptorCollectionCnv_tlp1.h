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
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigRoiDescriptorCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollection_CNV_TLP1_H
#define  TRIGSTEERINGEVENTTPCNV_TrigRoiDescriptorCollection_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_tlp1.h"
 
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p1.h"   

 
class TrigRoiDescriptorCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigRoiDescriptorCollectionCnv_p1, TrigRoiDescriptorCollection_tlp1 >
{
 
 public:
 
  TrigRoiDescriptorCollectionCnv_tlp1();
  virtual ~TrigRoiDescriptorCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigRoiDescriptorCollection_tlp1 *storage );
  
 protected: 
  TrigRoiDescriptorCollectionCnv_p1    m_TrigRoiDescriptorCollectionCnv;
  TrigRoiDescriptorCnv_p1              m_TrigRoiDescriptorCnv;

 
}; //end of class definitions
 
#endif
