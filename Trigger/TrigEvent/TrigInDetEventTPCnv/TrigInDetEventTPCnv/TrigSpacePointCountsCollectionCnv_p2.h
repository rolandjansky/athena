/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollectionCnv_p2
 *
 * @brief transient-persistent converter for TrigSpacePointCountsCollection_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollectionCnv_p2.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"

class TrigSpacePointCountsCollectionCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigSpacePointCountsCollection, TrigSpacePointCountsCollection_p2, TrigSpacePointCountsCnv_p2 >

{
 
 public:
  TrigSpacePointCountsCollectionCnv_p2() {}
 
}; //end of class definitions
 
#endif
