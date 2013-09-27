/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigTrtHitCountsCollection_tlp1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_p1.h"   
 
class TrigTrtHitCountsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTrtHitCountsCollectionCnv_p1, TrigTrtHitCountsCollection_tlp1 >
{
 
 public:
 
  TrigTrtHitCountsCollectionCnv_tlp1();
  virtual ~TrigTrtHitCountsCollectionCnv_tlp1() {}
  
  //default methods  
  virtual void  setPStorage( TrigTrtHitCountsCollection_tlp1 *storage );
  
 protected: 
  TrigTrtHitCountsCollectionCnv_p1    m_trigTrtHitCountsCollectionCnv;
  TrigTrtHitCountsCnv_p1             m_trigTrtHitCountsCnv;
 
}; //end of class definitions
 


#endif
