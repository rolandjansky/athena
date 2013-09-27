/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCollectionCnv_p1
 *
 * @brief transient-persistent converter for TrigSpacePointCountsCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCollectionCnv_p1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"

class TrigSpacePointCountsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigSpacePointCountsCollection, TrigSpacePointCountsCollection_p1, ITPConverterFor<TrigSpacePointCounts> >
{
 
 public:
  TrigSpacePointCountsCollectionCnv_p1() {}
 
}; //end of class definitions
 
#endif
