/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCountsCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigVertexCountsCollection_tlp1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigVertexCountsCollectionCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCnv_p1.h"
  
 
class TrigVertexCountsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigVertexCountsCollectionCnv_p1, TrigVertexCountsCollection_tlp1 >
{
 
 public:
 
  TrigVertexCountsCollectionCnv_tlp1();
  virtual ~TrigVertexCountsCollectionCnv_tlp1() {}
  
  //default methods  
  virtual void  setPStorage( TrigVertexCountsCollection_tlp1 *storage );
  
 protected: 
  TrigVertexCountsCollectionCnv_p1   m_trigVertexCountsCollectionCnv;
  TrigVertexCountsCnv_p1             m_trigVertexCountsCnv_p1;
 
}; //end of class definitions
 



#endif
