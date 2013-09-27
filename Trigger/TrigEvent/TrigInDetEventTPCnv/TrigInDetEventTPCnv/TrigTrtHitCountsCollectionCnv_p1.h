/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigTrtHitCountsCollection
 *
 * @author W.Bell@cern.ch  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p1.h"
 
class TrigTrtHitCountsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTrtHitCountsCollection, TrigTrtHitCountsCollection_p1, TrigTrtHitCountsCnv_p1 >
{
 
 public:
  TrigTrtHitCountsCollectionCnv_p1() {}
 
}; //end of class definitions
 


#endif
