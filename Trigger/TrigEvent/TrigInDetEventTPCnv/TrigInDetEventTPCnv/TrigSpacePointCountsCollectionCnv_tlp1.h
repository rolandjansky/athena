/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigSpacePointCountsCollection_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_p1.h"   
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h" 
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p3.h"

 
class TrigSpacePointCountsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigSpacePointCountsCollectionCnv_p1, TrigSpacePointCountsCollection_tlp1 >
{
 
 public:
 
  TrigSpacePointCountsCollectionCnv_tlp1();
  virtual ~TrigSpacePointCountsCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigSpacePointCountsCollection_tlp1 *storage );
 
 protected: 
  TrigSpacePointCountsCollectionCnv_p1    m_trigSpacePointCountsCollectionCnv;
  TrigSpacePointCountsCnv_p1              m_trigSpacePointCountsCnv_p1;
  TrigSpacePointCountsCnv_p2              m_trigSpacePointCountsCnv_p2;
  TrigSpacePointCountsCnv_p3              m_trigSpacePointCountsCnv_p3;

}; //end of class definitions
 


#endif
