/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCollectionCnv_p2
 *
 * @brief transient persistent converter for TrigTrtHitCountsCollection
 *
 * @author W.Bell@cern.ch  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCOLLECTION_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p2.h"
 
typedef T_AthenaPoolTPCnvVector< TrigTrtHitCountsCollection, TrigTrtHitCountsCollection_p2, TrigTrtHitCountsCnv_p2 > TrigTrtHitCountsCollectionCnv_p2;
 

#endif
